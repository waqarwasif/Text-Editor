#pragma once
#include <string>
#include <iostream>

// Represents a single line of text in the document
struct Node
{
    std::string textData;
    Node *prev;
    Node *next;

    Node(const std::string &data) : textData(data), prev(nullptr), next(nullptr){}
};

class DoublyLinkedList
{
private:
    Node *head;
    Node *tail;
    int lineCount;
    int totalCharacters;

public:
    DoublyLinkedList();
    ~DoublyLinkedList();

    // Core operations for Text Insertion and Deletion (Feature 3)[cite: 13]
    void insertLine(int lineNumber, const std::string &text);
    void deleteLine(int lineNumber);
    void deleteRangeOfLines(int startLine, int endLine);
    void displayDocumentLineByLine() const;
    void displayDocument() const;
    void displayDocumentInfo() const;
    // Utility operations
    int getLineCount() const;
    std::string getLine(int lineNumber) const;
    void modifyLine(int lineNumber, const std::string &newText);
    bool isEmpty() const;
    int calculateLength(const std::string &str) const;


    // File Operation Helpers added by Asma (Feature 1)[cite: 12]
    const Node *getHead() const { return head; }
    void clear();
    void swap(DoublyLinkedList &other) noexcept;

    // Memory safety: prevents accidental list duplication that causes crashes (Feature 7)[cite: 13]
    DoublyLinkedList(const DoublyLinkedList &) = delete;
    DoublyLinkedList &operator=(const DoublyLinkedList &) = delete;
};