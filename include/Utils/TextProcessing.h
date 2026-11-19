#pragma once

#include "DataStructures/MyString.h"

namespace TextProcessing
{
    enum class TextStatus
    {
        Ok,
        EmptyText,
        InvalidCharacter,
        ColumnOutOfRange
    };

    bool isValidLineText(const mystring &str);

    TextStatus insertAt(const mystring &str, int column,
                        const mystring &text, mystring &out);

    bool replaceAt(const mystring &str, int index, int matchLen,
                   const mystring &replacement, mystring &out);

    int replaceAllInLine(const mystring &str, const mystring &target,
                         const mystring &replacement, mystring &out);

    bool contains(const mystring &s, char c);
    bool containsAnyOf(const mystring &s, const char *set);
    bool isBlank(const mystring &s);
    mystring trim(const mystring &s);
    char toLowerAscii(char c);
    void eraseFront(mystring &s, int count);
}
