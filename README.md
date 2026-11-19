<div align="center">

# 📝 In-Memory C++ Text Editor
### CS-218 Data Structures & Algorithms — Complex Engineering Problem (CEP)
**Department of Computer & Information Systems Engineering**  
*NED University of Engineering & Technology*

<!-- Animated Typing Header -->
<a href="https://github.com/waqarwasif/Text-Editor">
  <img src="https://readme-typing-svg.demolab.com?font=Fira+Code&weight=600&size=22&pause=1000&color=0055A5&center=true&vCenter=true&width=650&lines=CS-218+Complex+Engineering+Problem;High-Performance+In-Memory+Text+Editor;Zero+STL+Container+Dependencies;Custom+Doubly+Linked+List+%2B+Memento+Undo" alt="Typing Banner" />
</a>

<p align="center">
  <img src="https://img.shields.io/badge/Language-C%2B%2B17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++17" />
  <img src="https://img.shields.io/badge/Build_System-CMake_%E2%89%A5_3.16-064F8C?style=for-the-badge&logo=cmake&logoColor=white" alt="CMake" />
  <img src="https://img.shields.io/badge/Design_Patterns-Facade_%26_Memento-2ea44f?style=for-the-badge" alt="Design Patterns" />
  <img src="https://img.shields.io/badge/Tests-7%20Passed%20(100%25)-success?style=for-the-badge&logo=checkmarx&logoColor=white" alt="Tests" />
  <img src="https://img.shields.io/badge/Dependencies-Zero_STL_Containers-orange?style=for-the-badge" alt="Dependencies" />
</p>

---

