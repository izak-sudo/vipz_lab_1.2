#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

// --- CLASS BOOK: Responsible for storing and basic processing of a single book's data ---
class Book {
private:
    std::string mAuthor;
    std::string mTitle;
    int mYear;
    int mPages;
    double mPrice;

public:
    Book() : mYear(0), mPages(0), mPrice(0.0) {}

    Book(std::string author, std::string title, int year, int pages, double price)
        : mAuthor(author), mTitle(title), mYear(year), mPages(pages), mPrice(price) {
    }

    // Getters
    int getYear() const { return mYear; }
    int getPages() const { return mPages; }
    std::string getAuthor() const { return mAuthor; }

    // Read from console with basic validation prompts
    void readFromConsole() {
        std::cout << "  Author: ";
        std::getline(std::cin >> std::ws, mAuthor);
        std::cout << "  Book Title: ";
        std::getline(std::cin >> std::ws, mTitle);
        std::cout << "  Publication Year: ";
        std::cin >> mYear;
        std::cout << "  Number of Pages: ";
        std::cin >> mPages;
        std::cout << "  Price: ";
        std::cin >> mPrice;
    }

    // Deserialization: string from file formatted as "Author|Title|Year|Pages|Price"
    bool parseFromString(const std::string& line) {
        std::stringstream ss(line);
        std::string yearStr, pagesStr, priceStr;

        if (std::getline(ss, mAuthor, '|') &&
            std::getline(ss, mTitle, '|') &&
            std::getline(ss, yearStr, '|') &&
            std::getline(ss, pagesStr, '|') &&
            std::getline(ss, priceStr)) {
            try {
                mYear = std::stoi(yearStr);
                mPages = std::stoi(pagesStr);
                mPrice = std::stod(priceStr);
                return true;
            }
            catch (...) {
                return false; // Number conversion error
            }
        }
        return false; // Incorrect format
    }

    // Serialization for writing to file
    std::string serializeToString() const {
        return mAuthor + "|" + mTitle + "|" + std::to_string(mYear) + "|" +
            std::to_string(mPages) + "|" + std::to_string(mPrice);
    }

    // Formatted console output
    void printRow() const {
        std::cout << std::left << std::setw(20) << mAuthor.substr(0, 19) << " | "
            << std::left << std::setw(20) << mTitle.substr(0, 19) << " | "
            << std::right << std::setw(4) << mYear << " | "
            << std::right << std::setw(8) << mPages << " | "
            << std::right << std::setw(7) << mPrice << "\n";
    }
};

// Singly linked list node
struct Node {
    Book data;
    Node* next;
    Node(const Book& book) : data(book), next(nullptr) {}
};

// --- CLASS BOOKLIST: Collection Management (Linked List) ---
class BookList {
private:
    Node* head;
    Node* tail;

public:
    BookList() : head(nullptr), tail(nullptr) {}

    // Destructor: automatic memory management (RAII)
    ~BookList() {
        clear();
    }

