#include "Core/TextEditor.h"
#include "DataStructures/MyString.h"
#include "Utils/TextProcessing.h"
#include <initializer_list>
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

// Loads lines straight into the list. This does NOT mark the document as modified,
// so tests can prove that failed / no-match replaces leave the flag untouched.
static void fill(TextEditor &ed, std::initializer_list<const char *> lines)
{
    for (const char *text : lines)
        ed.document().insertLine(ed.document().getLineCount() + 1, text);
}

static void testHelpers()
{
    using namespace TextProcessing;
    mystring out("keep");

    // replaceAt: valid ranges
    check(replaceAt("hello", 1, 3, "", out) && out == "ho", "replaceAt: delete middle");
    check(replaceAt("hello", 0, 5, "X", out) && out == "X", "replaceAt: whole string");
    check(replaceAt("hello", 4, 1, "!!", out) && out == "hell!!", "replaceAt: last character");

    // replaceAt: invalid ranges must leave `out` untouched
    out = "keep";
    check(!replaceAt("hello", 3, 5, "x", out) && out == "keep", "replaceAt: range past the end");
    check(!replaceAt("hello", -1, 1, "x", out) && out == "keep", "replaceAt: negative index");
    check(!replaceAt("hello", 1, 0, "x", out) && out == "keep", "replaceAt: zero length");
    check(!replaceAt("", 0, 1, "x", out) && out == "keep", "replaceAt: empty string");

    // replaceAllInLine: valid
    check(replaceAllInLine("aaa", "aa", "b", out) == 1 && out == "ba", "replaceAll: no overlap, left to right");
    check(replaceAllInLine("aaa", "a", "aa", out) == 3 && out == "aaaaaa", "replaceAll: replacement contains target");
    check(replaceAllInLine("abcabc", "abc", "", out) == 2 && out == "", "replaceAll: empty replacement deletes");
    check(replaceAllInLine("a b a", "a", "XY", out) == 2 && out == "XY b XY", "replaceAll: replacement longer");
    check(replaceAllInLine("xaax", "a", "-", out) == 2 && out == "x--x", "replaceAll: text before and after");
    check(replaceAllInLine("Aa", "a", "x", out) == 1 && out == "Ax", "replaceAll: case sensitive");

    // replaceAllInLine: nothing to do must leave `out` untouched
    out = "keep";
    check(replaceAllInLine("abc", "x", "y", out) == 0 && out == "keep", "replaceAll: no match");
    check(replaceAllInLine("ab", "abc", "x", out) == 0 && out == "keep", "replaceAll: target longer than line");
    check(replaceAllInLine("", "a", "b", out) == 0 && out == "keep", "replaceAll: empty line");
    check(replaceAllInLine("x", "", "y", out) == 0 && out == "keep", "replaceAll: empty target");
}

