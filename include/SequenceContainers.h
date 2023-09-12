#pragma once





#include "ofMain.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>


template <typename T>
class DynamicArray //A DynamicArray is defined by its capacity the current number of elements and a pointer to the elements
{
public:

	DynamicArray() : size(0), capacity(0), arr(nullptr)  //data initially null, size and capacity of array both initially 0
	{
	}


	DynamicArray(size_t _capacity) : size(0), capacity(_capacity)//allocate memory for an array of size capacity where each element of the array is element_size
	{
		arr = new T[_capacity];
	}

	~DynamicArray() //allocate memory for an array of size capacity where each element of the array is element_size
	{
		if (arr != nullptr)
		{
			delete[] arr;
			arr = nullptr;
			size = 0;
			capacity = 0;
		}
	}




	DynamicArray(const DynamicArray& other) : size(other.size), capacity(other.capacity)
	{
		arr = new T[capacity];
		std::copy(other.arr, other.arr + size, arr);
	}
	DynamicArray& operator=(const DynamicArray& other)
	{
		if (this != &other)
		{
			delete[] arr;
			size = other.size;
			capacity = other.capacity;
			arr = new T[capacity];


			for (int i = 0; i < other.size; i++)
			{
				arr[i] = other[i]; //copy old array elements to new resized array 
			}


			std::copy(other.arr, other.arr + size, arr);
		}
		return(*this);
	}








	T& operator[](size_t index) //must return as reference as array element can be put on left side
	{
		if (index >= size)
		{
			//ofLog(OF_LOG_FATAL_ERROR, "Array index is out of bounds");
			//exit(1);
			throw std::runtime_error("Array index is out of bounds");
		}
		else
		{
			return(arr[index]);
		}
	}

	T& backEnd()
	{
		if (size > 0)
		{
			return(arr[size - 1]);
		}
		else
		{
			//cout << "\nVector is Empty, backEnd";
		}
	}



	/*-----------   Allocate memory for Dynamic Array   ------------*/
	void allocDynamicArray(size_t _capacity)
	{
		deallocateDynamicArray();
		capacity = _capacity;
		arr = new T[capacity];
	}


	/*-----------   Insert an element to the Dynamic Array   ------------*/
	void insertElement(const T& element)
	{
		if (size > capacity) // Defensive programming: ensure size is not beyond capacity
		{
			//ofLog(OF_LOG_FATAL_ERROR, "Array size is greater than capacity");
			throw std::runtime_error("Array size is greater than capacity");
			cout << "\n\nArray size is greater than capacity";
			//return;
		}

		if (size == capacity)
		{
			resizeDynamicArray();
		}
		arr[size] = element;
		size = size + 1;
	}


	size_t getSize() const
	{
		return size;
	}

	size_t getCapacity() const
	{
		return capacity;
	}

	/*-----------   Clear Dynamic Array   ------------*/
	void deallocateDynamicArray()
	{
		delete[] arr;
		arr = nullptr;
		size = 0;
		capacity = 0;
	}

private:

	/*-----------   Resize Dynamic Array   ------------*/
	void resizeDynamicArray()
	{
		capacity = (capacity == 0) ? 1 : (capacity * 2);//if the current size of the array is equal to the max size of the array, then increase the capacity

		T* tempArr = new T[capacity];
		for (int i = 0; i < size; i++)
		{
			tempArr[i] = arr[i]; //copy old array elements to new resized array 
		}
		//delete the old array and reassign it's values to resized array
		delete[] arr;
		arr = tempArr;

	}



	/*-----------------------   Dynamic Array Member Variables   ------------------------*/
	T* arr;  //elements of arbitrary type 
	size_t size;   //total current size of array/vector, i.e., number of elements
	size_t capacity;  // Total capacity of the allocated array, i.e., max size 
};















template <typename T>
class DynamicBufferArray
{
public:

	DynamicBufferArray() {}

	DynamicBufferArray(size_t _capacity) : size(0), capacity(_capacity)
	{
		listData = new T[_capacity];
	}



	DynamicBufferArray(const DynamicBufferArray& other)
	{
		if (other.listData.capacity == fixed_cap)
		{
			listData = other.listData;
			listData.data = listData.buffer;
		}
		else
		{
			reserveDBA(other.listData.size);
			std::copy(other.listData.data, other.listData.data + other.listData.size, listData.data);


			cout << "\n\nother.listData.size: " << other.listData.size;
			cout << "\nother.listData.capacity: " << other.listData.capacity;
			cout << "\n\nlistData.size: " << listData.size;
			cout << "\nlistData.capacity: " << listData.capacity;
		}
	}


