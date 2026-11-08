#pragma once
#include <string>

class DoublyLinkedList; // full definition is only needed in FileOperations.cpp

// Outcome of a file operation. The core functions report; they never print.
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
    // ---- Core features (non-interactive, return a FileResult) ----
    FileResult createNew(DoublyLinkedList &doc);
    FileResult open(const std::string &path, DoublyLinkedList &doc);
    FileResult save(const DoublyLinkedList &doc);
    FileResult saveAs(const std::string &path, const DoublyLinkedList &doc);
    FileResult renameFile(const std::string &newName);

    // ---- Interactive guards (console prompts) ----
    // Call before createNew / open / exit. Returns true if it is safe to continue.
    bool confirmUnsavedChanges(const DoublyLinkedList &doc);
    bool exitEditor(const DoublyLinkedList &doc); // true = OK to quit

    // ---- State ----
    bool hasPath() const { return !filePath_.empty(); }
    const std::string &currentPath() const { return filePath_; }
    std::string displayName() const; // file name only, or "Untitled"
    bool isModified() const { return modified_; }
    void markModified() { modified_ = true; } // call after every edit

    // ---- Helpers ----
    static bool fileExists(const std::string &path);
    static std::string describe(FileResult result);

private:
    FileResult writeAtomic(const std::string &path, const DoublyLinkedList &doc);
    bool saveInteractive(const DoublyLinkedList &doc);

    std::string filePath_;   // "" = unsaved new document
    std::string eol_ = "\n"; // line ending of the opened file (LF or CRLF)
    bool modified_ = false;  // unsaved changes exist
};