static void testReplaceAll()
{
    {
        TextEditor ed;
        fill(ed, {"aaa", "a b a", "none", ""});
        ReplaceReport rep;
        check(!ed.files().isModified(), "replaceAll: starts unmodified");
        check(ed.replaceAll("a", "aa", 0, &rep) == EditResult::Ok, "replaceAll: whole document ok");
        check(rep.count == 5, "replaceAll: count is 5");
        check(ed.document().getLine(1) == "aaaaaa" && ed.document().getLine(2) == "aa b aa" &&
              ed.document().getLine(3) == "none" && ed.document().getLine(4) == "",
              "replaceAll: every line correct, untouched lines kept");
        check(ed.document().getLineCount() == 4, "replaceAll: line count unchanged");
        check(ed.files().isModified(), "replaceAll: marks modified");
    }
    {
        TextEditor ed;
        fill(ed, {"foo foo", "foo"});
        ReplaceReport rep;
        check(ed.replaceAll("foo", "bar", 2, &rep) == EditResult::Ok && rep.count == 1, "replaceAll: single line ok");
        check(ed.document().getLine(1) == "foo foo" && ed.document().getLine(2) == "bar", "replaceAll: other lines untouched");
    }
    {
        TextEditor ed;
        fill(ed, {"a-b-c"});
        check(ed.replaceAll("-", "") == EditResult::Ok && ed.document().getLine(1) == "abc", "replaceAll: empty replacement");
    }
    {
        TextEditor ed;
        fill(ed, {"aaa"});
        check(ed.replaceAll("aa", "b") == EditResult::Ok && ed.document().getLine(1) == "ba", "replaceAll: overlap rule");
    }
    {
        TextEditor ed;
        fill(ed, {"Hello hello"});
        check(ed.replaceAll("hello", "X") == EditResult::Ok && ed.document().getLine(1) == "Hello X", "replaceAll: case sensitive");
    }
    {
        // every error leaves the document unchanged and unmodified
        TextEditor ed;
        fill(ed, {"abc"});
        check(ed.replaceAll("", "x") == EditResult::EmptyTarget, "replaceAll: empty target");
        check(ed.replaceAll("a\nb", "x") == EditResult::InvalidText, "replaceAll: newline in target");
        check(ed.replaceAll("a", "x\ry") == EditResult::InvalidText, "replaceAll: CR in replacement");
        check(ed.replaceAll("a", "x", -1) == EditResult::LineOutOfRange, "replaceAll: negative line");
        check(ed.replaceAll("a", "x", 2) == EditResult::LineOutOfRange, "replaceAll: line past the end");
        check(ed.replaceAll("a", "a", 5) == EditResult::LineOutOfRange, "replaceAll: bad line beats no-change");
        check(ed.replaceAll("a", "a") == EditResult::NoChange, "replaceAll: identical text");
        check(ed.replaceAll("zzz", "x") == EditResult::NoMatchFound, "replaceAll: no match");
        check(ed.document().getLine(1) == "abc" && ed.document().getLineCount() == 1, "replaceAll: errors changed nothing");
        check(!ed.files().isModified(), "replaceAll: errors do not mark modified");
    }
    {
        TextEditor ed;
        fill(ed, {"foo", "bar"});
        check(ed.replaceAll("foo", "x", 2) == EditResult::NoMatchFound, "replaceAll: match exists only on another line");
        check(ed.document().getLine(1) == "foo" && !ed.files().isModified(), "replaceAll: other line untouched, unmodified");
    }
    {
        TextEditor ed; // empty document
        check(ed.replaceAll("a", "b") == EditResult::NoMatchFound, "replaceAll: empty document");
        check(ed.replaceAll("a", "b", 1) == EditResult::LineOutOfRange, "replaceAll: empty document, line 1");
        check(!ed.files().isModified(), "replaceAll: empty document stays unmodified");
    }
}

