#pragma once

#include <vector>
#include <iostream>

template <typename T>
class ODArray : public std::vector<T>
{
public:

	ODArray(){}

	inline void SwapRemove(size_t position)
	{
		this->operator[](position) = this->back();
		this->pop_back();
	}

	//inline void SwapRemove(std::vector<T>::const_iterator position)
	//{
	//	const size_t posInArray = this->begin() - position;
	//	SwapRemove(posInArray);
	//}

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

template <typename T>
std::ostream& operator<<(const ODArray<T>& arr, std::ostream& os)
{
	for (auto ele : arr)
	{
		os << ele;
	}
	return os;
}
