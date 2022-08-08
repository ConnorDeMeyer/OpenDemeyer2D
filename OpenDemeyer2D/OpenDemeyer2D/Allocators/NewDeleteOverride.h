#pragma once
#include <cstdlib>

class StackAllocator;
class SmallObjectAllocator;

SmallObjectAllocator& GetSmallObjectAllocator();
StackAllocator& GetStackAllocator();

[[nodiscard]] void* operator new(size_t count);
[[nodiscard]] void* operator new[](size_t count);

void operator delete(void* ptr) noexcept;
void operator delete[](void* ptr) noexcept;
