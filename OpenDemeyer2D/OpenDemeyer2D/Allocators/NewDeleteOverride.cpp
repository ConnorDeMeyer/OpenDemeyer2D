#include "pch.h"

#include "NewDeleteOverride.h"
#include "SmallObjectAllocator.h"
#include "StackAllocator.h"

constexpr size_t StackAllocatorStackSize{ 1024 * 8 };

SmallObjectAllocator& GetSmallObjectAllocator()
{
	static SmallObjectAllocator allocator;
	return allocator;
}

StackAllocator& GetStackAllocator()
{
	static StackAllocator stackAllocator(StackAllocatorStackSize);
	return stackAllocator;
}

void* operator new(size_t count)
{
	return GetSmallObjectAllocator().allocate(count);
}

void* operator new [](size_t count)
{
	return GetSmallObjectAllocator().allocate(count);
}

void operator delete(void* ptr) noexcept
{
	GetSmallObjectAllocator().deallocate(ptr);
}

void operator delete [](void* ptr) noexcept
{
	GetSmallObjectAllocator().deallocate(ptr);
}
