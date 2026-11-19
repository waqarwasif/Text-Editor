#include "DataStructures/DoublyLinkedList.h"
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
    DoublyLinkedList list;

    // Test 1: Insert lines at tail
    for (int i = 1; i <= 7; ++i)
        list.insertLine(i, mystring::fromInt(i));
    for (int i = 1; i <= 7; ++i)
        check(list.getLine(i) == mystring::fromInt(i), "getLine returns the right node");

    // Test 2: Insert near head and tail
    list.insertLine(2, "A");
    list.insertLine(7, "B");
    check(list.getLineCount() == 9, "line count after inserts");
    check(list.getLine(1) == "1" && list.getLine(2) == "A" && list.getLine(7) == "B" &&
          list.getLine(8) == "6" && list.getLine(9) == "7", "order after inserts");

    // Test 3: modifyLine updates character count
    list.modifyLine(7, "XYZ");
    check(list.getLine(7) == "XYZ", "modifyLine");
    list.displayDocumentInfo();

    // Test 4: Bounds checking
    check(throwsOutOfRange([&] { list.getLine(0); }), "getLine(0) throws");
    check(throwsOutOfRange([&] { list.getLine(10); }), "getLine(count+1) throws");
    check(throwsOutOfRange([&] { list.modifyLine(0, "x"); }), "modifyLine(0) throws");
    check(throwsOutOfRange([&] { list.insertLine(0, "x"); }), "insertLine(0) throws");
    check(throwsOutOfRange([&] { list.insertLine(11, "x"); }), "insertLine(count+2) throws");
    check(list.getLineCount() == 9, "failed calls changed nothing");

    // Test 5: clear()
    list.clear();
    check(list.isEmpty() && list.getLineCount() == 0, "clear empties the list");
    list.displayDocumentInfo();

    if (failures == 0)
        std::cout << "All linked list checks passed.\n";
    return failures == 0 ? 0 : 1;
}
