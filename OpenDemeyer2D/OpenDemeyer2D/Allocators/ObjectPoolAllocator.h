#pragma once

#include <cassert>
#include <new>
#include <memory>
#include <vector>

template <typename T>
class ObjectPoolChunk final
{
public:
	ObjectPoolChunk(size_t size)
		: m_Data{ static_cast<T*>(malloc(size * sizeof(T))) }
		, m_Size{ size }
		, m_NextFreeSpace{ m_Data }
		, m_FreeSpaces{ size }
		, m_OccupiedPlaces(size, false)
	{
	}
	~ObjectPoolChunk()
	{
		free(m_Data);
	}

	ObjectPoolChunk(ObjectPoolChunk<T>&& other) noexcept
		: m_Data{ other.m_Data }
		, m_Size{ other.m_Size }
		, m_NextFreeSpace{ other.m_NextFreeSpace }
		, m_FreeSpaces{ other.m_FreeSpaces }
		, m_OccupiedPlaces(std::move(other.m_OccupiedPlaces))
	{
		other.m_FreeSpaces = 0;
		other.m_Data = nullptr;
		other.m_Size = 0;
	}
	ObjectPoolChunk& operator=(ObjectPoolChunk<T>&& other) noexcept
	{
		m_Data = other.m_Data;
		m_Size = other.m_Size;
		m_NextFreeSpace = other.m_NextFreeSpace;
		m_FreeSpaces = other.m_FreeSpaces;
		m_OccupiedPlaces = std::move(other.m_OccupiedPlaces);

		other.m_FreeSpaces = 0;
		other.m_Data = nullptr;
		other.m_Size = 0;
		return *this;
	}

	ObjectPoolChunk(const ObjectPoolChunk<T>&) = delete;
	ObjectPoolChunk& operator=(const ObjectPoolChunk<T>&) = delete;

	size_t GetMaxElementAmount() const { return m_Size; }
	size_t GetElementAmount() const { return GetMaxElementAmount() - m_FreeSpaces; }
	size_t GetFreeSpaces() const { return m_FreeSpaces; }
	bool contains(const T* address) const { return (address < m_Data + m_Size) && (address >= m_Data); }

	T* allocate()
	{
		if (!m_FreeSpaces) throw std::bad_alloc();

		assert(m_NextFreeSpace);

		T* returnValue = m_NextFreeSpace;

		size_t pos = m_NextFreeSpace - m_Data;
		m_OccupiedPlaces[pos] = true;

		--m_FreeSpaces;

		while (true)
		{
			assert(pos < m_Size);

			if (m_OccupiedPlaces[pos] == false)
			{
				m_NextFreeSpace = m_Data + pos;
				return returnValue;
			}
			else
			{
				++pos %= GetMaxElementAmount();
			}
		}
	}
	void deallocate(T* ptr)
	{
		size_t pos = ptr - m_Data;
		assert(pos < m_Size);
		m_OccupiedPlaces[pos] = false;

		++m_FreeSpaces;
		m_NextFreeSpace = ptr;
	}

private:

	T* m_Data{};
	size_t m_Size{};
	T* m_NextFreeSpace{};

	size_t m_FreeSpaces{};
	std::vector<bool> m_OccupiedPlaces;

};

template <typename T>
class ObjectPoolAllocator final
{
public:

	ObjectPoolAllocator(size_t chunkSize = 32)
		: ChunkSize{ chunkSize }
	{
		m_Chunks.reserve(16);
	}

	T* allocate()
	{
		for (auto rIt{ m_Chunks.rbegin() }; rIt != m_Chunks.rend(); ++rIt)
		{
			if (rIt->GetFreeSpaces() != 0)
			{
				return rIt->allocate();
			}
		}

		m_Chunks.emplace_back(ChunkSize);
		return m_Chunks.back().allocate();
	}

	void deallocate(T* ptr)
	{
		for (auto& chunk : m_Chunks)
		{
			if (chunk.contains(ptr))
			{
				chunk.deallocate(ptr);
				return;
			}
		}
	}

private:

	size_t ChunkSize{};
	std::vector<ObjectPoolChunk<T>> m_Chunks;


};
