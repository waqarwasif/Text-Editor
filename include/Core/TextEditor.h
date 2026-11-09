#pragma once
#include <string>

#include "Core/FileOperations.h"
#include "DataStructures/DoublyLinkedList.h"

// Outcome of an edit. The core functions report; they never print.
enum class EditResult
{
    Ok,
    NothingToInsert,  // text is empty: nothing changed, document not marked modified
    InvalidText,      // text contains '\n', '\r' or '\0'
    LineOutOfRange,   // line not in 1 .. lineCount+1
    ColumnOutOfRange  // column not in 1 .. length+1 (must be 1 on a new line)
};

class TextEditor
{
public:
    // ---- Feature 3: insertion (all positions are 1-based) ----
    // Inserts text before `column` of `line`. line == lineCount+1 (with column 1)
    // starts a new last line, so an empty document can be typed into.
    EditResult insertText(int line, int column, const std::string &text);

    // Inserts a whole new line at `line` (1 .. lineCount+1). Blank by default.
    EditResult insertNewLine(int line, const std::string &text = "");

    // ---- Access (file operations and display use these; every EDIT must go
    // through the functions above so markModified() is never forgotten) ----
    DoublyLinkedList &document() { return doc_; }
    const DoublyLinkedList &document() const { return doc_; }
    FileOperations &files() { return files_; }
    const FileOperations &files() const { return files_; }

    static std::string describe(EditResult result);

private:
    DoublyLinkedList doc_;
    FileOperations files_;
};