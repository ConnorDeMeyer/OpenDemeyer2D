#include "pch.h"
#include "SmallObjectAllocator.h"

#include <numeric>

// SmallObjectPool
// SmallObjectPool
// SmallObjectPool

SmallObjectPool::SmallObjectPool(size_t dataSize, size_t elementAmount)
	: m_Data{ malloc(dataSize * elementAmount) }
	, m_Size{ dataSize * elementAmount }
	, m_NextFreeSpace{ m_Data }
	, m_FreeSpaces{ elementAmount }
	, m_OccupiedPlaces(elementAmount, false)
{
	assert(m_Data);
}

SmallObjectPool::SmallObjectPool(size_t dataSize, size_t elementAmount, StackAllocator& allocator)
	: m_Data{ allocator.allocate(dataSize * elementAmount) }
	, m_Size{ dataSize * elementAmount }
	, m_NextFreeSpace{ m_Data }
	, m_FreeSpaces{ elementAmount }
	, m_OccupiedPlaces(elementAmount, false)
{
	assert(m_Data);
}

SmallObjectPool::~SmallObjectPool()
{
	free(m_Data);
}

SmallObjectPool::SmallObjectPool(SmallObjectPool&& other) noexcept
	: m_Data{ other.m_Data }
	, m_Size{ other.m_Size }
	, m_NextFreeSpace{ other.m_NextFreeSpace }
	, m_FreeSpaces{ other.m_FreeSpaces }
	, m_OccupiedPlaces(std::move(other.m_OccupiedPlaces))
{
	other.m_Size = 0;
	other.m_Data = nullptr;
	other.m_NextFreeSpace = nullptr;
	other.m_FreeSpaces = 0;
}

SmallObjectPool& SmallObjectPool::operator=(SmallObjectPool&& other) noexcept
{
	m_Data = other.m_Data;
	m_Size = other.m_Size;
	m_NextFreeSpace = other.m_NextFreeSpace;
	m_FreeSpaces = other.m_FreeSpaces;
	m_OccupiedPlaces = std::move(other.m_OccupiedPlaces);

	other.m_Size = 0;
	other.m_Data = nullptr;
	other.m_NextFreeSpace = nullptr;
	other.m_FreeSpaces = 0;
	return *this;
}

bool SmallObjectPool::contains(void* address) const
{
	return (address < static_cast<uint8_t*>(m_Data) + m_Size) && address >= m_Data;
}

void* SmallObjectPool::allocate()
{
	if (!m_FreeSpaces) throw std::bad_alloc();

	void* returnValue = m_NextFreeSpace;

	size_t pos = (static_cast<uint8_t*>(m_NextFreeSpace) - static_cast<uint8_t*>(m_Data)) / GetElementSize();
	m_OccupiedPlaces[pos] = true;

	--m_FreeSpaces;

	size_t elementSize{ GetElementSize() };

	if (m_FreeSpaces == 0)
		return returnValue;

	++pos %= GetMaxElementAmount();
	while (true)
	{
		if (m_OccupiedPlaces[pos] == false)
		{
			m_NextFreeSpace = (static_cast<uint8_t*>(m_Data) + elementSize * pos);
			return returnValue;
		}
		else
		{
			++pos %= GetMaxElementAmount();
		}
	}
}

void SmallObjectPool::deallocate(void* ptr)
{
	size_t pos = (static_cast<uint8_t*>(ptr) - static_cast<uint8_t*>(m_Data)) / GetElementSize();
	assert(pos < GetMaxElementAmount());
	m_OccupiedPlaces[pos] = false;

	++m_FreeSpaces;
	m_NextFreeSpace = ptr;
}

void* SmallObjectPool::end() const
{
	return static_cast<uint8_t*>(m_Data) + m_Size;
}

// SmallObjectAllocator
// SmallObjectAllocator
// SmallObjectAllocator

SmallObjectAllocator::SmallObjectAllocator()
{
	initialize();
}

void* SmallObjectAllocator::allocate(size_t count)
{
	assert(count);

	if (count > MaxElementSize)
	{
		auto ptr = malloc(count);
		assert(ptr);
		return ptr;
	}

	auto& pools{ m_Pools[count - 1] };
	for (SmallObjectPool& pool : pools)
	{
		if (pool.GetFreeSpaces())
		{
			auto ptr = pool.allocate();
			assert(ptr);
#ifdef _DEBUG
			m_MemoryAsserter.emplace(size_t(ptr), count);
#endif
			return ptr;
		}
	}
	auto& pool = pools.emplace_back(count, pools.back().GetMaxElementAmount() * 2);
	m_AddressLookUpSet.insert(AddressLookUp(pool.data(), count, pool.end()));
	auto ptr = pool.allocate();
	assert(ptr);
#ifdef _DEBUG
	m_MemoryAsserter.emplace(size_t(ptr), count);
#endif
	return ptr;
}

void SmallObjectAllocator::deallocate(void* ptr)
{
	if (!ptr)
		return;

	auto lower_bound = --m_AddressLookUpSet.end();
	if (ptr < m_AddressLookUpSet.rbegin()->address)
	{
		lower_bound = m_AddressLookUpSet.lower_bound(AddressLookUp(ptr, 0));
		if (!(lower_bound != m_AddressLookUpSet.end() && lower_bound->address == ptr) && --lower_bound == m_AddressLookUpSet.end())
		{
#ifdef _DEBUG
			assert(!m_MemoryAsserter.contains(size_t(ptr)));
#endif
			free(ptr);
			return;
		}
	}
	
	assert(ptr >= lower_bound->address);
	if (ptr < lower_bound->poolEnd)
	{
		deallocate(ptr, lower_bound->elementSize);
	}
	else
	{
#ifdef _DEBUG
		assert(!m_MemoryAsserter.contains(size_t(ptr)));
#endif
		free(ptr);
	}
}

void SmallObjectAllocator::deallocate(void* ptr, size_t count)
{
	if (count <= MaxElementSize)
	{
		auto& pools{ m_Pools[count - 1] };
		for (SmallObjectPool& pool : pools)
		{
			assert(pool.GetElementSize() == count);
			if (pool.contains(ptr))
			{
#ifdef _DEBUG
				auto it = m_MemoryAsserter.find(size_t(ptr));
				assert(it != m_MemoryAsserter.end());
				assert(it->second == count);
				m_MemoryAsserter.erase(it);
#endif
				pool.deallocate(ptr);
				return;
			}
		}
	}
	else
	{
#ifdef _DEBUG
		assert(!m_MemoryAsserter.contains(size_t(ptr)));
#endif
		free(ptr);
	}
}

void SmallObjectAllocator::initialize()
{
	for (size_t i{ 1 }; i <= MaxElementSize; ++i)
	{
		size_t size = InitialMaxSize;
		size *= i & 0b11 ? 1 : Multiple4Multiplier;
		size *= i & 0b111 ? 1 : Multiple8Multiplier;
		size /= i;
		auto& pool = m_Pools[i - 1].emplace_back(i, size);
		m_AddressLookUpSet.insert(AddressLookUp{ pool.data(), i, pool.end() });
	}
}
