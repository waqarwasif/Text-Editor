#pragma once

#include "DataStructures/DoublyLinkedList.h"
#include "Core/FileOperations.h"
#include "Core/TextEditor.h"

void displayMenu(const FileOperations &fileOps);

void handleCreateNew(DoublyLinkedList &doc, FileOperations &fileOps);
void handleOpen(DoublyLinkedList &doc, FileOperations &fileOps);
void handleSave(DoublyLinkedList &doc, FileOperations &fileOps);
void handleSaveAs(DoublyLinkedList &doc, FileOperations &fileOps);
void handleDisplayDocument(const DoublyLinkedList &doc);
void handleDisplayLineByLine(const DoublyLinkedList &doc);
void handleDisplayStatistics(const DoublyLinkedList &doc);
void handleGetLine(const DoublyLinkedList &doc);
void handleInsertLine(TextEditor &editor);
void handleInsertText(TextEditor &editor);
void handleModifyLine(DoublyLinkedList &doc, FileOperations &fileOps);
void handleDeleteLine(DoublyLinkedList &doc, FileOperations &fileOps);
void handleDeleteRangeOfLines(DoublyLinkedList &doc, FileOperations &fileOps);
void handleDeleteCharacter(DoublyLinkedList &doc, FileOperations &fileOps);
void handleDeleteTextRange(DoublyLinkedList &doc, FileOperations &fileOps);
void handleDeleteWord(DoublyLinkedList &doc, FileOperations &fileOps);
void handleSearch(const DoublyLinkedList &doc);

void menu(TextEditor &editor);
