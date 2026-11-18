#pragma once

#include "DataStructures/MyString.h"

namespace TextProcessing
{
    enum class TextStatus
    {
        Ok,
        EmptyText,        // nothing to insert
        InvalidCharacter, // contains '\n', '\r' or '\0'
        ColumnOutOfRange  // column not in 1 .. length+1
    };

    bool isValidLineText(const mystring &str);

    TextStatus insertAt(const mystring &str, int column,
                        const mystring &text, mystring &out);

    bool contains(const mystring &s, char c);
    bool containsAnyOf(const mystring &s, const char *set);
    bool isBlank(const mystring &s);
    mystring trim(const mystring &s);
    char toLowerAscii(char c);
    void eraseFront(mystring &s, int count);
}
