#pragma once

#include "Core/FileOperations.h"
#include "Core/TextEditor.h"
#include "DataStructures/DoublyLinkedList.h"

void displayMenu(const FileOperations &fileOps, int undoCount = 0);

void handleCreateNew(TextEditor &editor);
void handleCreateNew(DoublyLinkedList &doc, FileOperations &fileOps);

void handleOpen(TextEditor &editor);
void handleOpen(DoublyLinkedList &doc, FileOperations &fileOps);

void handleSave(DoublyLinkedList &doc, FileOperations &fileOps);
void handleSaveAs(DoublyLinkedList &doc, FileOperations &fileOps);
void handleDisplayDocument(const DoublyLinkedList &doc);
void handleDisplayLineByLine(const DoublyLinkedList &doc);
void handleDisplayStatistics(const DoublyLinkedList &doc);
void handleGetLine(const DoublyLinkedList &doc);

void handleInsertLine(TextEditor &editor);
void handleInsertText(TextEditor &editor);

void handleModifyLine(TextEditor &editor);
void handleModifyLine(DoublyLinkedList &doc, FileOperations &fileOps);

void handleDeleteLine(TextEditor &editor);
void handleDeleteLine(DoublyLinkedList &doc, FileOperations &fileOps);

void handleDeleteRangeOfLines(TextEditor &editor);
void handleDeleteRangeOfLines(DoublyLinkedList &doc, FileOperations &fileOps);

void handleDeleteCharacter(TextEditor &editor);
void handleDeleteCharacter(DoublyLinkedList &doc, FileOperations &fileOps);

void handleDeleteTextRange(TextEditor &editor);
void handleDeleteTextRange(DoublyLinkedList &doc, FileOperations &fileOps);

void handleDeleteWord(TextEditor &editor);
void handleDeleteWord(DoublyLinkedList &doc, FileOperations &fileOps);

void handleSearch(const DoublyLinkedList &doc);
void handleReplaceOccurrence(TextEditor &editor);
void handleReplaceAll(TextEditor &editor);

void handleUndo(TextEditor &editor);

void menu(TextEditor &editor);
