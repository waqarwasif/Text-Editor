#include "Utils/Menu.h"
#include "Core/TextEditor.h"
#include <iostream>
#include <limits>

static bool confirmOverwrite(const mystring &path)
{
    if (!FileOperations::fileExists(path))
        return true;
    std::cout << "'" << path << "' already exists. Overwrite? (y/n): ";
    mystring answer;
    if (!mystring::getline(std::cin, answer))
        return false;
    return !answer.empty() && (answer[0] == 'y' || answer[0] == 'Y');
}

void displayMenu(const FileOperations &fileOps, int undoCount)
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
    std::cout << "--- Replace ---\n";
    std::cout << "18. Replace One Occurrence\n";
    std::cout << "19. Replace All Occurrences\n";
    std::cout << "--- Undo ---\n";
    std::cout << "20. Undo Last Action";
    if (undoCount > 0)
        std::cout << " (" << undoCount << " available)";
    std::cout << "\n------------------------\n";
    std::cout << " 0. Exit\n";
    std::cout << "Select an option: ";
}

void handleCreateNew(TextEditor &editor)
{
    if (editor.files().confirmUnsavedChanges(editor.document()))
    {
        FileResult res = editor.files().createNew(editor.document());
        editor.clearUndo();
        std::cout << FileOperations::describe(res) << "\n";
    }
}

void handleCreateNew(DoublyLinkedList &doc, FileOperations &fileOps)
{
    if (fileOps.confirmUnsavedChanges(doc))
    {
        FileResult res = fileOps.createNew(doc);
        std::cout << FileOperations::describe(res) << "\n";
    }
}

void handleOpen(TextEditor &editor)
{
    if (editor.files().confirmUnsavedChanges(editor.document()))
    {
        mystring path;
        std::cout << "Enter file path to open: ";
        if (!mystring::getline(std::cin, path))
            return;
        FileResult res = editor.files().open(path, editor.document());
        if (res == FileResult::Ok)
            editor.clearUndo();
        std::cout << FileOperations::describe(res) << "\n";
    }
}

void handleOpen(DoublyLinkedList &doc, FileOperations &fileOps)
{
    if (fileOps.confirmUnsavedChanges(doc))
    {
        mystring path;
        std::cout << "Enter file path to open: ";
        if (!mystring::getline(std::cin, path))
            return;
        FileResult res = fileOps.open(path, doc);
        std::cout << FileOperations::describe(res) << "\n";
    }
}

