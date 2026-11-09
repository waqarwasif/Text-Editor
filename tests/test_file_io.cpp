#include "Core/FileOperations.h"
#include "DataStructures/DoublyLinkedList.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

static int failures = 0;
static void check(bool ok, const char *name)
{
    if (!ok)
    {
        std::cout << "FAIL: " << name << "\n";
        ++failures;
    }
}

// Test-only helpers: the "no built-ins" rule applies to the editor, not to its tests.
static void writeRaw(const std::string &path, const std::string &bytes)
{
    std::ofstream out(path, std::ios::binary);
    out.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
}
static std::string readRaw(const std::string &path)
{
    std::ifstream in(path, std::ios::binary);
    return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}
static bool startsWith(const std::string &s, const std::string &prefix)
{
    return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

int main()
{
    const std::string fa = "tmp_fileio_a.txt";
    const std::string fb = "tmp_fileio_b.txt";
    std::remove(fa.c_str());
    std::remove(fb.c_str());

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
    {   // BOM stripped (eraseFront)
        writeRaw(fa, "\xEF\xBB\xBF" "hello\n");
        FileOperations ops;
        DoublyLinkedList doc;
        check(ops.open(fa, doc) == FileResult::Ok && doc.getLine(1) == "hello", "BOM stripped");
    }
    {   // NUL byte means binary (contains)
        writeRaw(fa, std::string("ab\0cd\n", 6));
        FileOperations ops;
        DoublyLinkedList doc;
        check(ops.open(fa, doc) == FileResult::BinaryFile, "NUL byte -> BinaryFile");
    }
    {   // CRLF: no CR in lines, and save keeps CRLF
        writeRaw(fa, "a\r\nb\r\n");
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
        std::remove(fa.c_str());
        std::remove(fb.c_str());
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

    std::remove(fa.c_str());
    std::remove(fb.c_str());

    if (failures == 0)
        std::cout << "All file I/O checks passed.\n";
    return failures == 0 ? 0 : 1;
}