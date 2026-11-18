#include "Core/FileOperations.h"
#include "DataStructures/DoublyLinkedList.h"
#include "Utils/TextProcessing.h"

#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <system_error>

namespace fs = std::filesystem;

// ============================================================ local helpers
namespace
{

    bool isValidPath(const std::string &path)
    {
        if (TextProcessing::isBlank(path))
            return false; // blank
        const std::string name = fs::path(path).filename().string();
        if (name.empty())
            return false; // path ends with a separator
#ifdef _WIN32
        if (TextProcessing::containsAnyOf(name, "<>:\"|?*"))
            return false;
#endif
        for (unsigned char c : name)
            if (c < 0x20)
                return false;
        return true;
    }

    FileResult fromError(const std::error_code &ec)
    {
        return ec == std::errc::permission_denied ? FileResult::PermissionDenied
                                                  : FileResult::WriteError;
    }

    // Returns false if input is closed (EOF) so callers never loop forever.
    bool readLine(const std::string &prompt, std::string &out)
    {
        std::cout << prompt << std::flush;
        if (!std::getline(std::cin, out))
            return false;
        out = TextProcessing::trim(out);
        return true;
    }

    bool askYesNo(const std::string &question)
    {
        std::string answer;
        while (readLine(question + " (y/n): ", answer))
        {
            if (!answer.empty() && TextProcessing::toLowerAscii(answer[0]) == 'y')
                return true;
            if (!answer.empty() && TextProcessing::toLowerAscii(answer[0]) == 'n')
                return false;
            std::cout << "Please answer y or n.\n";
        }
        return false;
    }

} // namespace

// ================================================================ new file
// Creates an empty, untitled document. The file itself is created on disk
// the first time the user saves (Save As).
FileResult FileOperations::createNew(DoublyLinkedList &doc)
{
    doc.clear();
    filePath_.clear();
    eol_ = "\n";
    modified_ = false;
    return FileResult::Ok;
}

// =============================================================== open file
FileResult FileOperations::open(const std::string &path, DoublyLinkedList &doc)
{
    if (!isValidPath(path))
        return FileResult::InvalidName;

    const fs::path p(path);
    std::error_code ec;
    const bool exists = fs::exists(p, ec);
    if (ec)
        return FileResult::ReadError;
    if (!exists)
        return FileResult::NotFound;
    if (!fs::is_regular_file(p, ec))
        return FileResult::NotAFile;

    // Binary mode: we strip "\r" ourselves, so behaviour is the same on every OS.
    std::ifstream in(p, std::ios::binary);
    if (!in)
        return FileResult::PermissionDenied;

    // Load into a TEMPORARY list. The live document is replaced only after
    // everything succeeded, so a failed open never destroys unsaved work.
    DoublyLinkedList loaded;
    std::string detectedEol = "\n";

    try
    {
        std::string line;
        bool firstLine = true;
        while (std::getline(in, line))
        {
            const bool hadCR = !line.empty() && line.back() == '\r';
            if (hadCR)
                line.pop_back();

            if (firstLine)
            {
                if (hadCR)
                    detectedEol = "\r\n";
                // Strip UTF-8 BOM (EF BB BF).
                if (line.size() >= 3 &&
                    static_cast<unsigned char>(line[0]) == 0xEF &&
                    static_cast<unsigned char>(line[1]) == 0xBB &&
                    static_cast<unsigned char>(line[2]) == 0xBF)
                    TextProcessing::eraseFront(line, 3);
                firstLine = false;
            }

            if (TextProcessing::contains(line, '\0'))
                return FileResult::BinaryFile;

            loaded.insertLine(loaded.getLineCount() + 1, line); // O(1): tail insert
        }
        if (in.bad())
            return FileResult::ReadError;
    }
    catch (const std::exception &) // e.g. out of memory on a huge file
    {
        return FileResult::ReadError; // 'loaded' is destroyed, 'doc' untouched
    }

    doc.swap(loaded); // O(1); the old content dies with 'loaded'
    filePath_ = path;
    eol_ = detectedEol;
    modified_ = false;
    return FileResult::Ok;
}

// ================================================================== save
FileResult FileOperations::save(const DoublyLinkedList &doc)
{
    if (!hasPath())
        return FileResult::NoFileName; // caller falls back to Save As
    const FileResult r = writeAtomic(filePath_, doc);
    if (r == FileResult::Ok)
        modified_ = false;
    return r;
}

FileResult FileOperations::saveAs(const std::string &path, const DoublyLinkedList &doc)
{
    if (!isValidPath(path))
        return FileResult::InvalidName;
    const FileResult r = writeAtomic(path, doc);
    if (r == FileResult::Ok)
    {
        filePath_ = path; // switch target only after a successful write
        modified_ = false;
    }
    return r;
}

