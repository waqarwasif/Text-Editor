#pragma once
#include <string>
#include <iostream>
#include "Array.h"

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

    // Node at 1-based lineNumber, walking from whichever end is closer.
    // Precondition: 1 <= lineNumber <= lineCount (callers validate).
    Node *nodeAt(int lineNumber) const;

public:

public:
    DoublyLinkedList();
    ~DoublyLinkedList();

    // Core operations for Text Insertion and Deletion (Feature 3)[cite: 13]
    void insertLine(int lineNumber, const std::string &text);
    void deleteLine(int lineNumber);
    void deleteRangeOfLines(int startLine, int endLine);
    void deleteTextFromLine(int lineNumber, int startIndex, int count);
    void deleteCharacterFromLine(int lineNumber, int charIndex);
    bool deleteWord(const std::string &targetWord);
    void displayDocumentLineByLine() const;
    void displayDocument() const;
    void displayDocumentInfo() const;
    ResultArray searchDocument(const std::string &target) const;



    // Utility operations
    int getLineCount() const;
    const std::string &getLine(int lineNumber) const; // valid until that line is modified/deleted
    void modifyLine(int lineNumber, std::string newText); // by value: caller may std::move in
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