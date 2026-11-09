#include "Utils/TextProcessing.h"
#include <utility>

namespace TextProcessing
{
    bool isValidLineText(const std::string &str)
    {
        const int n = static_cast<int>(str.size());
        for (int i = 0; i < n; ++i)
        {
            const char c = str[i];
            if (c == '\n' || c == '\r' || c == '\0')
                return false;
        }
        return true;
    }

    TextStatus insertAt(const std::string &str, int column,
                        const std::string &text, std::string &out)
    {
        const int len = static_cast<int>(str.size()); //str.size() returns unsigned integer type
        const int insLen = static_cast<int>(text.size());

        if (insLen == 0)
            return TextStatus::EmptyText;
        if (!isValidLineText(text))
            return TextStatus::InvalidCharacter;
        if (column < 1 || column > len + 1)
            return TextStatus::ColumnOutOfRange;

        std::string result;
        result.reserve(len + insLen); // one allocation, no regrowth

        int i = 0;
        for (; i < column - 1; ++i)   // part before the insertion point
            result.push_back(str[i]);
        for (int j = 0; j < insLen; ++j) // the new text
            result.push_back(text[j]);
        for (; i < len; ++i)          // the rest of the original
            result.push_back(str[i]);

        out = std::move(result); //move uses memory transfer it does not copy char by char
        return TextStatus::Ok;
    }
        bool contains(const std::string &s, char c)
    {
        const int n = static_cast<int>(s.size());
        for (int i = 0; i < n; ++i)
        {
            if (s[i] == c)
                return true;
        }
        return false;
    }
 //-------helpers definitions----------------------
    bool containsAnyOf(const std::string &s, const char *set)
    {
        const int n = static_cast<int>(s.size());
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

    bool isBlank(const std::string &s)
    {
        const int n = static_cast<int>(s.size());
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

    std::string trim(const std::string &s)
    {
        const int n = static_cast<int>(s.size());
        int first = 0;
        while (first < n && isTrimmable(s[first]))
            ++first;
        int last = n; // one past the last character we keep
        while (last > first && isTrimmable(s[last - 1]))
            --last;

        std::string result;
        result.reserve(last - first);
        for (int i = first; i < last; ++i)
            result.push_back(s[i]);
        return result;
    }

    char toLowerAscii(char c)
    {
        return (c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : c;
    }

    void eraseFront(std::string &s, int count)
    {
        const int n = static_cast<int>(s.size());
        if (count <= 0)
            return;
        if (count > n)
            count = n;
        for (int i = count; i < n; ++i) // shift the survivors to the front
            s[i - count] = s[i];
        s.resize(n - count);            // drop the leftover tail
    }
    
}