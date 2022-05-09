#pragma once

#include <vector>
#include <iostream>
#include <sstream>

template <typename T>
class ODArray : public std::vector<T>
{
public:

	ODArray() : std::vector<T>() {};

	inline void SwapRemove(size_t position)
	{
		this->operator[](position) = this->back();
		this->pop_back();
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

};