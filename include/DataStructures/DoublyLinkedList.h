#pragma once
#include <string>
#include <iostream>

// Represents a single line of text in the document
struct Node
{
    std::string textData;
    Node *prev;
    Node *next;

    Node(const std::string &data) : textData(data), prev(nullptr), next(nullptr) {}
};

class DoublyLinkedList
{
private:
    Node *head;
    Node *tail;
    int lineCount;

public:
    DoublyLinkedList();
    ~DoublyLinkedList();

    void insertLine(int lineNumber, const std::string &text);
    void deleteLine(int lineNumber);
    void displayDocument() const;

    int getLineCount() const;
    std::string getLine(int lineNumber) const;
    void modifyLine(int lineNumber, const std::string &newText);
    bool isEmpty() const;

    const Node *getHead() const { return head; }
    void clear();
    void swap(DoublyLinkedList &other) noexcept;
    DoublyLinkedList(const DoublyLinkedList &) = delete;
    DoublyLinkedList &operator=(const DoublyLinkedList &) = delete;
};
