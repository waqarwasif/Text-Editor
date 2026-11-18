#include "Core/FileOperations.h"
#include "DataStructures/DoublyLinkedList.h"
#include "DataStructures/MyString.h"

#include <cstdio>
#include <fstream>
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

static void writeRaw(const char *path, const char *bytes, int count)
{
    std::ofstream out(path, std::ios::binary);
    out.write(bytes, count);
}

static mystring readRaw(const char *path)
{
    std::ifstream in(path, std::ios::binary);
    mystring result;
    char ch;
    while (in.get(ch))
    {
        result.push_back(ch);
    }
    return result;
}

static bool startsWith(const mystring &s, const char *prefix)
{
    int pLen = 0;
    while (prefix[pLen] != '\0') pLen++;
    if (s.length() < pLen)
        return false;
    for (int i = 0; i < pLen; ++i)
    {
        if (s[i] != prefix[i])
            return false;
    }
    return true;
}

int main()
{
    const char *fa = "tmp_fileio_a.txt";
    const char *fb = "tmp_fileio_b.txt";
    std::remove(fa);
    std::remove(fb);

    // ---- basic contract ----
    {
        FileOperations ops;
        DoublyLinkedList doc;
        doc.insertLine(1, "keep me");

        check(ops.open("tmp_fileio_missing.txt", doc) == FileResult::NotFound, "open: missing file");
        check(ops.open("   ", doc) == FileResult::InvalidName, "open: blank path");
        check(ops.open(".", doc) == FileResult::NotAFile, "open: a folder");
        check(doc.getLineCount() == 1 && doc.getLine(1) == "keep me", "failed open leaves the document intact");
        check(ops.save(doc) == FileResult::NoFileName, "save: no path yet");

        check(ops.createNew(doc) == FileResult::Ok, "createNew ok");
        check(doc.getLineCount() == 0 && !ops.hasPath() && !ops.isModified(), "createNew resets everything");
        check(ops.displayName() == "Untitled", "displayName of a new document");
    }

    // ---- round trip ----
    {
        FileOperations ops;
        DoublyLinkedList doc;
        doc.insertLine(1, "one");
        doc.insertLine(2, "two");
        doc.insertLine(3, "three");

        check(ops.saveAs(fa, doc) == FileResult::Ok, "saveAs ok");
        check(ops.hasPath() && ops.displayName() == fa, "saveAs sets the path");
        check(startsWith(readRaw(fa), "one\ntwo\nthree"), "file content on disk");

        FileOperations ops2;
        DoublyLinkedList loaded;
        check(ops2.open(fa, loaded) == FileResult::Ok, "open ok");
        check(loaded.getLineCount() == 3 && loaded.getLine(1) == "one" &&
                  loaded.getLine(2) == "two" && loaded.getLine(3) == "three",
              "round trip keeps every line");
    }

    // ---- Step 4 regression targets ----
    {   // BOM stripped
        const char bomHello[] = "\xEF\xBB\xBFhello\n";
        writeRaw(fa, bomHello, 9);
        FileOperations ops;
        DoublyLinkedList doc;
        check(ops.open(fa, doc) == FileResult::Ok && doc.getLine(1) == "hello", "BOM stripped");
    }
    {   // NUL byte means binary
        const char nulBytes[] = {'a', 'b', '\0', 'c', 'd', '\n'};
        writeRaw(fa, nulBytes, 6);
        FileOperations ops;
        DoublyLinkedList doc;
        check(ops.open(fa, doc) == FileResult::BinaryFile, "NUL byte -> BinaryFile");
    }
    {   // CRLF: save keeps CRLF
        const char crlfBytes[] = "a\r\nb\r\n";
        writeRaw(fa, crlfBytes, 6);
        FileOperations ops;
        DoublyLinkedList doc;
        check(ops.open(fa, doc) == FileResult::Ok && doc.getLineCount() == 2 &&
                  doc.getLine(1) == "a" && doc.getLine(2) == "b",
              "CRLF lines have no CR");
        check(ops.save(doc) == FileResult::Ok && startsWith(readRaw(fa), "a\r\nb"), "save keeps CRLF");
    }
    {   // invalid names, folders
        FileOperations ops;
        DoublyLinkedList doc;
        doc.insertLine(1, "x");
        check(ops.saveAs("   ", doc) == FileResult::InvalidName, "saveAs: blank name");
        check(ops.saveAs("no_such_dir/x.txt", doc) == FileResult::NotFound, "saveAs: missing folder");
        check(ops.saveAs(".", doc) == FileResult::NotAFile, "saveAs onto a folder");
        check(!ops.hasPath(), "failed saveAs keeps the document untitled");
    }
    {   // rename: success, then name already taken
        std::remove(fa);
        std::remove(fb);
        FileOperations ops;
        DoublyLinkedList doc;
        doc.insertLine(1, "x");
        check(ops.saveAs(fa, doc) == FileResult::Ok, "rename setup");
        check(ops.renameFile(fb) == FileResult::Ok && FileOperations::fileExists(fb) &&
                  !FileOperations::fileExists(fa),
              "rename moves the file");
        check(ops.saveAs(fa, doc) == FileResult::Ok && ops.renameFile(fb) == FileResult::AlreadyExists,
              "rename to an existing name");
    }

    std::remove(fa);
    std::remove(fb);

    if (failures == 0)
        std::cout << "All file I/O checks passed.\n";
    return failures == 0 ? 0 : 1;
}
