#include "Utils/TextProcessing.h"
#include <utility>

namespace TextProcessing
{
    bool isValidLineText(const mystring &str)
    {
        const int n = str.size();
        for (int i = 0; i < n; ++i)
        {
            const char c = str[i];
            if (c == '\n' || c == '\r' || c == '\0')
                return false;
        }
        return true;
    }

    TextStatus insertAt(const mystring &str, int column,
                        const mystring &text, mystring &out)
    {
        const int len = str.size();
        const int insLen = text.size();

        if (insLen == 0)
            return TextStatus::EmptyText;
        if (!isValidLineText(text))
            return TextStatus::InvalidCharacter;
        if (column < 1 || column > len + 1)
            return TextStatus::ColumnOutOfRange;

        mystring result;

        int i = 0;
        for (; i < column - 1; ++i)
            result.push_back(str[i]);
        for (int j = 0; j < insLen; ++j)
            result.push_back(text[j]);
        for (; i < len; ++i)
            result.push_back(str[i]);

        out = std::move(result);
        return TextStatus::Ok;
    }

    bool contains(const mystring &s, char c)
    {
        const int n = s.size();
        for (int i = 0; i < n; ++i)
        {
            if (s[i] == c)
                return true;
        }
        return false;
    }

    bool containsAnyOf(const mystring &s, const char *set)
    {
        if (set == nullptr)
            return false;
        const int n = s.size();
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; set[j] != '\0'; ++j)
            {
                if (s[i] == set[j])
                    return true;
            }
        }
        return false;
    }

    bool isBlank(const mystring &s)
    {
        const int n = s.size();
        for (int i = 0; i < n; ++i)
        {
            if (s[i] != ' ' && s[i] != '\t')
                return false;
        }
        return true;
    }

    static bool isTrimmable(char c)
    {
        return c == ' ' || c == '\t' || c == '\r' || c == '\n';
    }

    mystring trim(const mystring &s)
    {
        const int n = s.size();
        int first = 0;
        while (first < n && isTrimmable(s[first]))
            ++first;
        int last = n;
        while (last > first && isTrimmable(s[last - 1]))
            --last;

        mystring result;
        for (int i = first; i < last; ++i)
            result.push_back(s[i]);
        return result;
    }

    char toLowerAscii(char c)
    {
        return (c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : c;
    }

    void eraseFront(mystring &s, int count)
    {
        const int n = s.size();
        if (count <= 0)
            return;
        if (count > n)
            count = n;
        s.erase(0, count);
    }
}