// Writes "<name>.edtmp", verifies it, then renames it over the target.
// A crash or full disk can never leave a half-written original behind.
FileResult FileOperations::writeAtomic(const std::string &path, const DoublyLinkedList &doc)
{
    const fs::path target(path);
    std::error_code ec;

    if (fs::is_directory(target, ec))
        return FileResult::NotAFile;

    const fs::path parent = target.parent_path();
    if (!parent.empty() && !fs::is_directory(parent, ec))
        return FileResult::NotFound;

    fs::path tmp = target;
    tmp += ".edtmp";

    {
        std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
        if (!out)
            return FileResult::PermissionDenied;

        // Each line ends with eol_, so the file ends with a newline and
        // repeated saves never change its size.
        for (const Node *n = doc.getHead(); n != nullptr; n = n->next)
        {
            out << n->textData << eol_;
            if (!out)
            {
                out.close();
                fs::remove(tmp, ec);
                return FileResult::WriteError;
            }
        }
        out.flush();
        if (!out)
        {
            out.close();
            fs::remove(tmp, ec);
            return FileResult::WriteError;
        }
    } // stream closed: data is on disk

    fs::rename(tmp, target, ec);
    if (ec)
    {
        const FileResult r = fromError(ec);
        fs::remove(tmp, ec);
        return r;
    }
    return FileResult::Ok;
}

// ================================================================ rename
// Renames the current file on disk. A bare name keeps the file in its folder.
// Unsaved edits stay unsaved: renaming does not write content.
FileResult FileOperations::renameFile(const std::string &newName)
{
    if (!hasPath())
        return FileResult::NoFileName;
    if (!isValidPath(newName))
        return FileResult::InvalidName;

    const fs::path oldPath(filePath_);
    fs::path newPath(newName);
    if (!newPath.has_parent_path())
        newPath = oldPath.parent_path() / newPath;

    if (newPath == oldPath)
        return FileResult::Ok; // nothing to do

    std::error_code ec;
    const bool oldExists = fs::exists(oldPath, ec);
    if (ec)
        return FileResult::ReadError;
    if (!oldExists)
        return FileResult::NotFound; // deleted or moved outside the editor

    const fs::path parent = newPath.parent_path();
    if (!parent.empty() && !fs::is_directory(parent, ec))
        return FileResult::NotFound;

    const bool targetExists = fs::exists(newPath, ec);
    if (ec)
        return FileResult::ReadError;
    if (targetExists)
    {
        // Same file, only different letter case (a.txt -> A.txt on Windows) is allowed.
        std::error_code eq;
        if (!(fs::equivalent(oldPath, newPath, eq) && !eq))
            return FileResult::AlreadyExists; // never overwrite silently
    }

    fs::rename(oldPath, newPath, ec);
    if (ec)
        return fromError(ec);

    filePath_ = newPath.string();
    return FileResult::Ok;
}

// ================================================== interactive: unsaved work
bool FileOperations::confirmUnsavedChanges(const DoublyLinkedList &doc)
{
    if (!modified_)
        return true;

    std::string answer;
    while (readLine("'" + displayName() + "' has unsaved changes. [S]ave / [D]iscard / [C]ancel: ", answer))
    {
        const char c = answer.empty() ? '\0'
                                      : TextProcessing::toLowerAscii(answer[0]);
        if (c == 's')
        {
            if (saveInteractive(doc))
                return true;
            // save failed or was cancelled: ask again so the user can retry or discard
        }
        else if (c == 'd')
            return true;
        else if (c == 'c')
            return false;
        else
            std::cout << "Please enter S, D or C.\n";
    }
    return false; // input closed: never discard silently
}

bool FileOperations::exitEditor(const DoublyLinkedList &doc)
{
    return confirmUnsavedChanges(doc);
}

bool FileOperations::saveInteractive(const DoublyLinkedList &doc)
{
    FileResult r;
    if (hasPath())
    {
        r = save(doc);
    }
    else
    {
        std::string path;
        if (!readLine("Save as (leave blank to cancel): ", path) || path.empty())
            return false;
        if (fileExists(path) && !askYesNo("'" + path + "' already exists. Overwrite?"))
            return false;
        r = saveAs(path, doc);
    }

    if (r != FileResult::Ok)
    {
        std::cout << "Save failed: " << describe(r) << "\n";
        return false;
    }
    return true;
}

// ================================================================ helpers
std::string FileOperations::displayName() const
{
    return hasPath() ? fs::path(filePath_).filename().string() : "Untitled";
}

bool FileOperations::fileExists(const std::string &path)
{
    std::error_code ec;
    return fs::exists(fs::path(path), ec) && !ec;
}

std::string FileOperations::describe(FileResult r)
{
    switch (r)
    {
    case FileResult::Ok:
        return "Success.";
    case FileResult::NotFound:
        return "File or folder not found.";
    case FileResult::NotAFile:
        return "Path is a folder, not a file.";
    case FileResult::PermissionDenied:
        return "Permission denied.";
    case FileResult::AlreadyExists:
        return "A file with that name already exists.";
    case FileResult::ReadError:
        return "Error while reading the file.";
    case FileResult::WriteError:
        return "Error while writing the file (disk full or locked?).";
    case FileResult::InvalidName:
        return "Invalid file name.";
    case FileResult::NoFileName:
        return "Document has no file name yet; use Save As.";
    case FileResult::BinaryFile:
        return "This looks like a binary file, not text.";
    }
    return "Unknown error.";
}