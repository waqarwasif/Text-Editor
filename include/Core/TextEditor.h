#pragma once

#include "Core/FileOperations.h"
#include "DataStructures/DoublyLinkedList.h"
#include "DataStructures/MyString.h"
#include "DataStructures/UndoStack.h"

enum class EditResult
{
    Ok,
    NothingToInsert,
    InvalidText,
    LineOutOfRange,
    ColumnOutOfRange,
    EmptyTarget,
    InvalidOccurrence,
    NoMatchFound,
    NoChange
};

struct ReplaceReport
{
    int count = 0;
    int line = 0;
    int column = 0;
};

class TextEditor
{
public:
    EditResult insertText(int line, int column, const mystring &text);
    EditResult insertNewLine(int line, const mystring &text = mystring(""));

    EditResult replaceOccurrence(const mystring &target, const mystring &replacement,
                                 int occurrence, int line = 0,
                                 ReplaceReport *report = nullptr);

    EditResult replaceAll(const mystring &target, const mystring &replacement,
                          int line = 0, ReplaceReport *report = nullptr);

    void modifyLine(int lineNumber, const mystring &newText);
    void deleteLine(int lineNumber);
    void deleteRangeOfLines(int startLine, int endLine);
    void deleteCharacterFromLine(int lineNumber, int charIndex);
    void deleteTextFromLine(int lineNumber, int startIndex, int count);
    bool deleteWord(const mystring &targetWord);

    bool undo();
    bool canUndo() const;
    void saveSnapshot();
    void clearUndo();
    int undoCount() const;

    DoublyLinkedList &document() { return doc_; }
    const DoublyLinkedList &document() const { return doc_; }
    FileOperations &files() { return files_; }
    const FileOperations &files() const { return files_; }
    UndoStack &undoStack() { return undoStack_; }
    const UndoStack &undoStack() const { return undoStack_; }

    static const char *describe(EditResult result);

private:
    DoublyLinkedList doc_;
    FileOperations files_;
    UndoStack undoStack_;
};
