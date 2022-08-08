#pragma once

#include <cassert>
#include <vector>
#include "Mallocator.h"

/**
 * How to use:
 * new (stackAllocator) T
 * new (stackAllocator) T[]
 */

class StackAllocator final
{

	class Stack final
	{
	public:

		Stack(size_t size);
		~Stack();

		Stack(const Stack& other) = delete;
		Stack& operator=(const Stack& other) = delete;
		Stack(Stack&& other) noexcept;
		Stack& operator=(Stack&& other) noexcept;

		size_t GetHeight() const { return m_pStackPtr - m_pStackStart; }
		size_t GetStackSize() const { return m_StackSize; }
		bool Fits(const uint8_t* ptr) const;
		bool Fits(size_t size) const;
		void* allocate(size_t size);
		void Reset();

	private:
		uint8_t* m_pStackStart{};
		uint8_t* m_pStackPtr{};
		size_t m_StackSize{};
	};

public:
	
	StackAllocator(size_t stackSizes);
	~StackAllocator() = default;

	StackAllocator(const StackAllocator&) = delete;
	StackAllocator(StackAllocator&&) = delete;
	StackAllocator& operator=(const StackAllocator&) = delete;
	StackAllocator& operator=(StackAllocator&&) = delete;

	void* allocate(const size_t memorySize);

	void Reset();
	size_t GetStackHeight() const;
	size_t GetStackSize() const;
	const auto& GetStacks() const { return m_Stacks; }

	template <typename T>
	T* allocate(const size_t amount = 1)
	{
		assert(amount);
		return static_cast<T*>(allocate(sizeof(T) * amount));
	}

private:
	
	std::vector<Stack, Mallocator<Stack>> m_Stacks{};

};


inline [[nodiscard]] void* operator new(size_t size, StackAllocator& alloc)
{
	return alloc.allocate(size);
}
inline [[nodiscard]] void* operator new[](size_t size, StackAllocator& alloc)
{
	return alloc.allocate(size);
}

inline void operator delete(void*, StackAllocator&) {}
inline void operator delete[](void*, StackAllocator&) {}
