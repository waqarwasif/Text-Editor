#include "DataStructures/UndoStack.h"
#include <stdexcept>

UndoStack::UndoStack(int capacity)
    : topNode(nullptr), count(0)
{
    if (capacity > 0)
    {
        maxCapacity = capacity;
    }
    else
    {
        maxCapacity = 20;
    }
}

UndoStack::~UndoStack()
{
    clear();
}

void UndoStack::dropOldest()
{
    if (topNode == nullptr)
        return;

    if (topNode->next == nullptr)
    {
        delete topNode;
        topNode = nullptr;
        count = 0;
        return;
    }

    StackNode *curr = topNode;
    while (curr->next->next != nullptr)
    {
        curr = curr->next;
    }

    delete curr->next;
    curr->next = nullptr;
    count--;
}

void UndoStack::push(const DoublyLinkedList &doc)
{
    if (count >= maxCapacity)
    {
        dropOldest();
    }

    StackNode *newNode = new StackNode(doc, topNode);
    topNode = newNode;
    count++;
}

bool UndoStack::pop(DoublyLinkedList &outDoc)
{
    if (isEmpty())
        return false;

    StackNode *temp = topNode;
    outDoc.swap(temp->snapshot);
    topNode = topNode->next;

    delete temp;
    count--;
    return true;
}

const DoublyLinkedList &UndoStack::peek() const
{
    if (isEmpty())
        throw std::out_of_range("Undo stack is empty.");
    return topNode->snapshot;
}

bool UndoStack::isEmpty() const
{
    return topNode == nullptr;
}

int UndoStack::size() const
{
    return count;
}

int UndoStack::capacity() const
{
    return maxCapacity;
}

void UndoStack::clear()
{
    while (topNode != nullptr)
    {
        StackNode *temp = topNode;
        topNode = topNode->next;
        delete temp;
    }
    count = 0;
}