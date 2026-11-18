#pragma once
#include <stdexcept>

// Stores the coordinates of a found string
struct SearchResult
{
    int lineNumber;
    int startIndex;
};


class ResultArray
{
private:
    SearchResult *data;
    int capacity;
    int currentSize;

    void resize();

public:
    ResultArray();
    ~ResultArray();

    void add(SearchResult result);
    SearchResult get(int index) const;
    int size() const;
};