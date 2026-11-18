#pragma once
#include <string>

namespace TextProcessing
{
    enum class TextStatus
    {
        Ok,
        EmptyText,        // nothing to insert
        InvalidCharacter, // contains '\n', '\r' or '\0'
        ColumnOutOfRange  // column not in 1 .. length+1
    };

    // True if str contains no '\n', '\r' or '\0'.
    bool isValidLineText(const std::string &str);

    // Inserts `text` before 1-based `column` of `str`, writing the result to `out`.
    // `out` is modified ONLY when the status is Ok.
    TextStatus insertAt(const std::string &str, int column,
const std::string &text, std::string &out);

    // ---- general helpers (also used by Core/FileOperations) ----
    bool contains(const std::string &s, char c);
    bool containsAnyOf(const std::string &s, const char *set); // set: NUL-terminated list
    bool isBlank(const std::string &s);                         // empty or only ' ' / '\t'
    std::string trim(const std::string &s);                     // strips ' ' '\t' '\r' '\n' at both ends
    char toLowerAscii(char c);
    void eraseFront(std::string &s, int count);                 // removes the first `count` chars
}