    void clear() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = tail = nullptr;
    }

    // Add a new record to the end of the list
    void addBook(const Book& book) {
        Node* newNode = new Node(book);
        if (!head) {
            head = tail = newNode;
        }
        else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    // Load book list from file
    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "[Error] Failed to open file for reading: " << filename << "\n";
            return false;
        }

        std::string line;
        int lineNum = 1;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            Book tempBook;
            if (tempBook.parseFromString(line)) {
                addBook(tempBook);
            }
            else {
                std::cerr << "[Warning] Corrupted data in file at line " << lineNum << ". Skipping.\n";
            }
            lineNum++;
        }
        file.close();
        return true;
    }

    // Save results to file
    bool saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "[Error] Failed to create/open file for writing: " << filename << "\n";
            return false;
        }

        Node* current = head;
        while (current != nullptr) {
            file << current->data.serializeToString() << "\n";
            current = current->next;
        }
        file.close();
        std::cout << "[Success] Data successfully saved to file: " << filename << "\n";
        return true;
    }

    // Sort list by year (Insertion sort, modifies pointers only)
    void sortByYear() {
        if (!head || !head->next) return;

        Node* sortedList = nullptr;
        Node* current = head;

        while (current != nullptr) {
            Node* nextNode = current->next;

            if (!sortedList || sortedList->data.getYear() >= current->data.getYear()) {
                current->next = sortedList;
                sortedList = current;
            }
            else {
                Node* search = sortedList;
                while (search->next != nullptr && search->next->data.getYear() < current->data.getYear()) {
                    search = search->next;
                }
                current->next = search->next;
                search->next = current;
            }
            current = nextNode;
        }

        // Update head and tail
        head = sortedList;
        tail = head;
        while (tail && tail->next != nullptr) {
            tail = tail->next;
        }
    }

    // Remove books by authors starting with a specific letter
    void removeAuthorsStartingWith(char letter) {
        Node* current = head;
        Node* prev = nullptr;

        while (current != nullptr) {
            std::string author = current->data.getAuthor();
            if (!author.empty() && author[0] == letter) {
                Node* toDelete = current;
                if (prev == nullptr) {
                    head = current->next; // Removing the head
                }
                else {
                    prev->next = current->next;
                }
                current = current->next;
                delete toDelete;
            }
            else {
                prev = current;
                current = current->next;
            }
        }
        // Restore tail
        tail = prev;
    }

    // Find the three books with the fewest pages
    void printTop3SmallestBooks() const {
        if (!head) {
            std::cout << "  The list is empty.\n";
            return;
        }

        const int TOP_COUNT = 3;
        const Book* smallest[TOP_COUNT] = { nullptr, nullptr, nullptr };

        for (Node* curr = head; curr != nullptr; curr = curr->next) {
            const Book* currentBook = &(curr->data);

            // Insert current book into top-3 array if it's smaller than existing ones
            for (int i = 0; i < TOP_COUNT; i++) {
                if (smallest[i] == nullptr || currentBook->getPages() < smallest[i]->getPages()) {
                    // Shift elements to the right
                    for (int j = TOP_COUNT - 1; j > i; j--) {
                        smallest[j] = smallest[j - 1];
                    }
                    smallest[i] = currentBook;
                    break;
                }
            }
        }

        std::cout << "\n--- TOP-3 Books with the Fewest Pages ---\n";
        printHeader();
        for (int i = 0; i < TOP_COUNT; i++) {
            if (smallest[i]) smallest[i]->printRow();
        }
    }

    void printHeader() const {
        std::cout << std::left << std::setw(20) << "Author" << " | "
            << std::left << std::setw(20) << "Book Title" << " | "
            << "Year | Pages    | Price \n";
        std::cout << "---------------------+----------------------+------+----------+--------\n";
    }

    void printTable() const {
        std::cout << "\n";
        printHeader();
        if (!head) {
            std::cout << "  (The list is empty)\n";
            return;
        }
        Node* current = head;
        while (current != nullptr) {
            current->data.printRow();
            current = current->next;
        }
    }
};

// --- MAIN FUNCTION ---
int main() {
    BookList library;
    int choice;

    // Визначення окремих файлів для читання та запису
    const std::string inputFilename = "input_books.txt";
    const std::string outputFilename = "books_data.txt";

    std::cout << "=== Book Management System ===\n";
    std::cout << "Choose data input method:\n";
    std::cout << "1 - Enter data from keyboard\n";
    std::cout << "2 - Read data from file (" << inputFilename << ")\n";
    std::cout << "Your choice: ";
    std::cin >> choice;

    if (choice == 1) {
        int count;
        std::cout << "How many books do you want to enter? ";
        std::cin >> count;
        for (int i = 0; i < count; i++) {
            std::cout << "\n--- Book #" << (i + 1) << " ---\n";
            Book newBook;
            newBook.readFromConsole();
            library.addBook(newBook);
        }
    }
    else if (choice == 2) {
        // Зчитування з input_books.txt
        if (!library.loadFromFile(inputFilename)) {
            std::cout << "Creating an empty list. You can add data manually.\n";
        }
    }
    else {
        std::cerr << "Invalid choice. Exiting.\n";
        return 1;
    }

    std::cout << "\n>>> Initial list:";
    library.printTable();

    std::cout << "\n>>> Variant: Sorting by publication year (ascending):";
    library.sortByYear();
    library.printTable();

    std::cout << "\n>>> Variant: Finding the top 3 books with the fewest pages:";
    library.printTop3SmallestBooks();

    std::cout << "\n>>> Variant: Removing books by authors whose last name starts with 'K':";
    library.removeAuthorsStartingWith('K');
    library.printTable();

    std::cout << "\n>>> Adding a new record:\n";
    Book customBook;
    customBook.readFromConsole();
    library.addBook(customBook);

    // Resort so the new book is placed correctly
    library.sortByYear();
    std::cout << "\n>>> List after adding and resorting:";
    library.printTable();

    // Запис результатів у books_data.txt
    std::cout << "\n>>> Saving results to " << outputFilename << "...\n";
    library.saveToFile(outputFilename);

    return 0; // Memory will be automatically freed in ~BookList() destructor
}