	~DynamicBufferArray()
	{
		if (listData.data != listData.buffer)
		{
			delete[] listData.data;
		}
	}



	T& operator[](size_t index) //must return as reference as array element can be put on left side
	{
		//assert(index >= 0 && index < listData.size);
		if (index >= 0 && index < listData.size)
		{
			return(listData.data[index]);
		}
	}




	// Returns size() == 0.
	bool emptyDBA() const
	{
		return listData.size == 0;
	}

	// Returns the number of elements in the list.
	int sizeDBA() const
	{
		return listData.size;
	}




	void reserveDBA(size_t _size)
	{
		if (_size > listData.capacity)
		{
			T* newMemory = new T[_size];
			if (!newMemory)
			{
				throw std::bad_alloc();
			}
			std::copy(listData.data, listData.data + listData.capacity, newMemory);
			if (listData.data != listData.buffer)
			{
				delete[] listData.data;
			}

			listData.data = newMemory;
			listData.capacity = _size;
		}
	}

	void resizeDBA(size_t _size, const T& fill) //resizes the list to contain _size elements, filling new elements with fill
	{
		if (_size > listData.capacity)
		{
			reserveDBA(_size + 1);
		}
		if (_size > listData.size)
		{
			for (int j = listData.size; j < _size; ++j)
			{
				listData.data[j] = fill;
			}
		}
		listData.size = _size;
	}

	void pushBackDBA(const T& _data)
	{
		if (listData.size >= listData.capacity)
		{
			reserveDBA(listData.capacity * 2);
		}
		listData.data[listData.size++] = _data;
	}

	void popBackEndDBA(const T& _data)
	{
		return listData.data[--ld.size];
	}


	void clearDBA()
	{
		listData.size = 0;
	}



	T& backDBA()
	{
		assert(listData.size > 0)
		{
			return(listData.data[listData.size - 1]);
		}
	}

	T& frontDBA()
	{
		assert(listData.size > 0 && listData.size < listData.capacity)
		{
			return(listData.data[0]);
		}
	}

	void printDBA(string label)
	{
		cout << "\n\n\n\n\n\n__ " << label; cout << " __";
		cout << "\n\nlistData.size: " << listData.size;
		cout << "\nlistData.capacity: " << listData.capacity;


		//cout << "\n\n\n";
		//assert(listData.size > 0 && listData.size < listData.capacity)
		//{
		//	for (int j = 0; j < listData.size; j++)
		//	{
		//		cout << "\nlistData.data[j]: " << listData.data[j];
		//	}
		//}
	}




	//private:


	enum { fixed_cap = 64 };
	struct ListData
	{
		ListData() : data(buffer), size(0), capacity(fixed_cap) {}
		T buffer[fixed_cap];// Stores a fixed-size buffer in advance to avoid requiring a heap allocation until we run out of space.
		T* data;
		size_t size;   //total current size of array/vector, i.e., number of elements
		size_t capacity;  // Total capacity of the allocated array, i.e., max size 
	};
	ListData listData;


};















// ---------------------------------------------------------------------------------
// SmallList
// ---------------------------------------------------------------------------------
/// Provides an indexed free list with constant-time removals from anywhere
/// in the list without invalidating indices. T must be trivially constructible 
/// and destructible.
template <class T>
class FreeList
{
public:
	/// Creates a new free list.
	FreeList() : first_free(-1)
	{
	}

	/// Inserts an element to the free list and returns an index to it.
	int insert(const T& element)
	{
		if (first_free != -1)
		{
			const int index = first_free;
			first_free = data[first_free].next;
			data[index].element = element;
			return index;
		}
		else
		{
			FreeElement fe;
			fe.element = element;
			data.push_back(fe);
			return data.size() - 1;
		}
	}

	// Removes the nth element from the free list.
	void erase(int n)
	{
		assert(n >= 0 && n < data.size());
		data[n].next = first_free;
		first_free = n;
	}

	// Removes all elements from the free list.
	void clear()
	{
		data.clear();
		first_free = -1;
	}

	// Returns the range of valid indices.
	int range() const
	{
		return data.size();
	}

	// Returns the nth element.
	T& operator[](int n)
	{
		return data[n].element;
	}

	// Returns the nth element.
	const T& operator[](int n) const
	{
		return data[n].element;
	}

private:


	union FreeElement
	{
		T element;
		int next;
	};
	DynamicBufferArray<FreeElement> data;
	int first_free;
};
