#pragma once

#include <iostream>
#include "DataStructures/MyString.h"
#include "DataStructures/Array.h"

struct Node
{
    mystring textData;
    Node *prev;
    Node *next;

    Node(const mystring &data) : textData(data), prev(nullptr), next(nullptr) {}
};

class DoublyLinkedList
{
private:
    Node *head;
    Node *tail;
    int lineCount;
    int totalCharacters;

    Node *nodeAt(int lineNumber) const;

public:
    DoublyLinkedList();
    ~DoublyLinkedList();

    void insertLine(int lineNumber, const mystring &text);
    void deleteLine(int lineNumber);
    void deleteRangeOfLines(int startLine, int endLine);
    void deleteTextFromLine(int lineNumber, int startIndex, int count);
    void deleteCharacterFromLine(int lineNumber, int charIndex);
    bool deleteWord(const mystring &targetWord);
    void displayDocumentLineByLine() const;
    void displayDocument() const;
    void displayDocumentInfo() const;
    ResultArray searchDocument(const mystring &target) const;

    int getLineCount() const;
    const mystring &getLine(int lineNumber) const;
    void modifyLine(int lineNumber, mystring newText);
    bool isEmpty() const;
    int calculateLength(const mystring &str) const;

    const Node *getHead() const { return head; }
    void clear();
    void swap(DoublyLinkedList &other) noexcept;

    DoublyLinkedList(const DoublyLinkedList &) = delete;
    DoublyLinkedList &operator=(const DoublyLinkedList &) = delete;
};