void handleSave(DoublyLinkedList &doc, FileOperations &fileOps)
{
    if (!fileOps.hasPath())
    {
        mystring path;
        std::cout << "No file name set. Enter file path to save as: ";
        if (!mystring::getline(std::cin, path))
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
    mystring path;
    std::cout << "Enter new file path to save as: ";
    if (!mystring::getline(std::cin, path))
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
        const mystring &line = doc.getLine(lineNum);
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
    mystring text;
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
    if (!mystring::getline(std::cin, text))
        return;

    std::cout << TextEditor::describe(editor.insertNewLine(lineNum, text)) << "\n";
}

void handleInsertText(TextEditor &editor)
{
    int lineNum, column;
    mystring text;
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
    std::cout << "Enter column (1 = start of line): ";
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
    if (!mystring::getline(std::cin, text))
        return;

    std::cout << TextEditor::describe(editor.insertText(lineNum, column, text)) << "\n";
}

void handleModifyLine(TextEditor &editor)
{
    int lineNum;
    mystring newText;
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
    if (!mystring::getline(std::cin, newText))
        return;

    try
    {
        editor.modifyLine(lineNum, newText);
        std::cout << "Line " << lineNum << " modified successfully.\n";
    }
    catch (const std::exception &e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void handleModifyLine(DoublyLinkedList &doc, FileOperations &fileOps)
{
    int lineNum;
    mystring newText;
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
    if (!mystring::getline(std::cin, newText))
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

void handleDeleteLine(TextEditor &editor)
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
        editor.deleteLine(lineNum);
        std::cout << "Line deleted successfully.\n";
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

void handleDeleteRangeOfLines(TextEditor &editor)
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
        editor.deleteRangeOfLines(startLine, endLine);
        std::cout << "Lines " << startLine << " to " << endLine << " deleted successfully.\n";
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

void handleDeleteCharacter(TextEditor &editor)
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
        editor.deleteCharacterFromLine(lineNum, charIndex);
        std::cout << "Character at index " << charIndex << " on line " << lineNum << " deleted successfully.\n";
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

void handleDeleteTextRange(TextEditor &editor)
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
        editor.deleteTextFromLine(lineNum, startIndex, count);
        std::cout << "Deleted " << count << " character(s) from line " << lineNum << ".\n";
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

void handleDeleteWord(TextEditor &editor)
{
    mystring word;
    std::cout << "Enter word to delete: ";
    if (!mystring::getline(std::cin, word))
        return;

    if (word.empty())
    {
        std::cout << "Word cannot be empty.\n";
        return;
    }

    try
    {
        bool deleted = editor.deleteWord(word);
        if (deleted)
        {
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

void handleDeleteWord(DoublyLinkedList &doc, FileOperations &fileOps)
{
    mystring word;
    std::cout << "Enter word to delete: ";
    if (!mystring::getline(std::cin, word))
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
    mystring target;
    std::cout << "Enter word or text to search for: ";
    if (!mystring::getline(std::cin, target))
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

static bool readInt(const char *prompt, int &value)
{
    std::cout << prompt;
    if (!(std::cin >> value))
    {
        if (std::cin.eof())
            return false;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a valid number.\n";
        return false;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return true;
}

void handleReplaceOccurrence(TextEditor &editor)
{
    mystring target;
    mystring replacement;
    std::cout << "Enter text to find (case-sensitive): ";
    if (!mystring::getline(std::cin, target))
        return;
    if (target.empty())
    {
        std::cout << "Text to find cannot be empty.\n";
        return;
    }
    std::cout << "Enter replacement text (empty = delete the match): ";
    if (!mystring::getline(std::cin, replacement))
        return;

    int occurrence = 0;
    int line = 0;
    if (!readInt("Which occurrence to replace (1 = first): ", occurrence))
        return;
    if (!readInt("Line to search in (0 = whole document): ", line))
        return;

    ReplaceReport report;
    const EditResult result = editor.replaceOccurrence(target, replacement, occurrence, line, &report);
    if (result != EditResult::Ok)
    {
        std::cout << TextEditor::describe(result) << "\n";
        return;
    }
    std::cout << "Replaced occurrence " << occurrence << " at line " << report.line
              << ", column (index) " << (report.column - 1) << ".\n";
}

void handleReplaceAll(TextEditor &editor)
{
    mystring target;
    mystring replacement;
    std::cout << "Enter text to find (case-sensitive): ";
    if (!mystring::getline(std::cin, target))
        return;
    if (target.empty())
    {
        std::cout << "Text to find cannot be empty.\n";
        return;
    }
    std::cout << "Enter replacement text (empty = delete the matches): ";
    if (!mystring::getline(std::cin, replacement))
        return;

    int line = 0;
    if (!readInt("Line to replace in (0 = whole document): ", line))
        return;

    ReplaceReport report;
    const EditResult result = editor.replaceAll(target, replacement, line, &report);
    if (result != EditResult::Ok)
    {
        std::cout << TextEditor::describe(result) << "\n";
        return;
    }
    std::cout << "Replaced " << report.count << " occurrence(s).\n";
}

void handleUndo(TextEditor &editor)
{
    if (editor.undo())
    {
        std::cout << "Undo successful. Document restored to previous state.\n";
    }
    else
    {
        std::cout << "Nothing to undo (undo history is empty).\n";
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

        displayMenu(fileOps, editor.undoCount());

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
            handleCreateNew(editor);
            break;
        case 2:
            handleOpen(editor);
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
            handleModifyLine(editor);
            break;
        case 12:
            handleDeleteLine(editor);
            break;
        case 13:
            handleDeleteRangeOfLines(editor);
            break;
        case 14:
            handleDeleteCharacter(editor);
            break;
        case 15:
            handleDeleteTextRange(editor);
            break;
        case 16:
            handleDeleteWord(editor);
            break;
        case 17:
            handleSearch(doc);
            break;
        case 18:
            handleReplaceOccurrence(editor);
            break;
        case 19:
            handleReplaceAll(editor);
            break;
        case 20:
            handleUndo(editor);
            break;
        default:
            std::cout << "Invalid option. Please select a valid menu number.\n";
        }
    }
}
