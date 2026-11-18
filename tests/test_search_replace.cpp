#include "DataStructures/DoublyLinkedList.h"
#include "DataStructures/Array.h"
#include "DataStructures/MyString.h"
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
    try { fn(); }
    catch (const std::out_of_range &) { return true; }
    return false;
}

int main()
{
    // ---- 1. ResultArray Tests ----
    {
        ResultArray arr;
        check(arr.size() == 0, "ResultArray initial size is 0");

        for (int i = 0; i < 25; ++i)
        {
            arr.add({i + 1, i * 2});
        }
        check(arr.size() == 25, "ResultArray resized to hold 25 elements");
        check(arr.get(0).lineNumber == 1 && arr.get(0).startIndex == 0, "ResultArray get(0)");
        check(arr.get(24).lineNumber == 25 && arr.get(24).startIndex == 48, "ResultArray get(24)");
        check(throwsOutOfRange([&] { arr.get(-1); }), "ResultArray get(-1) throws");
        check(throwsOutOfRange([&] { arr.get(25); }), "ResultArray get(25) throws");
    }

    // ---- 2. searchDocument Tests ----
    {
        DoublyLinkedList doc;
        doc.insertLine(1, "The quick brown fox");
        doc.insertLine(2, "jumps over the lazy dog");
        doc.insertLine(3, "fox and dog are friends");

        // Target found once
        ResultArray r1 = doc.searchDocument("quick");
        check(r1.size() == 1, "search 'quick' count");
        check(r1.get(0).lineNumber == 1 && r1.get(0).startIndex == 4, "search 'quick' position");

        // Target found across lines
        ResultArray r2 = doc.searchDocument("fox");
        check(r2.size() == 2, "search 'fox' count");
        check(r2.get(0).lineNumber == 1 && r2.get(0).startIndex == 16, "search 'fox' line 1 pos");
        check(r2.get(1).lineNumber == 3 && r2.get(1).startIndex == 0, "search 'fox' line 3 pos");

        // Target not found
        ResultArray r3 = doc.searchDocument("cat");
        check(r3.size() == 0, "search 'cat' not found");

        // Empty search target
        ResultArray r4 = doc.searchDocument("");
        check(r4.size() == 0, "search empty string returns 0 without crashing");

        // Target longer than any line
        ResultArray r5 = doc.searchDocument("This target is way longer than any line in this document");
        check(r5.size() == 0, "search overly long target returns 0");
    }

    // ---- 3. deleteWord Tests ----
    {
        DoublyLinkedList doc;
        doc.insertLine(1, "apple banana cherry");
        doc.insertLine(2, "date banana elderberry");

        check(doc.deleteWord("banana"), "deleteWord found and deleted");
        check(doc.getLine(1) == "apple  cherry", "deleteWord removed first occurrence on line 1");
        check(doc.getLine(2) == "date banana elderberry", "second occurrence remains on line 2");

        check(!doc.deleteWord("mango"), "deleteWord non-existent returns false");
        check(!doc.deleteWord(""), "deleteWord empty returns false");
    }

    // ---- 4. deleteCharacterFromLine & deleteTextFromLine Tests ----
    {
        DoublyLinkedList doc;
        doc.insertLine(1, "ABCDEF");

        doc.deleteCharacterFromLine(1, 0); // Delete 'A'
        check(doc.getLine(1) == "BCDEF", "deleteCharacterFromLine index 0");

        doc.deleteTextFromLine(1, 1, 2); // Delete 'CD' from 'BCDEF'
        check(doc.getLine(1) == "BEF", "deleteTextFromLine 2 chars");

        doc.deleteTextFromLine(1, 1, 100); // Delete 'EF'
        check(doc.getLine(1) == "B", "deleteTextFromLine clamps to end of line");

        check(throwsOutOfRange([&] { doc.deleteCharacterFromLine(0, 0); }), "bad line throws");
        check(throwsOutOfRange([&] { doc.deleteCharacterFromLine(1, -1); }), "bad index throws");
    }

    // ---- 5. deleteRangeOfLines Tests ----
    {
        DoublyLinkedList doc;
        for (int i = 1; i <= 5; ++i)
        {
            doc.insertLine(i, "Line " + mystring::fromInt(i));
        }

        doc.deleteRangeOfLines(2, 4);
        check(doc.getLineCount() == 2, "deleteRangeOfLines count is 2");
        check(doc.getLine(1) == "Line 1", "remaining line 1");
        check(doc.getLine(2) == "Line 5", "remaining line 2 is Line 5");

        doc.deleteRangeOfLines(1, 2);
        check(doc.getLineCount() == 0 && doc.isEmpty(), "delete entire range empties document");
    }

    if (failures == 0)
    {
        std::cout << "All search, replace, and deletion tests passed.\n";
    }
    return failures == 0 ? 0 : 1;
}
