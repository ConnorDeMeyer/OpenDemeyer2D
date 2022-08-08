#include "pch.h"
#include "StackAllocator.h"

#include <cassert>

StackAllocator::Stack::Stack(size_t size)
{
	m_pStackStart = static_cast<uint8_t*>(malloc(size));
	m_pStackPtr = m_pStackStart;
	m_StackSize = size;
}

StackAllocator::Stack::~Stack()
{
	free(m_pStackStart);
}

StackAllocator::Stack::Stack(Stack&& other) noexcept
	: m_pStackStart(other.m_pStackStart)
	, m_pStackPtr(other.m_pStackPtr)
	, m_StackSize(other.m_StackSize)
{
	other.m_pStackStart = nullptr;
	other.m_pStackPtr = nullptr;
	other.m_StackSize = 0;
}

StackAllocator::Stack& StackAllocator::Stack::operator=(Stack&& other) noexcept
{
	free(m_pStackStart);
	m_pStackStart = other.m_pStackStart;
	m_pStackPtr = other.m_pStackPtr;
	m_StackSize = other.m_StackSize;

	other.m_pStackStart = nullptr;
	other.m_pStackPtr = nullptr;
	other.m_StackSize = 0;
	return *this;
}

bool StackAllocator::Stack::Fits(const uint8_t* ptr) const
{
	return ptr < m_pStackStart + m_StackSize && ptr >= m_pStackStart;
}

bool StackAllocator::Stack::Fits(size_t size) const
{
	return size <= (GetStackSize() - GetHeight());
}

void* StackAllocator::Stack::allocate(size_t size)
{
	assert(Fits(size));

	auto returnValue = m_pStackPtr;
	m_pStackPtr += size;

	assert(m_pStackPtr <= m_pStackStart + m_StackSize);

	return returnValue;
}

void StackAllocator::Stack::Reset()
{
	m_pStackPtr = m_pStackStart;
}

//StackAllocator
//StackAllocator
//StackAllocator

StackAllocator::StackAllocator(size_t stackSizes)
{
	m_Stacks.emplace_back(stackSizes);
}

void* StackAllocator::allocate(const size_t memorySize)
{
	assert(memorySize <= m_Stacks.front().GetStackSize());

	for (auto rIt{m_Stacks.rbegin()}; rIt != m_Stacks.rend(); ++rIt)
	{
		if (rIt->Fits(memorySize))
		{
			return rIt->allocate(memorySize);
		}
	}

	auto& stack = m_Stacks.emplace_back(m_Stacks.front().GetStackSize());
	return stack.allocate(memorySize);
}

void StackAllocator::Reset()
{
	for (auto& stack : m_Stacks)
	{
		stack.Reset();
	}
}

size_t StackAllocator::GetStackHeight() const
{
	size_t sum{};
	for (auto& stack : m_Stacks)
	{
		sum += stack.GetHeight();
	}
	return sum;
}

size_t StackAllocator::GetStackSize() const
{
	return m_Stacks.size() * m_Stacks.front().GetStackSize();
}


