#include "DataStructures/DoublyLinkedList.h"
#include "Core/FileOperations.h"
#include "Utils/Menu.h"

int main()
{
    DoublyLinkedList doc;
    FileOperations fileOps;

    menu(doc, fileOps);

    return 0;
}
