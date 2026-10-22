import os

def create_structure():
    # Define directories to organize the program into appropriate classes, functions, and modules[cite: 16]
    directories = [
        'include/DataStructures',
        'include/Core',
        'include/Utils',
        'src/DataStructures',
        'src/Core',
        'src/Utils',
        'tests',
        'data',
        'docs'
    ]
    
    # Define files required for the core DSA requirements and basic features[cite: 14, 15]
    files = [
        '.gitignore',
        'CMakeLists.txt',
        'README.md',
        'include/DataStructures/DoublyLinkedList.h',
        'include/DataStructures/UndoStack.h',
        'include/Core/TextEditor.h',
        'include/Core/FileOperations.h',
        'include/Core/ActionState.h',
        'include/Utils/TextProcessing.h',
        'include/Utils/ErrorHandler.h',
        'src/main.cpp',
        'src/DataStructures/DoublyLinkedList.cpp',
        'src/DataStructures/UndoStack.cpp',
        'src/Core/TextEditor.cpp',
        'src/Core/FileOperations.cpp',
        'src/Utils/TextProcessing.cpp',
        'src/Utils/ErrorHandler.cpp',
        'tests/test_linked_list.cpp',
        'tests/test_undo_stack.cpp',
        'tests/test_file_io.cpp',
        'tests/test_search_replace.cpp',
        'data/sample_doc.txt',
        'data/empty_doc.txt',
        'docs/Project_Report.pdf',
        'docs/Complexity_Analysis.md'
    ]

    print("--- Initializing Text Editor CEP Project Structure ---")

    # 1. Create directories (ignores if already present to prevent errors)
    for dir_path in directories:
        os.makedirs(dir_path, exist_ok=True)
        print(f"Directory checked/created: {dir_path}")

    # 2. Create files ONLY if they do not already exist (prevents overwriting source code)
    print("\n--- Generating Files ---")
    for file_path in files:
        if not os.path.exists(file_path):
            with open(file_path, 'w') as f:
                pass # Creates an empty file
            print(f"[CREATED] {file_path}")
        else:
            # Ignores the file if it is already present in the workspace
            print(f"[IGNORED - ALREADY EXISTS] {file_path}")

    print("\nProject structure generation complete. Safe to begin compilation setup.")

if __name__ == "__main__":
    create_structure()