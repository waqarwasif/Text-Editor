#include "DataStructures/DoublyLinkedList.h"
#include <stdexcept>

DoublyLinkedList::DoublyLinkedList() : head(nullptr), tail(nullptr), lineCount(0) {}

// DoublyLinkedList.cpp: your destructor loop moves into clear()
void DoublyLinkedList::clear() {
    Node* current = head;
    while (current != nullptr) {
        Node* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    head = tail = nullptr;
    lineCount = 0;
}
DoublyLinkedList::~DoublyLinkedList() { clear(); }

void DoublyLinkedList::swap(DoublyLinkedList& other) noexcept {
    std::swap(head, other.head);
    std::swap(tail, other.tail);
    std::swap(lineCount, other.lineCount);
}

void DoublyLinkedList::insertLine(int lineNumber, const std::string &text)
{
    if (lineNumber < 1 || lineNumber > lineCount + 1)
    {
        throw std::out_of_range("Line number out of bounds.");
    }

    Node *newNode = new Node(text);

    // Case 1: Inserting into an empty document
    if (head == nullptr)
    {
        head = tail = newNode;
    }
    // Case 2: Inserting at the beginning
    else if (lineNumber == 1)
    {
        newNode->next = head;
        head->prev = newNode;
        head = newNode;
    }
    // Case 3: Inserting at the end
    else if (lineNumber == lineCount + 1)
    {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
    // Case 4: Inserting in the middle
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
}

void DoublyLinkedList::deleteLine(int lineNumber)
{
    if (lineNumber < 1 || lineNumber > lineCount || head == nullptr)
    {
        throw std::out_of_range("Invalid line number for deletion.");
    }

    Node *nodeToDelete = head;

    // Traverse to the correct line
    for (int i = 1; i < lineNumber; ++i)
    {
        nodeToDelete = nodeToDelete->next;
    }

    // Update pointers to bypass the node
    if (nodeToDelete->prev != nullptr)
    {
        nodeToDelete->prev->next = nodeToDelete->next;
    }
    else
    {
        head = nodeToDelete->next; // Deleting the head
    }

    if (nodeToDelete->next != nullptr)
    {
        nodeToDelete->next->prev = nodeToDelete->prev;
    }
    else
    {
        tail = nodeToDelete->prev; // Deleting the tail
    }

    delete nodeToDelete;
    lineCount--;
}

void DoublyLinkedList::displayDocument() const
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
    current->textData = newText;
}

bool DoublyLinkedList::isEmpty() const
{
    return head == nullptr;
}