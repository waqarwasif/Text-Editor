#include "Utils/Menu.h"
#include "Core/TextEditor.h"
#include <iostream>
#include <string>
#include <limits>

static bool confirmOverwrite(const std::string &path)
{
    if (!FileOperations::fileExists(path))
        return true;
    std::cout << "'" << path << "' already exists. Overwrite? (y/n): ";
    std::string answer;
    if (!std::getline(std::cin, answer))
        return false;
    return !answer.empty() && (answer[0] == 'y' || answer[0] == 'Y');
}

void displayMenu(const FileOperations &fileOps)
{
    std::cout << "\n=== TEXT EDITOR MENU ===\n";
    std::cout << "Current File: " << fileOps.displayName();
    if (fileOps.isModified())
        std::cout << " (Unsaved Changes)";
    std::cout << "\n------------------------\n";
    std::cout << "--- File Operations ---\n";
    std::cout << " 1. Create New Document\n";
    std::cout << " 2. Open Document\n";
    std::cout << " 3. Save Document\n";
    std::cout << " 4. Save As...\n";
    std::cout << "--- Text Display & Info ---\n";
    std::cout << " 5. Display Document\n";
    std::cout << " 6. Display Document Line by Line\n";
    std::cout << " 7. Display Document Statistics\n";
    std::cout << " 8. View Specific Line\n";
    std::cout << "--- Text Insertion & Modification ---\n";
    std::cout << " 9. Insert Line\n";
    std::cout << "10. Insert Text at Position\n";
    std::cout << "11. Modify Line\n";
    std::cout << "--- Text Deletion ---\n";
    std::cout << "12. Delete Line\n";
    std::cout << "13. Delete Range of Lines\n";
    std::cout << "14. Delete Character from Line\n";
    std::cout << "15. Delete Text Range from Line\n";
    std::cout << "16. Delete Word\n";
    std::cout << "--- Search ---\n";
    std::cout << "17. Search Document\n";
    std::cout << "------------------------\n";
    std::cout << " 0. Exit\n";
    std::cout << "Select an option: ";
}

void handleCreateNew(DoublyLinkedList &doc, FileOperations &fileOps)
{
    if (fileOps.confirmUnsavedChanges(doc))
    {
        FileResult res = fileOps.createNew(doc);
        std::cout << FileOperations::describe(res) << "\n";
    }
}

void handleOpen(DoublyLinkedList &doc, FileOperations &fileOps)
{
    if (fileOps.confirmUnsavedChanges(doc))
    {
        std::string path;
        std::cout << "Enter file path to open: ";
        if (!std::getline(std::cin, path))
            return;
        FileResult res = fileOps.open(path, doc);
        std::cout << FileOperations::describe(res) << "\n";
    }
}

void handleSave(DoublyLinkedList &doc, FileOperations &fileOps)
{
    if (!fileOps.hasPath())
    {
        std::string path;
        std::cout << "No file name set. Enter file path to save as: ";
        if (!std::getline(std::cin, path))
            return;
        if (!path.empty() && confirmOverwrite(path))
        {
            FileResult res = fileOps.saveAs(path, doc);
            std::cout << FileOperations::describe(res) << "\n";
        }
    }
    else
    {
        FileResult res = fileOps.save(doc);
        std::cout << FileOperations::describe(res) << "\n";
    }
}

void handleSaveAs(DoublyLinkedList &doc, FileOperations &fileOps)
{
    std::string path;
    std::cout << "Enter new file path to save as: ";
    if (!std::getline(std::cin, path))
        return;
    if (!path.empty() && confirmOverwrite(path))
    {
        FileResult res = fileOps.saveAs(path, doc);
        std::cout << FileOperations::describe(res) << "\n";
    }
}

void handleDisplayDocument(const DoublyLinkedList &doc)
{
    std::cout << "\n--- Document Contents ---\n";
    doc.displayDocument();
    std::cout << "-------------------------\n";
}

void handleDisplayLineByLine(const DoublyLinkedList &doc)
{
    std::cout << "\n--- Document Contents (Line by Line) ---\n";
    doc.displayDocumentLineByLine();
    std::cout << "----------------------------------------\n";
}

