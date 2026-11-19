#include "DataStructures/Array.h"

ResultArray::ResultArray() : capacity(10), currentSize(0)
{
    data = new SearchResult[capacity];
}

ResultArray::ResultArray(const ResultArray &other)
    : data(new SearchResult[other.capacity]), capacity(other.capacity), currentSize(other.currentSize)
{
    for (int i = 0; i < currentSize; i++)
    {
        data[i] = other.data[i];
    }
}

ResultArray &ResultArray::operator=(const ResultArray &other)
{
    if (this == &other)
        return *this;

    SearchResult *newData = new SearchResult[other.capacity];
    for (int i = 0; i < other.currentSize; i++)
    {
        newData[i] = other.data[i];
    }
    delete[] data;
    data = newData;
    capacity = other.capacity;
    currentSize = other.currentSize;
    return *this;
}

ResultArray::ResultArray(ResultArray &&other) noexcept
    : data(other.data), capacity(other.capacity), currentSize(other.currentSize)
{
    other.data = nullptr;
    other.capacity = 0;
    other.currentSize = 0;
}

ResultArray &ResultArray::operator=(ResultArray &&other) noexcept
{
    if (this == &other)
        return *this;

    delete[] data;
    data = other.data;
    capacity = other.capacity;
    currentSize = other.currentSize;

    other.data = nullptr;
    other.capacity = 0;
    other.currentSize = 0;
    return *this;
}

ResultArray::~ResultArray()
{
    delete[] data;
}

void ResultArray::resize()
{
        capacity = (capacity > 0) ? capacity * 2 : 10;
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