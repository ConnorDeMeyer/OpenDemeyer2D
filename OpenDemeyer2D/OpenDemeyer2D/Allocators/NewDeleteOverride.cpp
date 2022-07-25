#include "pch.h"

#include "NewDeleteOverride.h"
#include "SmallObjectAllocator.h"

SmallObjectAllocator& GetSmallObjectAllocator()
{
	static SmallObjectAllocator allocator;
	return allocator;
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
