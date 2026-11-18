#pragma once

#include <iostream>
#include <stdexcept>

class mystring
{
private:
    char *data;
    int len;
    int cap;

    // Manual low-level helpers (replacing <cstring>)
    static int rawLength(const char *str);
    static void rawCopy(char *dest, const char *src, int count);
    void ensureCapacity(int minCapacity);

public:
    // Constructors & Rule of Five
    mystring();
    mystring(const char *str);
    mystring(const mystring &other);
    mystring &operator=(const mystring &other);
    mystring(mystring &&other) noexcept;
    mystring &operator=(mystring &&other) noexcept;
    ~mystring();

    // Capacity & Access
    int length() const;
    int size() const;
    bool empty() const;
    const char *c_str() const;

    char &operator[](int index);
    const char &operator[](int index) const;
    char front() const;
    char &front();
    char back() const;
    char &back();

    // Modifiers (built with manual array shifting)
    void clear();
    void push_back(char ch);
    void pop_back();
    void resize(int newLength, char fillChar = '\0');
    void insert(int index, const mystring &other);
    void erase(int index, int count);
    mystring substr(int startIndex, int count) const;
    int find(char ch, int startPos = 0) const;
    int find(const char *sub, int startPos = 0) const;
    int find(const mystring &sub, int startPos = 0) const;

    // Operators
    mystring operator+(const mystring &other) const;
    mystring &operator+=(const mystring &other);
    mystring &operator+=(const char *other);
    mystring &operator+=(char ch);
    bool operator==(const mystring &other) const;
    bool operator!=(const mystring &other) const;

    // Non-member operator overloads
    friend mystring operator+(const mystring &lhs, const char *rhs);
    friend mystring operator+(const char *lhs, const mystring &rhs);
    friend mystring operator+(const mystring &lhs, char rhs);
    friend mystring operator+(char lhs, const mystring &rhs);

    friend bool operator==(const mystring &lhs, const char *rhs);
    friend bool operator==(const char *lhs, const mystring &rhs);
    friend bool operator!=(const mystring &lhs, const char *rhs);
    friend bool operator!=(const char *lhs, const mystring &rhs);

    // Stream Output & Custom Line Input (replaces std::getline)
    friend std::ostream &operator<<(std::ostream &os, const mystring &str);
    static bool getline(std::istream &is, mystring &out, bool *hadCR = nullptr);

    // Utility: Number to mystring conversion without std::string
    static mystring fromInt(int value);
};

using MyString = mystring;
