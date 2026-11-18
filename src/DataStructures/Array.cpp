#include "DataStructures/Array.h"

ResultArray::ResultArray() : capacity(10), currentSize(0)
{
    data = new SearchResult[capacity];
}

ResultArray::~ResultArray()
{
    delete[] data;
}

void ResultArray::resize()
{
    capacity *= 2;
    SearchResult *newData = new SearchResult[capacity];
    for (int i = 0; i < currentSize; i++)
    {
        newData[i] = data[i];
    }
    delete[] data;
    data = newData;
}

void ResultArray::add(SearchResult result)
{
    if (currentSize == capacity)
    {
        resize();
    }
    data[currentSize] = result;
    currentSize++;
}

SearchResult ResultArray::get(int index) const
{
    if (index < 0 || index >= currentSize)
    {
        throw std::out_of_range("Index out of bounds");
    }
    return data[index];
}

int ResultArray::size() const
{
    return currentSize;
}