void handleDisplayStatistics(const DoublyLinkedList &doc)
{
    doc.displayDocumentInfo();
}

void handleGetLine(const DoublyLinkedList &doc)
{
    int lineNum;
    std::cout << "Enter line number to view: ";
    if (!(std::cin >> lineNum))
    {
        if (std::cin.eof())
            return;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a valid number.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    try
    {
        std::string line = doc.getLine(lineNum);
        std::cout << "Line " << lineNum << ": " << line << "\n";
    }
    catch (const std::exception &e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void handleInsertLine(TextEditor &editor)
{
    int lineNum;
    std::string text;
    std::cout << "Enter line number to insert at: ";
    if (!(std::cin >> lineNum))
    {
        if (std::cin.eof())
            return;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a valid number.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Enter text (empty = blank line): ";
    if (!std::getline(std::cin, text))
        return;

    std::cout << TextEditor::describe(editor.insertNewLine(lineNum, text)) << "\n";
}

void handleInsertText(TextEditor &editor)
{
    int lineNum, column;
    std::string text;
    std::cout << "Enter line number: ";
    if (!(std::cin >> lineNum))
    {
        if (std::cin.eof())
            return;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a valid number.\n";
        return;
    }
    std::cout << "Enter column (0-based index): ";
    if (!(std::cin >> column))
    {
        if (std::cin.eof())
            return;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a valid number.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Enter text to insert: ";
    if (!std::getline(std::cin, text))
        return;

    std::cout << TextEditor::describe(editor.insertText(lineNum, column, text)) << "\n";
}

void handleModifyLine(DoublyLinkedList &doc, FileOperations &fileOps)
{
    int lineNum;
    std::string newText;
    std::cout << "Enter line number to modify: ";
    if (!(std::cin >> lineNum))
    {
        if (std::cin.eof())
            return;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a valid number.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Enter new text: ";
    if (!std::getline(std::cin, newText))
        return;

    try
    {
        doc.modifyLine(lineNum, newText);
        fileOps.markModified();
        std::cout << "Line " << lineNum << " modified successfully.\n";
    }
    catch (const std::exception &e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void handleDeleteLine(DoublyLinkedList &doc, FileOperations &fileOps)
{
    int lineNum;
    std::cout << "Enter line number to delete: ";
    if (!(std::cin >> lineNum))
    {
        if (std::cin.eof())
            return;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a valid number.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    try
    {
        doc.deleteLine(lineNum);
        fileOps.markModified();
        std::cout << "Line deleted successfully.\n";
    }
    catch (const std::exception &e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void handleDeleteRangeOfLines(DoublyLinkedList &doc, FileOperations &fileOps)
{
    int startLine, endLine;
    std::cout << "Enter start line number: ";
    if (!(std::cin >> startLine))
    {
        if (std::cin.eof())
            return;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input.\n";
        return;
    }
    std::cout << "Enter end line number: ";
    if (!(std::cin >> endLine))
    {
        if (std::cin.eof())
            return;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    try
    {
        doc.deleteRangeOfLines(startLine, endLine);
        fileOps.markModified();
        std::cout << "Lines " << startLine << " to " << endLine << " deleted successfully.\n";
    }
    catch (const std::exception &e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void handleDeleteCharacter(DoublyLinkedList &doc, FileOperations &fileOps)
{
    int lineNum, charIndex;
    std::cout << "Enter line number: ";
    if (!(std::cin >> lineNum))
    {
        if (std::cin.eof())
            return;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input.\n";
        return;
    }
    std::cout << "Enter character index (0-based): ";
    if (!(std::cin >> charIndex))
    {
        if (std::cin.eof())
            return;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    try
    {
        doc.deleteCharacterFromLine(lineNum, charIndex);
        fileOps.markModified();
        std::cout << "Character at index " << charIndex << " on line " << lineNum << " deleted successfully.\n";
    }
    catch (const std::exception &e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void handleDeleteTextRange(DoublyLinkedList &doc, FileOperations &fileOps)
{
    int lineNum, startIndex, count;
    std::cout << "Enter line number: ";
    if (!(std::cin >> lineNum))
    {
        if (std::cin.eof())
            return;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input.\n";
        return;
    }
    std::cout << "Enter start index (0-based): ";
    if (!(std::cin >> startIndex))
    {
        if (std::cin.eof())
            return;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input.\n";
        return;
    }
    std::cout << "Enter number of characters to delete: ";
    if (!(std::cin >> count))
    {
        if (std::cin.eof())
            return;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    try
    {
        doc.deleteTextFromLine(lineNum, startIndex, count);
        fileOps.markModified();
        std::cout << "Deleted " << count << " character(s) from line " << lineNum << ".\n";
    }
    catch (const std::exception &e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void handleDeleteWord(DoublyLinkedList &doc, FileOperations &fileOps)
{
    std::string word;
    std::cout << "Enter word to delete: ";
    if (!std::getline(std::cin, word))
        return;

    if (word.empty())
    {
        std::cout << "Word cannot be empty.\n";
        return;
    }

    try
    {
        bool deleted = doc.deleteWord(word);
        if (deleted)
        {
            fileOps.markModified();
            std::cout << "Word \"" << word << "\" deleted successfully.\n";
        }
        else
        {
            std::cout << "Word \"" << word << "\" not found in document.\n";
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void handleSearch(const DoublyLinkedList &doc)
{
    std::string target;
    std::cout << "Enter word or text to search for: ";
    if (!std::getline(std::cin, target))
        return;

    if (target.empty())
    {
        std::cout << "Search target cannot be empty.\n";
        return;
    }

    ResultArray results = doc.searchDocument(target);
    if (results.size() == 0)
    {
        std::cout << "No matches found for \"" << target << "\".\n";
    }
    else
    {
        std::cout << "Found " << results.size() << " match(es) for \"" << target << "\":\n";
        for (int i = 0; i < results.size(); ++i)
        {
            SearchResult match = results.get(i);
            std::cout << "  - Line " << match.lineNumber << ", Column (index) " << match.startIndex << "\n";
        }
    }
}

void menu(TextEditor &editor)
{
    DoublyLinkedList &doc = editor.document();
    FileOperations &fileOps = editor.files();
    int choice;
    std::cout << "Welcome to the CS-218 Text Editor.\n";

    while (true)
    {
        if (std::cin.eof())
        {
            break;
        }

        displayMenu(fileOps);

        if (!(std::cin >> choice))
        {
            if (std::cin.eof())
            {
                break;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 0)
        {
            if (fileOps.exitEditor(doc))
            {
                std::cout << "Exiting editor...\n";
                break;
            }
            if (std::cin.eof())
            {
                break;
            }
            continue;
        }

        switch (choice)
        {
        case 1:
            handleCreateNew(doc, fileOps);
            break;
        case 2:
            handleOpen(doc, fileOps);
            break;
        case 3:
            handleSave(doc, fileOps);
            break;
        case 4:
            handleSaveAs(doc, fileOps);
            break;
        case 5:
            handleDisplayDocument(doc);
            break;
        case 6:
            handleDisplayLineByLine(doc);
            break;
        case 7:
            handleDisplayStatistics(doc);
            break;
        case 8:
            handleGetLine(doc);
            break;
        case 9:
            handleInsertLine(editor);
            break;
        case 10:
            handleInsertText(editor);
            break;
        case 11:
            handleModifyLine(doc, fileOps);
            break;
        case 12:
            handleDeleteLine(doc, fileOps);
            break;
        case 13:
            handleDeleteRangeOfLines(doc, fileOps);
            break;
        case 14:
            handleDeleteCharacter(doc, fileOps);
            break;
        case 15:
            handleDeleteTextRange(doc, fileOps);
            break;
        case 16:
            handleDeleteWord(doc, fileOps);
            break;
        case 17:
            handleSearch(doc);
            break;
        default:
            std::cout << "Invalid option. Please select a valid menu number.\n";
        }
    }
}