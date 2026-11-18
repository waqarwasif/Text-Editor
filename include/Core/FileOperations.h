#pragma once

#include "DataStructures/MyString.h"

class DoublyLinkedList;

enum class FileResult
{
    Ok,
    NotFound,         // file or target folder does not exist
    NotAFile,         // path is a directory, not a regular file
    PermissionDenied, // exists but cannot be opened / replaced
    AlreadyExists,    // rename target is taken
    ReadError,
    WriteError,
    InvalidName,      // blank or illegal characters
    NoFileName,       // document has no path yet
    BinaryFile        // contains NUL bytes, not text
};

class FileOperations
{
public:
    FileResult createNew(DoublyLinkedList &doc);
    FileResult open(const mystring &path, DoublyLinkedList &doc);
    FileResult save(const DoublyLinkedList &doc);
    FileResult saveAs(const mystring &path, const DoublyLinkedList &doc);
    FileResult renameFile(const mystring &newName);

    bool confirmUnsavedChanges(const DoublyLinkedList &doc);
    bool exitEditor(const DoublyLinkedList &doc);

    bool hasPath() const { return !filePath_.empty(); }
    const mystring &currentPath() const { return filePath_; }
    mystring displayName() const;
    bool isModified() const { return modified_; }
    void markModified() { modified_ = true; }

    static bool fileExists(const mystring &path);
    static const char *describe(FileResult result);

private:
    FileResult writeAtomic(const mystring &path, const DoublyLinkedList &doc);
    bool saveInteractive(const DoublyLinkedList &doc);

    mystring filePath_;
    mystring eol_ = "\n";
    bool modified_ = false;
};
