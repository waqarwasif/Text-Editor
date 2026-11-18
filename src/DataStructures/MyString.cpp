#include "DataStructures/MyString.h"

int mystring::rawLength(const char *str)
{
    if (str == nullptr)
        return 0;
    int len = 0;
    while (str[len] != '\0')
    {
        len++;
    }
    return len;
}

void mystring::rawCopy(char *dest, const char *src, int count)
{
    for (int i = 0; i < count; ++i)
    {
        dest[i] = src[i];
    }
}

void mystring::ensureCapacity(int minCapacity)
{
    if (minCapacity <= cap && data != nullptr)
        return;

    int newCap = (cap > 0) ? cap * 2 : 16;
    while (newCap < minCapacity)
    {
        newCap *= 2;
    }

    char *newData = new char[newCap];
    if (data != nullptr && len > 0)
    {
        rawCopy(newData, data, len);
    }
    newData[len] = '\0';

    delete[] data;
    data = newData;
    cap = newCap;
}

mystring::mystring() : data(new char[16]), len(0), cap(16)
{
    data[0] = '\0';
}

mystring::mystring(const char *str)
{
    len = rawLength(str);
    cap = (len + 1 > 16) ? (len + 1) * 2 : 16;
    data = new char[cap];
    if (str != nullptr && len > 0)
    {
        rawCopy(data, str, len);
    }
    data[len] = '\0';
}

mystring::mystring(const mystring &other) : len(other.len), cap(other.cap)
{
    data = new char[cap];
    rawCopy(data, other.data, len);
    data[len] = '\0';
}

mystring &mystring::operator=(const mystring &other)
{
    if (this == &other)
        return *this;

    char *newData = new char[other.cap];
    rawCopy(newData, other.data, other.len);
    newData[other.len] = '\0';

    delete[] data;
    data = newData;
    len = other.len;
    cap = other.cap;
    return *this;
}

mystring::mystring(mystring &&other) noexcept : data(other.data), len(other.len), cap(other.cap)
{
    other.data = nullptr;
    other.len = 0;
    other.cap = 0;
}

mystring &mystring::operator=(mystring &&other) noexcept
{
    if (this == &other)
        return *this;

    delete[] data;
    data = other.data;
    len = other.len;
    cap = other.cap;

    other.data = nullptr;
    other.len = 0;
    other.cap = 0;
    return *this;
}

mystring::~mystring()
{
    delete[] data;
}

int mystring::length() const { return len; }
int mystring::size() const { return len; }
bool mystring::empty() const { return len == 0; }

const char *mystring::c_str() const
{
    return data != nullptr ? data : "";
}

char &mystring::operator[](int index)
{
    if (index < 0 || index >= len)
        throw std::out_of_range("mystring index out of range.");
    return data[index];
}

const char &mystring::operator[](int index) const
{
    if (index < 0 || index >= len)
        throw std::out_of_range("mystring index out of range.");
    return data[index];
}

char mystring::front() const
{
    if (len == 0)
        throw std::out_of_range("mystring is empty.");
    return data[0];
}

char &mystring::front()
{
    if (len == 0)
        throw std::out_of_range("mystring is empty.");
    return data[0];
}

char mystring::back() const
{
    if (len == 0)
        throw std::out_of_range("mystring is empty.");
    return data[len - 1];
}

char &mystring::back()
{
    if (len == 0)
        throw std::out_of_range("mystring is empty.");
    return data[len - 1];
}

void mystring::clear()
{
    len = 0;
    if (data != nullptr)
        data[0] = '\0';
}

void mystring::push_back(char ch)
{
    ensureCapacity(len + 2);
    data[len] = ch;
    len++;
    data[len] = '\0';
}

void mystring::pop_back()
{
    if (len > 0)
    {
        len--;
        data[len] = '\0';
    }
}

void mystring::resize(int newLength, char fillChar)
{
    if (newLength < 0)
        throw std::out_of_range("Negative resize length.");

    if (newLength > len)
    {
        ensureCapacity(newLength + 1);
        for (int i = len; i < newLength; ++i)
        {
            data[i] = fillChar;
        }
    }
    len = newLength;
    if (data != nullptr)
        data[len] = '\0';
}

void mystring::insert(int index, const mystring &other)
{
    if (index < 0 || index > len)
        throw std::out_of_range("Insert index out of range.");
    if (other.len == 0)
        return;

    ensureCapacity(len + other.len + 1);

    for (int i = len - 1; i >= index; --i)
    {
        data[i + other.len] = data[i];
    }
    for (int j = 0; j < other.len; ++j)
    {
        data[index + j] = other.data[j];
    }

    len += other.len;
    data[len] = '\0';
}

void mystring::erase(int index, int count)
{
    if (index < 0 || index >= len || count <= 0)
        throw std::out_of_range("Erase index or count out of range.");

    if (index + count > len)
        count = len - index;

    for (int i = index; i < len - count; ++i)
    {
        data[i] = data[i + count];
    }

    len -= count;
    data[len] = '\0';
}