[Key Features](#-key-features) •
[Architecture & Design Patterns](#-architecture--design-patterns) •
[Custom Data Structures](#-custom-data-structures-deep-dive) •
[Complexity Analysis](#-time--space-complexity-analysis) •
[Build & Run](#-quick-start--build-guide) •
[Test Verification](#-automated-testing-suite) •
[Contributors](#-development-team)

---

</div>

## 📌 Project Overview

This project is a high-performance, robust, console-based **in-memory text editor** implemented from scratch in **C++17**. Developed for the **CS-218 Data Structures and Algorithms** Complex Engineering Problem (CEP), the editor prioritizes core algorithmic efficiency, manual dynamic memory management, and clean object-oriented architecture over third-party abstractions.

### 🌟 Core Highlights
- **Zero Standard Container Dependencies:** Operates without `std::list`, `std::vector`, `std::stack`, or `std::string` — all data structures are purpose-built by hand.
- **Bi-Directional Fast Traversal:** Accesses document lines from either `head` or `tail` based on target index, cutting sequential search overhead in half.
- **Zero-Copy Undo History:** Implements the **GoF Memento Pattern** coupled with an $O(1)$ raw pointer-swap mechanism for instant state restoration without memory re-allocation.
- **Fail-Safe Robustness:** Resilient input recovery, file boundary validations, and protection against crashes on empty documents or invalid numeric prompts.

---

## ⚡ Key Features

The editor provides a fully interactive terminal interface featuring **20 essential operations**:

| Category | Operations | Description |
| :--- | :--- | :--- |
| **📁 File Operations** | `1` to `4`, `0` | **Create New Document**, **Open File**, **Save Document**, **Save As...**, and **Safe Exit** with unsaved change warnings. Supports cross-platform CRLF/LF line parsing. |
| **📄 Display & Stats** | `5` to `8` | **Display Document**, **Display Line-by-Line** (numbered), **Document Statistics** (total lines & characters), and **View Specific Line**. |
| **✏️ Insertion & Edit** | `9` to `11` | **Insert Line** at any line $1 \dots N+1$, **Insert Text at Column** (start, middle, or end of line), and **Modify Line Content**. |
| **🗑️ Deletion Tools** | `12` to `16` | **Delete Line**, **Delete Range of Lines**, **Delete Character**, **Delete Text Range**, and **Delete Specific Word** (first occurrence). |
| **🔍 Search & Replace** | `17` to `19` | **Search Document** (returns match count and exact `[Line, Column]` coordinates), **Replace Specific Occurrence** (globally or within a specific line), and **Replace All Occurrences**. |
| **⏪ State Rollback** | `20` | **Undo Last Action** (bounded 20-step LIFO history) restoring previous document snapshots instantaneously. |

---

## 🏗️ Architecture & Design Patterns

The software is structured across three decoupled layers to uphold the **Separation of Concerns (SoC)** and the **Law of Demeter**:

```
┌────────────────────────────────────────────────────────────────────────┐
│                   Presentation Layer (Menu.cpp / UI)                   │
│         Console loop, stream recovery, user prompts & formatting       │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │ facade dispatch
                                    ▼
┌────────────────────────────────────────────────────────────────────────┐
│                 Controller Facade Layer (TextEditor.h)                 │
│      Parameter validation • State snapshotting • Dirty state flags     │
└──────────────────┬─────────────────────────────────┬───────────────────┘
                   │                                 │
                   ▼                                 ▼
┌────────────────────────────────────┐  ┌────────────────────────────────┐
│   DoublyLinkedList (Originator)    │  │     UndoStack (Caretaker)      │
│  Line nodes, text editing, swap()  │  │  LIFO snapshots, bounded cap  │
└────────────────────────────────────┘  └────────────────────────────────┘
```

### 1. The Facade Pattern (`TextEditor`)
- Acts as a unified entry point over document manipulation (`DoublyLinkedList`), file persistence (`FileOperations`), and history preservation (`UndoStack`).
- User actions do not manipulate raw node pointers directly; the facade validates inputs, triggers snapshots, mutates structures, and flags dirty states atomically.

### 2. The Memento Pattern (`DoublyLinkedList` + `UndoStack`)
- **Originator (`DoublyLinkedList`):** Generates pristine deep-copy snapshots of document state and restores them via `swap()`.
- **Memento (`StackNode`):** Encapsulates the immutable snapshot containing head, tail, and line counts.
- **Caretaker (`UndoStack`):** Manages historical frames in LIFO order with a fixed capacity limit (`maxCapacity = 20`) via `dropOldest()`.

---

## 🧩 Custom Data Structures Deep-Dive

In strict compliance with academic standards, all core data structures are built without standard library containers:

### 1. `DoublyLinkedList` (Document Core)
- **Node Anatomy:** Each node contains a `mystring textData`, `Node* prev`, and `Node* next`.
- **Bi-Directional Optimization:** When indexing line $K$ out of $N$ lines:
  - If $K \le N / 2$: Traverses forward from `head`.
  - If $K > N / 2$: Traverses backward from `tail`.
  - Cuts maximum lookup hops from $N$ to $N/2$.

### 2. `UndoStack` (Historical Caretaker)
- Linked-stack implementation maintaining dynamic `StackNode` elements.
- **Zero-Copy Restore:** Uses `outDoc.swap(temp->snapshot)` to swap memory pointers in **$O(1)$ time**, bypassing expensive node allocations during undos.

### 3. `ResultArray` (Dynamic Buffer)
- Custom dynamic array storing `SearchResult { int lineNumber; int startIndex; }`.
- Automatically doubles buffer capacity when full ($O(1)$ amortized append).

### 4. `MyString` (Zero-Dependency String Class)
- Custom RAII string managing dynamic character buffers (`char*`) with full **Rule of Five** semantics:
  - Deep-copy constructor and copy-assignment.
  - Efficient move constructor and move-assignment (`noexcept`).
  - Safe operations: `find()`, `substr()`, `insert()`, `erase()`, `resize()`, `getline()`.

---

## ⏱️ Time & Space Complexity Analysis

Formal algorithmic evaluation under Course Learning Outcome 2 (**CLO-2**).  
*Let $L$ = total lines, $C$ = average characters per line, $S$ = undo stack capacity ($S \le 20$).*

| Operation | Function | Time Complexity | Space Complexity | Complexity Class |
| :--- | :--- | :---: | :---: | :---: |
| **Line Insert (Head/Tail)** | `insertLine(1 or L+1)` | $\mathcal{O}(1)$ | $\mathcal{O}(C)$ | Constant Time |
| **Line Insert (Arbitrary)** | `insertLine(K)` | $\mathcal{O}(L)$ $[\le L/2]$ | $\mathcal{O}(C)$ | Linear Traversal |
| **Line Delete (Head/Tail)** | `deleteLine(1 or L)` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | Constant Time |
| **Line Delete (Arbitrary)** | `deleteLine(K)` | $\mathcal{O}(L)$ $[\le L/2]$ | $\mathcal{O}(1)$ | Linear Traversal |
| **Range Line Deletion** | `deleteRangeOfLines(A, B)` | $\mathcal{O}(L + (B - A))$ | $\mathcal{O}(1)$ | Linear Traversal |
| **Character / Range Erase** | `deleteTextFromLine()` | $\mathcal{O}(L + C)$ | $\mathcal{O}(1)$ | Linear Time |
| **Document Search** | `searchDocument()` | $\mathcal{O}(L \cdot C)$ | $\mathcal{O}(M)$ | Linear Search |
| **Replace Occurrence** | `replaceOccurrence()` | $\mathcal{O}(L \cdot C)$ | $\mathcal{O}(C)$ | Linear Search |
| **Replace All** | `replaceAll()` | $\mathcal{O}(L \cdot C)$ | $\mathcal{O}(L \cdot C)$ | Full Document Pass |
| **Undo Snapshot (Push)** | `UndoStack::push()` | $\mathcal{O}(L \cdot C)$ | $\mathcal{O}(L \cdot C)$ | Linear Deep Copy |
| **Undo Restore (Pop)** | `UndoStack::pop()` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | **Zero-Copy Swap** |
| **Oldest Eviction** | `UndoStack::dropOldest()` | $\mathcal{O}(S)$ $[S \le 20]$ | $\mathcal{O}(1)$ | Bounded Overhead |

---

## 🚀 Quick Start & Build Guide

### Prerequisites
- **Compiler:** Modern C++17 compatible compiler (GCC / MinGW-w64, Clang, or MSVC).
- **Build System:** CMake version $\ge 3.16$.

### Option 1: Standard CMake Build (Cross-Platform)

```bash
# 1. Clone the repository
git clone https://github.com/waqarwasif/Text-Editor.git
cd Text-Editor/Text-Editor

# 2. Configure build directory
cmake -B build -S .

# 3. Build executable and test suites
cmake --build build -j 4

# 4. Launch the application
./build/TextEditor          # On Linux/macOS
.\build\TextEditor.exe      # On Windows
```

To run all automated tests through CTest:
```bash
ctest --test-dir build --output-on-failure
```

---

### Option 2: Windows PowerShell Automation Script

A self-contained build automation script `build.ps1` is included in the project root:

```powershell
# Build everything:
.\build.ps1

# Build and execute all 7 test suites:
.\build.ps1 -RunTests

# Build and immediately launch the interactive editor:
.\build.ps1 -RunApp
```

---

## 🧪 Automated Testing Suite

The project includes **7 specialized unit test suites** verifying every subsystem:

<details>
<summary><b>🔍 Click to view Test Suite breakdown</b></summary>

| Test Executable | Target Scope | Verified Scenarios |
| :--- | :--- | :--- |
| `test_linked_list` | `DoublyLinkedList` | Head/tail inserts, bi-directional walks, modify line, character accounting, `clear()`. |
| `test_text_editor` | `TextEditor` Facade | Column inserts (start/mid/end), line inserts, rejection of bad coordinates, dirty flag tracking. |
| `test_text_processing` | `TextProcessing` | Substring replacement, bounds clamping, character validation, buffer preservation. |
| `test_file_io` | `FileOperations` | New document creation, opening existing files, CRLF/LF line-break stripping, path validation. |
| `test_search_replace` | `ResultArray` & Search | Dynamic array expansion, exact pattern matching, coordinate accuracy, overlap detection. |
| `test_replace` | Multi-Pass Replace | Single occurrence replacement, line-scoped replace, global document replace-all. |
| `test_undo_stack` | `UndoStack` & Memento | LIFO pop ordering, capacity eviction (`dropOldest`), rollback across all 10 edit operations. |

</details>

```text
================== AUTOMATED TEST RUNNER RESULTS ==================
==> Running test_linked_list      [PASS] - All linked list checks passed.
==> Running test_text_editor      [PASS] - All TextEditor checks passed.
==> Running test_text_processing  [PASS] - All TextProcessing tests passed.
==> Running test_file_io          [PASS] - All file I/O checks passed.
==> Running test_search_replace   [PASS] - All search & deletion tests passed.
==> Running test_replace          [PASS] - All replace checks passed.
==> Running test_undo_stack       [PASS] - All UndoStack tests passed.
===================================================================
STATUS: ALL 7 TEST SUITES COMPLETED WITH 0 FAILURES (100% PASS)
```

---

## 👥 Development Team

**EZ Gang — SE Batch 2025 (Fall Semester 2026)**  
*Department of Computer and Information Systems Engineering, NED University*

- **Syed Waqar Wasif** — Roll No: `CS-25123`
- **Asma Khalid** — Roll No: `CS-25108`

---

<div align="center">
  <sub>Developed for CS-218 Data Structures and Algorithms • Academic Year 2026</sub>
</div>
