#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>

struct Book {
    char mAuthor[20];
    char mNameBook[20];
    int mYear;
    int mPages;
    int mPrice;
};

struct Node {
    Book mpInfo;
    Node* mpNext;
};

void sortYear(Node** ppHead, int count) {
    if (*ppHead == nullptr) { return; }
    if (count < 2) { return; }

    for (int i = 0; i < count - 1; i++) {
        Node* pTemp = *(ppHead);
        Node* pPrevNode = nullptr;
        Node* pNextNode = nullptr;

        while (pTemp->mpNext != nullptr) {
            pNextNode = pTemp->mpNext;

            if ((pTemp->mpInfo.mYear) > (pTemp->mpNext->mpInfo.mYear)) {
                pTemp->mpNext = pNextNode->mpNext;
                pNextNode->mpNext = pTemp;

                if (pPrevNode == nullptr) { *ppHead = pNextNode; }
                else { pPrevNode->mpNext = pNextNode; }

                pPrevNode = pNextNode;
            }
            else {
                pPrevNode = pTemp;
                pTemp = pTemp->mpNext;
            }
        }
    }
}

void deleteNode(Node** ppHead, int count) {
    Node* pTemp = *ppHead;
    Node* pPrevNode = nullptr;
    while (pTemp != nullptr) {

        if ((pTemp->mpInfo.mAuthor[0]) == 'K') {
            Node* pNodeToDelete = pTemp;

            if (pPrevNode == nullptr) {
                *ppHead = pTemp->mpNext;
                pTemp = *ppHead;
            }
            else {
                pPrevNode->mpNext = pTemp->mpNext;
                pTemp = pTemp->mpNext;
            }

            delete pNodeToDelete;
        }
        else {
            pPrevNode = pTemp;
            pTemp = pTemp->mpNext;
        }
    }
}

void ListAddElem(Node** ppHead, Node* pElem) {

    if (*ppHead == nullptr) {
        pElem->mpNext = *ppHead;
        *ppHead = pElem;
        return;
    }
    if (pElem->mpInfo.mYear < (*ppHead)->mpInfo.mYear) {
        pElem->mpNext = *ppHead;
        *ppHead = pElem;
        return;
    }

    Node* pTemp = *ppHead;
    while (pTemp->mpNext != nullptr && pTemp->mpNext->mpInfo.mYear < pElem->mpInfo.mYear) {
        pTemp = pTemp->mpNext;
    }
    pElem->mpNext = pTemp->mpNext;
    pTemp->mpNext = pElem;

}

void freeList(Node** ppHead) {
    Node* pTemp = *ppHead;
    Node* pNext;

    while (pTemp != nullptr) {
        pNext = pTemp->mpNext;
        delete pTemp;
        pTemp = pNext;
    }
    *ppHead = nullptr;
}

void printList(Node* pHead) {
    std::cout << "\n";
    std::cout << "Author \t\t Name of book \t\t Year \t Pages \t Price\n";
    std::cout << "-----------------------------------------------------\n";
    while (pHead != nullptr) {
        std::cout << std::left << std::setw(15) << pHead->mpInfo.mAuthor << " "
            << std::left << std::setw(15) << pHead->mpInfo.mNameBook << " \t "
            << pHead->mpInfo.mYear << " \t "
            << pHead->mpInfo.mPages << " \t "
            << pHead->mpInfo.mPrice << "\n";

        pHead = pHead->mpNext;
    }
}

int main() {
    std::ifstream file("file.txt");
    if (!file.is_open())
    {
        std::cerr << "File not opened." << std::endl;
        return 1;
    }

    Node* pHead = nullptr;
    Node* pLast = nullptr;
    int count = 0;

    Book tempBook;
    while (file >> tempBook.mAuthor >> tempBook.mNameBook >> tempBook.mYear >> tempBook.mPages >> tempBook.mPrice) {

        Node* pNode = new Node;
        if (!pNode) { return 1; }

        pNode->mpInfo = tempBook;
        pNode->mpNext = nullptr;

        if (pHead == nullptr) {
            pHead = pNode;
            pLast = pNode;
        }
        else {
            pLast->mpNext = pNode;
            pLast = pNode;
        }
        count++;

        Node* pNodeLast;
        pNodeLast = pNode;
        pNodeLast->mpNext = nullptr;
    }

    file.close();

    std::cout << "After Sorting";
    sortYear(&pHead, count);
    printList(pHead);

    std::cout << "\nAfter Deleting";
    deleteNode(&pHead, count);
    printList(pHead);

    Node* pElem = new Node;
    if (pElem == nullptr) {
        std::cout << "There is no memory";
        return 0;
    }
    std::strcpy((pElem->mpInfo.mAuthor), "Kostenko");
    std::strcpy((pElem->mpInfo.mNameBook), "Marysya");
    pElem->mpInfo.mYear = 1961;
    pElem->mpInfo.mPages = 180;
    pElem->mpInfo.mPrice = 220;
    pElem->mpNext = nullptr;

    std::cout << "\nAfter Adding";
    ListAddElem(&pHead, pElem);
    printList(pHead);

    freeList(&pHead);

    return 0;
}
