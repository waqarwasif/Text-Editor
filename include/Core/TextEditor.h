#pragma once

#include "Core/FileOperations.h"
#include "DataStructures/DoublyLinkedList.h"
#include "DataStructures/MyString.h"

enum class EditResult
{
    Ok,
    NothingToInsert,   // text is empty: nothing changed, document not marked modified
    InvalidText,       // text contains '\n', '\r' or '\0'
    LineOutOfRange,    // line not in 1 .. lineCount+1
    ColumnOutOfRange,  // column not in 1 .. length+1 (must be 1 on a new line)
    EmptyTarget,       // search text is empty
    InvalidOccurrence, // occurrence number is less than 1
    NoMatchFound,      // text not found (or fewer occurrences than requested)
    NoChange           // target and replacement are identical: nothing to do
};

// Optional details about a replace operation (pass nullptr if not needed)
struct ReplaceReport
{
    int count = 0;  // number of replacements made
    int line = 0;   // replaceOccurrence only: 1-based line (0 for replaceAll)
    int column = 0; // replaceOccurrence only: 1-based column (0 for replaceAll)
};

class TextEditor
{
public:
    EditResult insertText(int line, int column, const mystring &text);
    EditResult insertNewLine(int line, const mystring &text = mystring(""));

    // Case-sensitive. line = 0 searches the whole document, line >= 1 only that line.
    // `occurrence` (1-based) counts matches in document order, overlaps included (same as search).
    EditResult replaceOccurrence(const mystring &target, const mystring &replacement,
                                 int occurrence, int line = 0,
                                 ReplaceReport *report = nullptr);

    // Replaces every non-overlapping occurrence (left to right) in the document or in one line.
    EditResult replaceAll(const mystring &target, const mystring &replacement,
                          int line = 0, ReplaceReport *report = nullptr);

    DoublyLinkedList &document() { return doc_; }
    const DoublyLinkedList &document() const { return doc_; }
    FileOperations &files() { return files_; }
    const FileOperations &files() const { return files_; }

    static const char *describe(EditResult result);

private:
    DoublyLinkedList doc_;
    FileOperations files_;
};
