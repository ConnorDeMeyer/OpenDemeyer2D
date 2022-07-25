#pragma once

#include <vector>
#include <array>
#include <set>
#include <unordered_map>

#include "Mallocator.h"

class SmallObjectPool final
{
public:

	SmallObjectPool(size_t dataSize, size_t elementAmount);
	~SmallObjectPool();

	SmallObjectPool(SmallObjectPool&& other) noexcept;
	SmallObjectPool& operator=(SmallObjectPool&& other) noexcept;

	SmallObjectPool(const SmallObjectPool&) = delete;
	SmallObjectPool& operator=(const SmallObjectPool&) = delete;

	size_t GetMaxElementAmount() const { return m_OccupiedPlaces.size(); }
	size_t GetCurrentElementAmount() const { return GetMaxElementAmount() - m_FreeSpaces; }
	size_t GetFreeSpaces() const { return m_FreeSpaces; }
	bool contains(void* address) const;
	size_t GetElementSize() const { return m_Size / m_OccupiedPlaces.size(); }

	void* allocate();
	void deallocate(void* ptr);

	void* data() const { return m_Data; }

	void* end() const;
private:



private:

	void* m_Data{};
	size_t m_Size{};
	void* m_NextFreeSpace{};

	size_t m_FreeSpaces{};
	std::vector<bool, Mallocator<bool>> m_OccupiedPlaces;
};

class SmallObjectAllocator final
{
	static const size_t InitialMaxSize{ 512 * 8 };
	static const size_t MaxElementSize{ 256 };

public:

	SmallObjectAllocator();

	void* allocate(size_t count);

	void deallocate(void* ptr);

	void deallocate(void* ptr, size_t count);

	template <typename T>
	T* allocate()
	{
		return allocate(sizeof(T));
	}

	template <typename T>
	void deallocate(T* ptr)
	{
		deallocate(ptr, sizeof(T));
	}

	const auto& GetPools() const { return m_Pools; }

private:

	void initialize();

private:

	std::array<std::vector<SmallObjectPool, Mallocator<SmallObjectPool>>, MaxElementSize> m_Pools;

private:

	struct AddressLookUp
	{
		void* address{};
		size_t elementSize{};
		void* poolEnd{};
	};

	struct AddressLookUpComp
	{
		bool operator()(const AddressLookUp& lhs, const AddressLookUp& rhs) const { return lhs.address < rhs.address; }
	};

	std::set<AddressLookUp, AddressLookUpComp, Mallocator<AddressLookUp>> m_AddressLookUpSet;

#ifdef _DEBUG
	std::unordered_map<size_t, size_t, std::hash<size_t>, std::equal_to<size_t>, Mallocator<std::pair<const size_t, size_t>>> m_MemoryAsserter;
#endif
};