mystring mystring::substr(int startIndex, int count) const
{
    if (startIndex < 0 || startIndex > len || count < 0)
        throw std::out_of_range("Substring range out of range.");

    if (startIndex + count > len)
        count = len - startIndex;

    mystring result;
    result.ensureCapacity(count + 1);
    for (int i = 0; i < count; ++i)
    {
        result.data[i] = data[startIndex + i];
    }
    result.len = count;
    result.data[count] = '\0';
    return result;
}

int mystring::find(char ch, int startPos) const
{
    if (startPos < 0 || startPos >= len)
        return -1;
    for (int i = startPos; i < len; ++i)
    {
        if (data[i] == ch)
            return i;
    }
    return -1;
}

int mystring::find(const char *sub, int startPos) const
{
    if (sub == nullptr || startPos < 0)
        return -1;
    int subLen = rawLength(sub);
    if (subLen == 0)
        return startPos <= len ? startPos : -1;
    if (startPos + subLen > len)
        return -1;

    for (int i = startPos; i <= len - subLen; ++i)
    {
        bool match = true;
        for (int j = 0; j < subLen; ++j)
        {
            if (data[i + j] != sub[j])
            {
                match = false;
                break;
            }
        }
        if (match)
            return i;
    }
    return -1;
}

int mystring::find(const mystring &sub, int startPos) const
{
    return find(sub.c_str(), startPos);
}

mystring mystring::operator+(const mystring &other) const
{
    mystring result(*this);
    result += other;
    return result;
}

mystring &mystring::operator+=(const mystring &other)
{
    if (other.len == 0)
        return *this;

    ensureCapacity(len + other.len + 1);
    rawCopy(data + len, other.data, other.len);
    len += other.len;
    data[len] = '\0';
    return *this;
}

mystring &mystring::operator+=(const char *other)
{
    if (other == nullptr)
        return *this;
    int otherLen = rawLength(other);
    if (otherLen == 0)
        return *this;

    ensureCapacity(len + otherLen + 1);
    rawCopy(data + len, other, otherLen);
    len += otherLen;
    data[len] = '\0';
    return *this;
}

mystring &mystring::operator+=(char ch)
{
    push_back(ch);
    return *this;
}

bool mystring::operator==(const mystring &other) const
{
    if (len != other.len)
        return false;
    for (int i = 0; i < len; ++i)
    {
        if (data[i] != other.data[i])
            return false;
    }
    return true;
}

bool mystring::operator!=(const mystring &other) const
{
    return !(*this == other);
}

mystring operator+(const mystring &lhs, const char *rhs)
{
    mystring res(lhs);
    res += rhs;
    return res;
}

mystring operator+(const char *lhs, const mystring &rhs)
{
    mystring res(lhs);
    res += rhs;
    return res;
}

mystring operator+(const mystring &lhs, char rhs)
{
    mystring res(lhs);
    res += rhs;
    return res;
}

mystring operator+(char lhs, const mystring &rhs)
{
    char buf[2] = {lhs, '\0'};
    mystring res(buf);
    res += rhs;
    return res;
}

bool operator==(const mystring &lhs, const char *rhs)
{
    if (rhs == nullptr)
        return false;
    int rLen = mystring::rawLength(rhs);
    if (lhs.len != rLen)
        return false;
    for (int i = 0; i < lhs.len; ++i)
    {
        if (lhs.data[i] != rhs[i])
            return false;
    }
    return true;
}

bool operator==(const char *lhs, const mystring &rhs)
{
    return rhs == lhs;
}

bool operator!=(const mystring &lhs, const char *rhs)
{
    return !(lhs == rhs);
}

bool operator!=(const char *lhs, const mystring &rhs)
{
    return !(rhs == lhs);
}

std::ostream &operator<<(std::ostream &os, const mystring &str)
{
    if (str.data != nullptr)
    {
        os << str.data;
    }
    return os;
}

bool mystring::getline(std::istream &is, mystring &out, bool *hadCR)
{
    out.clear();
    char ch;
    bool readAny = false;
    if (hadCR != nullptr)
        *hadCR = false;

    while (is.get(ch))
    {
        readAny = true;
        if (ch == '\n')
            break;
        out.push_back(ch);
    }

    if (out.len > 0 && out.data[out.len - 1] == '\r')
    {
        if (hadCR != nullptr)
            *hadCR = true;
        out.pop_back();
    }

    return readAny;
}

mystring mystring::fromInt(int value)
{
    if (value == 0)
        return mystring("0");

    char buf[32];
    int idx = 0;
    bool neg = false;
    long long v = value;
    if (v < 0)
    {
        neg = true;
        v = -v;
    }
    while (v > 0)
    {
        buf[idx++] = static_cast<char>('0' + (v % 10));
        v /= 10;
    }
    if (neg)
        buf[idx++] = '-';

    for (int i = 0; i < idx / 2; ++i)
    {
        char tmp = buf[i];
        buf[i] = buf[idx - 1 - i];
        buf[idx - 1 - i] = tmp;
    }
    buf[idx] = '\0';
    return mystring(buf);
}
