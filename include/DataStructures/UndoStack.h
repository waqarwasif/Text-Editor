#pragma once

#include "DataStructures/DoublyLinkedList.h"

class UndoStack
{
private:
    struct StackNode
    {
        DoublyLinkedList snapshot;
        StackNode *next;

        StackNode(const DoublyLinkedList &doc, StackNode *nextNode = nullptr)
            : snapshot(doc), next(nextNode) {}
    };

    StackNode *topNode;
    int count;
    int maxCapacity;

    void dropOldest();

public:
    explicit UndoStack(int capacity = 20);
    ~UndoStack();

    UndoStack(const UndoStack &) = delete;
    UndoStack &operator=(const UndoStack &) = delete;

    void push(const DoublyLinkedList &doc);
    bool pop(DoublyLinkedList &outDoc);
    const DoublyLinkedList &peek() const;

    bool isEmpty() const;
    int size() const;
    int capacity() const;
    void clear();
};
