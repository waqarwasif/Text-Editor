#include <iostream>
#include <string>
#include <limits>
#include "DataStructures/DoublyLinkedList.h"
#include "Core/FileOperations.h"
#include "Core/TextEditor.h"
static bool confirmOverwrite(const std::string &path)
{
    if (!FileOperations::fileExists(path))
        return true;
    std::cout << "'" << path << "' already exists. Overwrite? (y/n): ";
    std::string answer;
    std::getline(std::cin, answer);
    return !answer.empty() && (answer[0] == 'y' || answer[0] == 'Y');
}
void displayMenu(const FileOperations &fileOps)
{
    std::cout << "\n=== TEXT EDITOR MENU ===\n";
    std::cout << "Current File: " << fileOps.displayName();
    if (fileOps.isModified())
        std::cout << " (Unsaved Changes)";
    std::cout << "\n------------------------\n";
    std::cout << "1. Create New Document\n";
    std::cout << "2. Open Document\n";
    std::cout << "3. Save Document\n";
    std::cout << "4. Save As...\n";
    std::cout << "5. Display Document\n";
    std::cout << "6. Display Document Line by Line\n";
    std::cout << "7. Display Document Statistics\n";
    std::cout << "8. Insert Line\n";
    std::cout << "9. Delete Line\n";
    std::cout << "10. Insert Text at Position\n";
    std::cout << "0. Exit\n";
    std::cout << "Select an option: ";
}

static bool readInt(const char *prompt, int &out)
{
    std::cout << prompt;
    const bool ok = static_cast<bool>(std::cin >> out);
    if (!ok)
        std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return ok;
}

int main()
{
    TextEditor editor;
    DoublyLinkedList &doc = editor.document();   // same object the editor edits
    FileOperations &fileOps = editor.files();
    int choice;

    std::cout << "Welcome to the CS-218 Text Editor.\n";

    while (true)
    {
        displayMenu(fileOps);

        // Handle invalid non-integer inputs gracefully[cite: 6]
        if (!(std::cin >> choice))
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

        if (choice == 0)
        {
            if (fileOps.exitEditor(doc))
            {
                std::cout << "Exiting editor...\n";
                break;
            }
            continue; // User cancelled exit
        }

        switch (choice)
        {
        case 1: // Create New Document[cite: 5]
            if (fileOps.confirmUnsavedChanges(doc))
            {
                FileResult res = fileOps.createNew(doc);
                std::cout << FileOperations::describe(res) << "\n";
            }
            break;

        case 2:
        { // Open Existing Text File[cite: 5]
            if (fileOps.confirmUnsavedChanges(doc))
            {
                std::string path;
                std::cout << "Enter file path to open: ";
                std::getline(std::cin, path);
                FileResult res = fileOps.open(path, doc);
                std::cout << FileOperations::describe(res) << "\n";
            }
            break;
        }

        case 3:
        { // Save Current Document[cite: 5]
            if (!fileOps.hasPath())
            {
                std::string path;
                std::cout << "No file name set. Enter file path to save as: ";
                std::getline(std::cin, path);
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
            break;
        }

        case 4:
        { // Save Under New Filename[cite: 5]
            std::string path;
            std::cout << "Enter new file path to save as: ";
            std::getline(std::cin, path);
            if (!path.empty() && confirmOverwrite(path))
            {
                FileResult res = fileOps.saveAs(path, doc);
                std::cout << FileOperations::describe(res) << "\n";
            }
            break;
        }

        case 5: // Display Document[cite: 5]
            std::cout << "\n--- Document Contents ---\n";
            doc.displayDocument();
            std::cout << "-------------------------\n";
            break;

        case 6: // Display Document Line by Line[cite: 5]
            std::cout << "\n--- Document Contents ---\n";
            doc.displayDocumentLineByLine();
            std::cout << "-------------------------\n";
            break;

        case 7:
            doc.displayDocumentInfo();
            break;

          case 8:
        { // Insert a whole new line
            int lineNum;
            std::string text;
            if (!readInt("Enter line number to insert at: ", lineNum))
            {
                std::cout << "Invalid number.\n";
                break;
            }
            std::cout << "Enter text (empty = blank line): ";
            std::getline(std::cin, text);
            std::cout << TextEditor::describe(editor.insertNewLine(lineNum, text)) << "\n";
            break;
        }

        case 9:
        { // Delete a Line[cite: 6]
            int lineNum;
            std::cout << "Enter line number to delete: ";
            std::cin >> lineNum;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            try
            {
                doc.deleteLine(lineNum);
                fileOps.markModified(); // Crucial: Tells FileOps the document changed
                std::cout << "Line deleted.\n";
            }
            catch (const std::exception &e)
            {
                std::cout << "Error: " << e.what() << "\n";
            }
            break;
        }

        case 10:
        { // Insert text inside a line
            int lineNum, column;
            std::string text;
            if (!readInt("Line number: ", lineNum) || !readInt("Column: ", column))
            {
                std::cout << "Invalid number.\n";
                break;
            }
            std::cout << "Text to insert: ";
            std::getline(std::cin, text);
            std::cout << TextEditor::describe(editor.insertText(lineNum, column, text)) << "\n";
            break;
        }

        default:
            std::cout << "Invalid option. Please select a valid menu number.\n";
        }
    }

    return 0;
}