static void testReplaceOccurrence()
{
    {
        // overlapping matches are counted exactly like searchDocument
        TextEditor ed;
        fill(ed, {"aaa", "a b a"});
        ReplaceReport rep;
        check(!ed.files().isModified(), "replaceOccurrence: starts unmodified");
        check(ed.replaceOccurrence("aa", "X", 2, 0, &rep) == EditResult::Ok, "occurrence: 2nd overlapping match ok");
        check(ed.document().getLine(1) == "aX" && ed.document().getLine(2) == "a b a", "occurrence: 2nd overlapping match result");
        check(rep.line == 1 && rep.column == 2 && rep.count == 1, "occurrence: report for 2nd overlapping match");
        check(ed.files().isModified(), "occurrence: marks modified");
    }
    {
        // occurrences continue across lines
        TextEditor ed;
        fill(ed, {"aaa", "a b a"});
        ReplaceReport rep;
        check(ed.replaceOccurrence("a", "X", 4, 0, &rep) == EditResult::Ok, "occurrence: 4th match ok");
        check(ed.document().getLine(1) == "aaa" && ed.document().getLine(2) == "X b a", "occurrence: 4th match is first match of line 2");
        check(rep.line == 2 && rep.column == 1, "occurrence: 4th match position");
    }
    {
        TextEditor ed;
        fill(ed, {"aaa", "a b a"});
        ReplaceReport rep;
        check(ed.replaceOccurrence("a", "X", 5, 0, &rep) == EditResult::Ok && ed.document().getLine(2) == "a b X", "occurrence: last match");
        check(rep.line == 2 && rep.column == 5, "occurrence: last match position");
    }
    {
        TextEditor ed;
        fill(ed, {"aaa", "a b a"});
        check(ed.replaceOccurrence("a", "X", 6) == EditResult::NoMatchFound, "occurrence: more than exists");
        check(ed.document().getLine(1) == "aaa" && ed.document().getLine(2) == "a b a" && !ed.files().isModified(),
              "occurrence: too large changes nothing");
    }
    {
        // scoped to one line: occurrences are counted inside that line only
        TextEditor ed;
        fill(ed, {"aaa", "a b a"});
        ReplaceReport rep;
        check(ed.replaceOccurrence("a", "X", 2, 2, &rep) == EditResult::Ok, "occurrence: scoped ok");
        check(ed.document().getLine(1) == "aaa" && ed.document().getLine(2) == "a b X", "occurrence: scoped result");
        check(rep.line == 2 && rep.column == 5, "occurrence: scoped position");
    }
    {
        TextEditor ed;
        fill(ed, {"aaa", "a b a"});
        check(ed.replaceOccurrence("a", "X", 3, 2) == EditResult::NoMatchFound, "occurrence: scoped, too large");
        check(!ed.files().isModified(), "occurrence: scoped too large stays unmodified");
    }
    {
        TextEditor ed;
        fill(ed, {"foo", "bar"});
        check(ed.replaceOccurrence("foo", "x", 1, 2) == EditResult::NoMatchFound, "occurrence: match only on another line");
        check(ed.document().getLine(1) == "foo" && !ed.files().isModified(), "occurrence: other line untouched");
    }
    {
        TextEditor ed;
        fill(ed, {"abc"});
        check(ed.replaceOccurrence("a", "x", 0) == EditResult::InvalidOccurrence, "occurrence: zero");
        check(ed.replaceOccurrence("a", "x", -3) == EditResult::InvalidOccurrence, "occurrence: negative");
        check(ed.replaceOccurrence("", "x", 1) == EditResult::EmptyTarget, "occurrence: empty target");
        check(ed.replaceOccurrence("a\nb", "x", 1) == EditResult::InvalidText, "occurrence: newline in target");
        check(ed.replaceOccurrence("a", "x\ry", 1) == EditResult::InvalidText, "occurrence: CR in replacement");
        check(ed.replaceOccurrence("a", "x", 1, -1) == EditResult::LineOutOfRange, "occurrence: negative line");
        check(ed.replaceOccurrence("a", "x", 1, 2) == EditResult::LineOutOfRange, "occurrence: line past the end");
        check(ed.replaceOccurrence("a", "a", 1, 5) == EditResult::LineOutOfRange, "occurrence: bad line beats no-change");
        check(ed.replaceOccurrence("a", "a", 1) == EditResult::NoChange, "occurrence: identical text");
        check(ed.replaceOccurrence("zzz", "x", 1) == EditResult::NoMatchFound, "occurrence: no match");
        check(ed.document().getLine(1) == "abc" && !ed.files().isModified(), "occurrence: errors changed nothing");
    }
    {
        TextEditor ed;
        fill(ed, {"hello world"});
        ReplaceReport rep;
        check(ed.replaceOccurrence("o", "", 2, 0, &rep) == EditResult::Ok && ed.document().getLine(1) == "hello wrld",
              "occurrence: empty replacement deletes");
        check(rep.column == 8, "occurrence: deletion position");
    }
    {
        TextEditor ed;
        fill(ed, {"a"});
        check(ed.replaceOccurrence("a", "aa", 1) == EditResult::Ok && ed.document().getLine(1) == "aa",
              "occurrence: replacement contains target");
    }
    {
        TextEditor ed;
        fill(ed, {"cat"});
        check(ed.replaceOccurrence("c", "b", 1) == EditResult::Ok && ed.document().getLine(1) == "bat",
              "occurrence: works without a report");
    }
    {
        TextEditor ed; // empty document
        check(ed.replaceOccurrence("a", "b", 1) == EditResult::NoMatchFound, "occurrence: empty document");
        check(!ed.files().isModified(), "occurrence: empty document stays unmodified");
    }
}

int main()
{
    testHelpers();
    testReplaceAll();
    testReplaceOccurrence();

    if (failures == 0)
        std::cout << "All replace checks passed.\n";
    return failures == 0 ? 0 : 1;
}