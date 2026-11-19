#include "Core/TextEditor.h"
#include "DataStructures/DoublyLinkedList.h"
#include "DataStructures/MyString.h"
#include "DataStructures/UndoStack.h"
#include <iostream>
#include <stdexcept>

static int failures = 0;
static void check(bool ok, const char *name)
{
    if (!ok)
    {
        std::cout << "FAIL: " << name << "\n";
        ++failures;
    }
}

template <typename Fn>
static bool throwsOutOfRange(Fn fn)
{
    try
    {
        fn();
    }
    catch (const std::out_of_range &)
    {
        return true;
    }
    return false;
}

int main()
{
    // UndoStack unit tests
    {
        UndoStack stack(5);
        check(stack.isEmpty(), "Stack is initially empty");
        check(stack.size() == 0, "Stack initial size is 0");
        check(stack.capacity() == 5, "Stack capacity is 5");

        check(throwsOutOfRange([&] { stack.peek(); }), "peek() on empty stack throws");

        DoublyLinkedList outDoc;
        check(!stack.pop(outDoc), "pop() on empty stack returns false");

        DoublyLinkedList doc1;
        doc1.insertLine(1, "First Line");
        stack.push(doc1);

        check(!stack.isEmpty(), "Stack is not empty after push");
        check(stack.size() == 1, "Stack size is 1 after push");
        check(stack.peek().getLineCount() == 1, "peek() shows 1 line");
        check(stack.peek().getLine(1) == "First Line", "peek() content matches");

        DoublyLinkedList doc2;
        doc2.insertLine(1, "Second Document Line 1");
        doc2.insertLine(2, "Second Document Line 2");
        stack.push(doc2);

        check(stack.size() == 2, "Stack size is 2");
        check(stack.peek().getLineCount() == 2, "peek() shows top document has 2 lines");

        DoublyLinkedList restored;
        check(stack.pop(restored), "pop() returns true");
        check(restored.getLineCount() == 2, "Restored doc has 2 lines");
        check(restored.getLine(1) == "Second Document Line 1", "Restored doc line 1 matches");
        check(restored.getLine(2) == "Second Document Line 2", "Restored doc line 2 matches");
        check(stack.size() == 1, "Stack size is 1 after one pop");

        check(stack.pop(restored), "pop() second time returns true");
        check(restored.getLineCount() == 1, "Restored doc has 1 line");
        check(restored.getLine(1) == "First Line", "Restored doc line 1 matches doc1");
        check(stack.isEmpty(), "Stack is empty after popping all");

        UndoStack boundedStack(3);
        for (int i = 1; i <= 5; ++i)
        {
            DoublyLinkedList d;
            d.insertLine(1, mystring::fromInt(i));
            boundedStack.push(d);
        }
        check(boundedStack.size() == 3, "Bounded stack size stays capped at 3");
        DoublyLinkedList temp;
        check(boundedStack.pop(temp) && temp.getLine(1) == "5", "Top is 5");
        check(boundedStack.pop(temp) && temp.getLine(1) == "4", "Next is 4");
        check(boundedStack.pop(temp) && temp.getLine(1) == "3", "Next is 3");
        check(boundedStack.isEmpty(), "Bounded stack empty after popping the 3 kept items");

        boundedStack.push(doc1);
        boundedStack.push(doc2);
        check(boundedStack.size() == 2, "Pushed 2 items before clear");
        boundedStack.clear();
        check(boundedStack.isEmpty() && boundedStack.size() == 0, "clear() resets stack to empty");
    }

    // TextEditor undo integration tests
    {
        TextEditor editor;
        check(!editor.canUndo(), "Fresh editor has nothing to undo");
        check(editor.undoCount() == 0, "Fresh editor undo count is 0");
        check(!editor.undo(), "undo() returns false when empty");

        // insertText
        editor.insertText(1, 1, "Hello World");
        check(editor.document().getLineCount() == 1, "Line inserted");
        check(editor.undoCount() == 1, "Undo count is 1 after insertText");
        check(editor.undo(), "undo() succeeded");
        check(editor.document().getLineCount() == 0, "Document reverted back to empty");
        check(editor.undoCount() == 0, "Undo count is 0 after undo");

        // insertNewLine
        editor.insertNewLine(1, "Line 1");
        editor.insertNewLine(2, "Line 2");
        check(editor.document().getLineCount() == 2, "2 lines present");
        check(editor.undoCount() == 2, "2 undo actions recorded");
        editor.undo();
        check(editor.document().getLineCount() == 1, "Undid Line 2, 1 line remaining");
        check(editor.document().getLine(1) == "Line 1", "Line 1 is intact");

        // modifyLine
        editor.modifyLine(1, "Modified Line 1");
        check(editor.document().getLine(1) == "Modified Line 1", "Line modified");
        editor.undo();
        check(editor.document().getLine(1) == "Line 1", "Line reverted to original text");

        // deleteCharacterFromLine
        editor.deleteCharacterFromLine(1, 0);
        check(editor.document().getLine(1) == "ine 1", "Character deleted");
        editor.undo();
        check(editor.document().getLine(1) == "Line 1", "Character restored by undo");

        // deleteTextFromLine
        editor.deleteTextFromLine(1, 0, 4);
        check(editor.document().getLine(1) == " 1", "Text range deleted");
        editor.undo();
        check(editor.document().getLine(1) == "Line 1", "Text range restored by undo");

        // deleteLine
        editor.insertNewLine(2, "Line 2");
        editor.deleteLine(1);
        check(editor.document().getLineCount() == 1 && editor.document().getLine(1) == "Line 2", "Line 1 deleted");
        editor.undo();
        check(editor.document().getLineCount() == 2 && editor.document().getLine(1) == "Line 1", "Line 1 restored");

        // deleteRangeOfLines
        editor.insertNewLine(3, "Line 3");
        editor.insertNewLine(4, "Line 4");
        check(editor.document().getLineCount() == 4, "4 lines present");
        editor.deleteRangeOfLines(2, 3);
        check(editor.document().getLineCount() == 2, "Deleted lines 2 and 3");
        editor.undo();
        check(editor.document().getLineCount() == 4, "Lines 2 and 3 restored");
        check(editor.document().getLine(2) == "Line 2", "Line 2 text intact");
        check(editor.document().getLine(3) == "Line 3", "Line 3 text intact");

        // deleteWord
        check(editor.deleteWord("Line"), "deleteWord found and deleted 'Line'");
        editor.undo();
        check(editor.document().getLine(1) == "Line 1", "deleteWord undone");

        // replaceOccurrence
        ReplaceReport report;
        EditResult res = editor.replaceOccurrence("Line", "Paragraph", 1, 1, &report);
        check(res == EditResult::Ok, "replaceOccurrence ok");
        check(editor.document().getLine(1) == "Paragraph 1", "Replaced Line with Paragraph");
        editor.undo();
        check(editor.document().getLine(1) == "Line 1", "replaceOccurrence undone");

        // replaceAll
        editor.replaceAll("Line", "Row");
        check(editor.document().getLine(1) == "Row 1", "replaceAll changed line 1");
        check(editor.document().getLine(2) == "Row 2", "replaceAll changed line 2");
        check(editor.document().getLine(3) == "Row 3", "replaceAll changed line 3");
        check(editor.document().getLine(4) == "Row 4", "replaceAll changed line 4");
        editor.undo();
        check(editor.document().getLine(1) == "Line 1", "replaceAll undone line 1");
        check(editor.document().getLine(2) == "Line 2", "replaceAll undone line 2");
        check(editor.document().getLine(3) == "Line 3", "replaceAll undone line 3");
        check(editor.document().getLine(4) == "Line 4", "replaceAll undone line 4");

        // Invalid operations do not add snapshots
        int preCount = editor.undoCount();
        check(editor.insertText(0, 1, "bad") == EditResult::LineOutOfRange, "Invalid line returns error");
        check(editor.insertText(1, 999, "bad") == EditResult::ColumnOutOfRange, "Invalid col returns error");
        check(editor.replaceOccurrence("NonExistent", "X", 1) == EditResult::NoMatchFound, "No match returns error");
        check(editor.undoCount() == preCount, "Failed operations did not add undo snapshots");

        // Multi-level rollback to empty
        while (editor.canUndo())
        {
            editor.undo();
        }
        check(editor.document().isEmpty(), "Full undo rollback leaves document empty");
        check(editor.undoCount() == 0, "Undo count is 0 after full rollback");
    }

    if (failures == 0)
        std::cout << "All UndoStack and linked undo tests passed successfully!\n";
    return failures == 0 ? 0 : 1;
}
