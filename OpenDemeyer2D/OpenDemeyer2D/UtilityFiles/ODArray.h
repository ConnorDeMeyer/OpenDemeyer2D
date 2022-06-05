#pragma once

#include <vector>
#include <sstream>

template <typename T>
class ODArray : public std::vector<T>
{
public:

	ODArray() : std::vector<T>() {}
	ODArray(const std::vector<T>& other) : std::vector<T>(other) {}

	inline void SwapRemove(size_t position)
	{
		this->operator[](position) = this->back();
		this->pop_back();
	}

	inline void SwapRemove(std::vector<T>::const_iterator position)
	{
		auto pos = position - this->begin();
		SwapRemove(pos);
	}

	/** Searches for the element and Swap Removes it.*/
	inline void SwapRemove(const T& element)
	{
		size_t size = this->size();
		for (size_t i{}; i < size; ++i)
		{
			if (this->operator[](i) == element)
			{
				SwapRemove(i);
				break;
			}
		}
	}

	/** Searched for the element in reverse and swap removes it*/
	inline void RSwapRemove(const T& element)
	{
		size_t size = this->size();
		for (size_t i{size}; i != 0; --i)
		{
			if (this->operator[](i-1) == element)
			{
				SwapRemove(i-1);
				break;
			}
		}
	}

};