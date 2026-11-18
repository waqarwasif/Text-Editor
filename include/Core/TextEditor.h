#pragma once

#include "Core/FileOperations.h"
#include "DataStructures/DoublyLinkedList.h"
#include "DataStructures/MyString.h"

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
    EditResult insertText(int line, int column, const mystring &text);
    EditResult insertNewLine(int line, const mystring &text = mystring(""));

    DoublyLinkedList &document() { return doc_; }
    const DoublyLinkedList &document() const { return doc_; }
    FileOperations &files() { return files_; }
    const FileOperations &files() const { return files_; }

    static const char *describe(EditResult result);

private:
    DoublyLinkedList doc_;
    FileOperations files_;
};
