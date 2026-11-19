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

    static void appendRange(mystring &dst, const char *src, int from, int to)
    {
        for (int i = from; i < to; ++i)
            dst.push_back(src[i]);
    }

    bool replaceAt(const mystring &str, int index, int matchLen,
                   const mystring &replacement, mystring &out)
    {
        const int len = str.size();
        if (index < 0 || matchLen <= 0 || index > len - matchLen)
            return false;

        const char *s = str.c_str();
        mystring result;
        appendRange(result, s, 0, index);
        appendRange(result, replacement.c_str(), 0, replacement.size());
        appendRange(result, s, index + matchLen, len);

        out = std::move(result);
        return true;
    }

    int replaceAllInLine(const mystring &str, const mystring &target,
                         const mystring &replacement, mystring &out)
    {
        const int len = str.size();
        const int tLen = target.size();
        if (tLen == 0 || tLen > len)
            return 0;

        const char *s = str.c_str();
        const char *t = target.c_str();
        const char *r = replacement.c_str();
        const int rLen = replacement.size();

        mystring result;
        int count = 0;
        int pos = 0;
        int copied = 0;

        while (pos <= len - tLen)
        {
            int j = 0;
            while (j < tLen && s[pos + j] == t[j])
                ++j;

            if (j == tLen)
            {
                appendRange(result, s, copied, pos);
                appendRange(result, r, 0, rLen);
                pos += tLen;
                copied = pos;
                ++count;
            }
            else
            {
                ++pos;
            }
        }

        if (count == 0)
            return 0;

        appendRange(result, s, copied, len);
        out = std::move(result);
        return count;
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
