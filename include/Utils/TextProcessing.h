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

    // Replaces `matchLen` characters of `str` starting at 0-based `index` with `replacement`.
    // Returns false (and leaves `out` untouched) if the range is invalid. `out` must not alias `str`.
    bool replaceAt(const mystring &str, int index, int matchLen,
                   const mystring &replacement, mystring &out);

    // Replaces every non-overlapping, case-sensitive occurrence of `target` (scanning left to right).
    // Returns the number of replacements; if 0, `out` is left untouched. `out` must not alias `str`.
    // Precondition: `target` is not empty (an empty target returns 0).
    int replaceAllInLine(const mystring &str, const mystring &target,
                         const mystring &replacement, mystring &out);

    bool contains(const mystring &s, char c);
    bool containsAnyOf(const mystring &s, const char *set);
    bool isBlank(const mystring &s);
    mystring trim(const mystring &s);
    char toLowerAscii(char c);
    void eraseFront(mystring &s, int count);
}
