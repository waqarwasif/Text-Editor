#include "Utils/TextProcessing.h"
#include <iostream>
#include <string>

using namespace TextProcessing;

static int failures = 0;
static void check(bool ok, const char *name)
{
    if (!ok) { std::cout << "FAIL: " << name << "\n"; ++failures; }
}

int main()
{
    std::string out = "untouched";

    check(insertAt("hello", 1, ">> ", out) == TextStatus::Ok && out == ">> hello", "column 1");
    check(insertAt("helo", 3, "l", out) == TextStatus::Ok && out == "hello", "middle");
    check(insertAt("hello", 6, "!", out) == TextStatus::Ok && out == "hello!", "append at len+1");
    check(insertAt("", 1, "abc", out) == TextStatus::Ok && out == "abc", "empty line");

    out = "untouched"; // every failure below must leave out unchanged
    check(insertAt("abc", 2, "", out) == TextStatus::EmptyText && out == "untouched", "empty text");
    check(insertAt("abc", 0, "x", out) == TextStatus::ColumnOutOfRange && out == "untouched", "column 0");
    check(insertAt("abc", 5, "x", out) == TextStatus::ColumnOutOfRange && out == "untouched", "column > len+1");
    check(insertAt("abc", 2, "a\nb", out) == TextStatus::InvalidCharacter && out == "untouched", "newline");
    check(insertAt("abc", 2, "a\rb", out) == TextStatus::InvalidCharacter && out == "untouched", "carriage return");
    check(insertAt("abc", 2, std::string("a\0b", 3), out) == TextStatus::InvalidCharacter && out == "untouched", "NUL");

    std::string s = "hello"; // out aliasing str
    check(insertAt(s, 3, "XY", s) == TextStatus::Ok && s == "heXYllo", "out aliases str");

        // ---- helpers used by FileOperations ----
    check(isBlank("") && isBlank("  \t") && !isBlank(" a "), "isBlank");
    check(contains("abc", 'b') && !contains("abc", 'z') && !contains("", 'a'), "contains");
    check(contains(std::string("a\0b", 3), '\0'), "contains finds NUL");
    check(containsAnyOf("file?.txt", "<>:\"|?*") && !containsAnyOf("file.txt", "<>:\"|?*"), "containsAnyOf");
    check(trim("  hi \t\r\n") == "hi" && trim("   ") == "" && trim("") == "" && trim("a b") == "a b", "trim");
    check(toLowerAscii('Y') == 'y' && toLowerAscii('y') == 'y' && toLowerAscii('1') == '1', "toLowerAscii");

    std::string bom = "\xEF\xBB\xBFhello";
    eraseFront(bom, 3);
    check(bom == "hello", "eraseFront removes 3");
    std::string e = "abc";
    eraseFront(e, 0);
    check(e == "abc", "eraseFront 0 changes nothing");
    eraseFront(e, 10);
    check(e == "", "eraseFront more than length empties");
    if (failures == 0) std::cout << "All TextProcessing tests passed.\n";
    return failures == 0 ? 0 : 1;
}