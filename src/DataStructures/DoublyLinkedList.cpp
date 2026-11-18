#include "DataStructures/DoublyLinkedList.h"
#include <stdexcept>

DoublyLinkedList::DoublyLinkedList() : head(nullptr), tail(nullptr), lineCount(0), totalCharacters(0) {}

void DoublyLinkedList::clear()
{
    Node *current = head;
    while (current != nullptr)
    {
        Node *nextNode = current->next;
        delete current;
        current = nextNode;
    }
    head = tail = nullptr;
    lineCount = 0;
    totalCharacters = 0;
}

DoublyLinkedList::~DoublyLinkedList()
{
    clear();
}

void DoublyLinkedList::swap(DoublyLinkedList &other) noexcept
{
    std::swap(head, other.head);
    std::swap(tail, other.tail);
    std::swap(lineCount, other.lineCount);
    std::swap(totalCharacters, other.totalCharacters);
}

void DoublyLinkedList::insertLine(int lineNumber, const std::string &text)
{
    if (lineNumber < 1 || lineNumber > lineCount + 1)
    {
        throw std::out_of_range("Line number out of bounds.");
    }

    Node *newNode = new Node(text);

    if (head == nullptr)
    {
        head = tail = newNode;
    }
    else if (lineNumber == 1)
    {
        newNode->next = head;
        head->prev = newNode;
        head = newNode;
    }
    else if (lineNumber == lineCount + 1)
    {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
    else
    {
        Node *current = head;
        for (int i = 1; i < lineNumber - 1; ++i)
        {
            current = current->next;
        }
        newNode->next = current->next;
        newNode->prev = current;
        if (current->next != nullptr)
        {
            current->next->prev = newNode;
        }
        current->next = newNode;
    }
    lineCount++;
    totalCharacters += calculateLength(text);
}

void DoublyLinkedList::deleteLine(int lineNumber)
{
    if (lineNumber < 1 || lineNumber > lineCount || head == nullptr)
    {
        throw std::out_of_range("Invalid line number for deletion.");
    }

    Node *nodeToDelete = head;

    for (int i = 1; i < lineNumber; ++i)
    {
        nodeToDelete = nodeToDelete->next;
    }

    if (nodeToDelete->prev != nullptr)
    {
        nodeToDelete->prev->next = nodeToDelete->next;
    }
    else
    {
        head = nodeToDelete->next;
    }

    if (nodeToDelete->next != nullptr)
    {
        nodeToDelete->next->prev = nodeToDelete->prev;
    }
    else
    {
        tail = nodeToDelete->prev;
    }

    totalCharacters -= calculateLength(nodeToDelete->textData);
    delete nodeToDelete;
    lineCount--;
}

void DoublyLinkedList::deleteRangeOfLines(int startLine, int endLine)
{
    if (head == nullptr || startLine < 1 || startLine > lineCount)
    {
        throw std::out_of_range("Invalid start line number for deletion.");
    }
    if (endLine < 1 || endLine > lineCount)
    {
        throw std::out_of_range("Invalid end line number for deletion.");
    }
    if (startLine > endLine)
    {
        throw std::invalid_argument("startLine cannot be greater than endLine.");
    }

    Node *nodeToDelete = head;
    for (int i = 1; i < startLine; ++i)
    {
        nodeToDelete = nodeToDelete->next;
    }

    Node *nodeBeforeDelete = nodeToDelete->prev;
    Node *current;
    int linesDeleted = (endLine - startLine) + 1;

    int currentLine = startLine;
    while (currentLine <= endLine)
    {
        current = nodeToDelete;
        nodeToDelete = nodeToDelete->next;

        totalCharacters -= calculateLength(current->textData);

        delete current;
        currentLine++;
    }

    if (nodeBeforeDelete == nullptr)
    {
        head = nodeToDelete;
    }
    else
    {
        nodeBeforeDelete->next = nodeToDelete;
    }

    if (nodeToDelete != nullptr)
    {
        nodeToDelete->prev = nodeBeforeDelete;
    }
    else
    {
        tail = nodeBeforeDelete;
    }

    lineCount -= linesDeleted;
}

void DoublyLinkedList::displayDocumentInfo() const
{
    std::cout << "\n--- Document Statistics ---\n";
    if (head == nullptr)
    {
        std::cout << "The document is currently empty.\n";
        std::cout << "Total lines: 0\n";
        std::cout << "Total characters: 0\n";
    }
    else
    {
        std::cout << "Total lines: " << lineCount << "\n";
        std::cout << "Total characters: " << totalCharacters << "\n";
    }
    std::cout << "---------------------------\n";
}

void DoublyLinkedList::displayDocumentLineByLine() const
{
    if (head == nullptr)
    {
        std::cout << "[Document is empty]" << std::endl;
        return;
    }

    Node *current = head;
    int currentLine = 1;
    while (current != nullptr)
    {
        std::cout << currentLine << ": " << current->textData << std::endl;
        current = current->next;
        currentLine++;
    }
    std::cout << "\nTotal lines: " << lineCount << std::endl;
}

void DoublyLinkedList::displayDocument() const
{
    if (head == nullptr)
    {
        std::cout << "[Document is empty]" << std::endl;
        return;
    }

    Node *current = head;
    while (current != nullptr)
    {
        std::cout << current->textData << std::endl;
        current = current->next;
    }
    std::cout << "\nTotal lines: " << lineCount << std::endl;
}

int DoublyLinkedList::calculateLength(const std::string &str) const
{
    int count = 0;
    for (int i = 0; str[i]; i++)
    {
        count++;
    }
    return count;
}

ResultArray DoublyLinkedList::searchDocument(const std::string &target) const
{
    ResultArray results;
    int targetLength = calculateLength(target);

    if (targetLength == 0 || head == nullptr)
    {
        return results;
    }

    Node *current = head;
    int currentLine = 1;

    while (current != nullptr)
    {
        int lineLength = calculateLength(current->textData);

        if (lineLength >= targetLength)
        {
            for (int i = 0; i <= lineLength - targetLength; ++i)
            {
                bool match = true;

                for (int j = 0; j < targetLength; ++j)
                {
                    if (current->textData[i + j] != target[j])
                    {
                        match = false;
                        break;
                    }
                }

                if (match)
                {
                    results.add({currentLine, i});
                }
            }
        }
        current = current->next;
        currentLine++;
    }

    return results;
}

void DoublyLinkedList::deleteTextFromLine(int lineNumber, int startIndex, int count)
{
    if (head == nullptr || lineNumber < 1 || lineNumber > lineCount)
    {
        throw std::out_of_range("Invalid line number.");
    }

    Node *current = head;
    for (int i = 1; i < lineNumber; ++i)
    {
        current = current->next;
    }

    int lineLength = calculateLength(current->textData);

    if (startIndex < 0 || startIndex >= lineLength || count <= 0)
    {
        throw std::out_of_range("Invalid start index or count.");
    }

    if (startIndex + count > lineLength)
    {
        count = lineLength - startIndex;
    }

    for (int i = startIndex; i < lineLength - count; ++i)
    {
        current->textData[i] = current->textData[i + count];
    }

    current->textData.resize(lineLength - count);

    totalCharacters -= count;
}

void DoublyLinkedList::deleteCharacterFromLine(int lineNumber, int charIndex)
{
    deleteTextFromLine(lineNumber, charIndex, 1);
}

bool DoublyLinkedList::deleteWord(const std::string &targetWord)
{
    int wordLength = calculateLength(targetWord);
    if (wordLength == 0)
        return false;

    ResultArray matches = searchDocument(targetWord);
    if (matches.size() == 0)
    {
        return false;
    }

    SearchResult match = matches.get(0);
    deleteTextFromLine(match.lineNumber, match.startIndex, wordLength);
    return true;
}

int DoublyLinkedList::getLineCount() const
{
    return lineCount;
}

std::string DoublyLinkedList::getLine(int lineNumber) const
{
    if (lineNumber < 1 || lineNumber > lineCount)
    {
        throw std::out_of_range("Line number out of bounds.");
    }
    Node *current = head;
    for (int i = 1; i < lineNumber; ++i)
    {
        current = current->next;
    }
    return current->textData;
}

void DoublyLinkedList::modifyLine(int lineNumber, const std::string &newText)
{
    if (lineNumber < 1 || lineNumber > lineCount)
    {
        throw std::out_of_range("Line number out of bounds.");
    }
    Node *current = head;
    for (int i = 1; i < lineNumber; ++i)
    {
        current = current->next;
    }
    totalCharacters -= calculateLength(current->textData);
    current->textData = newText;
    totalCharacters += calculateLength(newText);
}

bool DoublyLinkedList::isEmpty() const
{
    return head == nullptr;
}
