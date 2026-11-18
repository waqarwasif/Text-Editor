#include "Core/FileOperations.h"
#include "DataStructures/DoublyLinkedList.h"
#include "Utils/TextProcessing.h"

#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <system_error>

namespace fs = std::filesystem;

namespace
{

bool isValidPath(const mystring &path)
{
    if (TextProcessing::isBlank(path))
        return false;
    mystring name(fs::path(path.c_str()).filename().string().c_str());
    if (name.empty())
        return false;
#ifdef _WIN32
    if (TextProcessing::containsAnyOf(name, "<>:\"|?*"))
        return false;
#endif
    for (size_t i = 0; i < name.length(); ++i)
    {
        unsigned char c = static_cast<unsigned char>(name[i]);
        if (c < 0x20)
            return false;
    }
    return true;
}

FileResult fromError(const std::error_code &ec)
{
    return ec == std::errc::permission_denied ? FileResult::PermissionDenied
                                              : FileResult::WriteError;
}

bool readLine(const mystring &prompt, mystring &out)
{
    std::cout << prompt << std::flush;
    if (!mystring::getline(std::cin, out))
        return false;
    out = TextProcessing::trim(out);
    return true;
}

bool askYesNo(const mystring &question)
{
    mystring answer;
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

FileResult FileOperations::createNew(DoublyLinkedList &doc)
{
    doc.clear();
    filePath_.clear();
    eol_ = "\n";
    modified_ = false;
    return FileResult::Ok;
}

FileResult FileOperations::open(const mystring &path, DoublyLinkedList &doc)
{
    if (!isValidPath(path))
        return FileResult::InvalidName;

    const fs::path p(path.c_str());
    std::error_code ec;
    const bool exists = fs::exists(p, ec);
    if (ec)
        return FileResult::ReadError;
    if (!exists)
        return FileResult::NotFound;
    if (!fs::is_regular_file(p, ec))
        return FileResult::NotAFile;

    std::ifstream in(p, std::ios::binary);
    if (!in)
        return FileResult::PermissionDenied;

    DoublyLinkedList loaded;
    mystring detectedEol = "\n";

    try
    {
        mystring line;
        bool firstLine = true;
        bool hadCR = false;
        while (mystring::getline(in, line, &hadCR))
        {
            if (firstLine)
            {
                if (hadCR)
                    detectedEol = "\r\n";
                // Strip UTF-8 BOM (EF BB BF)
                if (line.size() >= 3 &&
                    static_cast<unsigned char>(line[0]) == 0xEF &&
                    static_cast<unsigned char>(line[1]) == 0xBB &&
                    static_cast<unsigned char>(line[2]) == 0xBF)
                    TextProcessing::eraseFront(line, 3);
                firstLine = false;
            }

            if (TextProcessing::contains(line, '\0'))
                return FileResult::BinaryFile;

            loaded.insertLine(loaded.getLineCount() + 1, line);
        }
        if (in.bad())
            return FileResult::ReadError;
    }
    catch (const std::exception &)
    {
        return FileResult::ReadError;
    }

    doc.swap(loaded);
    filePath_ = path;
    eol_ = detectedEol;
    modified_ = false;
    return FileResult::Ok;
}

FileResult FileOperations::save(const DoublyLinkedList &doc)
{
    if (!hasPath())
        return FileResult::NoFileName;
    const FileResult r = writeAtomic(filePath_, doc);
    if (r == FileResult::Ok)
        modified_ = false;
    return r;
}

FileResult FileOperations::saveAs(const mystring &path, const DoublyLinkedList &doc)
{
    if (!isValidPath(path))
        return FileResult::InvalidName;
    const FileResult r = writeAtomic(path, doc);
    if (r == FileResult::Ok)
    {
        filePath_ = path;
        modified_ = false;
    }
    return r;
}

FileResult FileOperations::writeAtomic(const mystring &path, const DoublyLinkedList &doc)
{
    const fs::path target(path.c_str());
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
    }

    fs::rename(tmp, target, ec);
    if (ec)
    {
        const FileResult r = fromError(ec);
        fs::remove(tmp, ec);
        return r;
    }
    return FileResult::Ok;
}

FileResult FileOperations::renameFile(const mystring &newName)
{
    if (!hasPath())
        return FileResult::NoFileName;
    if (!isValidPath(newName))
        return FileResult::InvalidName;

    const fs::path oldPath(filePath_.c_str());
    fs::path newPath(newName.c_str());
    if (!newPath.has_parent_path())
        newPath = oldPath.parent_path() / newPath;

    if (newPath == oldPath)
        return FileResult::Ok;

    std::error_code ec;
    const bool oldExists = fs::exists(oldPath, ec);
    if (ec)
        return FileResult::ReadError;
    if (!oldExists)
        return FileResult::NotFound;

    const fs::path parent = newPath.parent_path();
    if (!parent.empty() && !fs::is_directory(parent, ec))
        return FileResult::NotFound;

    const bool targetExists = fs::exists(newPath, ec);
    if (ec)
        return FileResult::ReadError;
    if (targetExists)
    {
        std::error_code eq;
        if (!(fs::equivalent(oldPath, newPath, eq) && !eq))
            return FileResult::AlreadyExists;
    }

    fs::rename(oldPath, newPath, ec);
    if (ec)
        return fromError(ec);

    filePath_ = newPath.string().c_str();
    return FileResult::Ok;
}

bool FileOperations::confirmUnsavedChanges(const DoublyLinkedList &doc)
{
    if (!modified_)
        return true;

    mystring answer;
    while (readLine("'" + displayName() + "' has unsaved changes. [S]ave / [D]iscard / [C]ancel: ", answer))
    {
        const char c = answer.empty() ? '\0'
                                      : TextProcessing::toLowerAscii(answer[0]);
        if (c == 's')
        {
            if (saveInteractive(doc))
                return true;
        }
        else if (c == 'd')
            return true;
        else if (c == 'c')
            return false;
        else
            std::cout << "Please enter S, D or C.\n";
    }
    return false;
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
        mystring path;
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

mystring FileOperations::displayName() const
{
    return hasPath() ? mystring(fs::path(filePath_.c_str()).filename().string().c_str()) : mystring("Untitled");
}

bool FileOperations::fileExists(const mystring &path)
{
    std::error_code ec;
    return fs::exists(fs::path(path.c_str()), ec) && !ec;
}

const char *FileOperations::describe(FileResult r)
{
    switch (r)
    {
    case FileResult::Ok:               return "Success.";
    case FileResult::NotFound:         return "File or folder not found.";
    case FileResult::NotAFile:         return "Path is a folder, not a file.";
    case FileResult::PermissionDenied: return "Permission denied.";
    case FileResult::AlreadyExists:    return "A file with that name already exists.";
    case FileResult::ReadError:        return "Error while reading the file.";
    case FileResult::WriteError:       return "Error while writing the file (disk full or locked?).";
    case FileResult::InvalidName:      return "Invalid file name.";
    case FileResult::NoFileName:       return "Document has no file name yet; use Save As.";
    case FileResult::BinaryFile:       return "This looks like a binary file, not text.";
    }
    return "Unknown error.";
}
