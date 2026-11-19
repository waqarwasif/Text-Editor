#include "Core/TextEditor.h"
#include "DataStructures/MyString.h"
#include <iostream>

static int failures = 0;
static void check(bool ok, const char *name)
{
    if (!ok)
    {
        std::cout << "FAIL: " << name << "\n";
        ++failures;
    }
}

int main()
{
    TextEditor ed;

    // Test 1: Insert into empty document
    check(ed.insertText(1, 1, "hello") == EditResult::Ok, "type into empty document");
    check(ed.document().getLineCount() == 1 && ed.document().getLine(1) == "hello", "first line created");
    check(ed.files().isModified(), "marked modified");

    // Test 2: Insert at start, middle, and end of line
    check(ed.insertText(1, 3, "XX") == EditResult::Ok && ed.document().getLine(1) == "heXXllo", "middle");
    check(ed.insertText(1, 1, ">") == EditResult::Ok && ed.document().getLine(1) == ">heXXllo", "start");
    check(ed.insertText(1, 9, "!") == EditResult::Ok && ed.document().getLine(1) == ">heXXllo!", "append at len+1");

    // Test 3: Insert text creating new line
    check(ed.insertText(2, 1, "second") == EditResult::Ok && ed.document().getLineCount() == 2, "new last line");
    check(ed.insertText(3, 2, "x") == EditResult::ColumnOutOfRange, "new line needs column 1");

    // Test 4: Bounds and error checking
    check(ed.insertText(0, 1, "x") == EditResult::LineOutOfRange, "line 0");
    check(ed.insertText(4, 1, "x") == EditResult::LineOutOfRange, "line count+2");
    check(ed.insertText(1, 0, "x") == EditResult::ColumnOutOfRange, "column 0");
    check(ed.insertText(1, 99, "x") == EditResult::ColumnOutOfRange, "column too big");
    check(ed.insertText(1, 1, "") == EditResult::NothingToInsert, "empty text");
    check(ed.insertText(1, 1, "a\nb") == EditResult::InvalidText, "newline in text");
    check(ed.document().getLineCount() == 2 && ed.document().getLine(1) == ">heXXllo!", "errors changed nothing");

    // Test 5: Failed edits do not mark modified
    TextEditor fresh;
    check(fresh.insertText(1, 2, "x") == EditResult::ColumnOutOfRange, "fresh: bad column");
    check(fresh.insertText(1, 1, "") == EditResult::NothingToInsert, "fresh: empty text");
    check(!fresh.files().isModified(), "failed edits do not mark modified");

    // Test 6: insertNewLine verification
    check(ed.insertNewLine(2) == EditResult::Ok && ed.document().getLine(2) == "" && ed.document().getLineCount() == 3, "blank line in the middle");
    check(ed.document().getLine(3) == "second", "later lines shift down");
    check(ed.insertNewLine(4, "last") == EditResult::Ok && ed.document().getLine(4) == "last", "new line at the end");
    check(ed.insertNewLine(0) == EditResult::LineOutOfRange, "newline: line 0");
    check(ed.insertNewLine(6) == EditResult::LineOutOfRange, "newline: line count+2");
    check(ed.insertNewLine(1, "a\rb") == EditResult::InvalidText, "newline: bad text");

    ed.document().displayDocumentInfo();

    if (failures == 0)
        std::cout << "All TextEditor checks passed.\n";
    return failures == 0 ? 0 : 1;
}
