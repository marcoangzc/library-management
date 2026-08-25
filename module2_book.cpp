#include "common.h"

// Module 2 - Book Catalogue & Inventory Management - chong wen kai
//
// The catalogue is a vector<Book>. Copies that are out on loan are tracked by
// keeping totalCopies and availableCopies apart: copies on loan is always
// totalCopies - availableCopies. Shelf positions live in the shared 2D array
// shelfLayout[][] (row = shelf, column = slot, value = bookID or EMPTY).

// ---- function prototypes ----
void   addBook(vector<Book>& books);
void   searchBook(const vector<Book>& books);
void   searchBookByID(const vector<Book>& books);
void   searchBookByTitle(const vector<Book>& books);
void   searchBookByAuthor(const vector<Book>& books);
void   searchBookByISBN(const vector<Book>& books);
void   updateBook(vector<Book>& books);
void   manageCopies(vector<Book>& books);
void   addCopies(vector<Book>& books, int index, int count);
bool   removeCopies(vector<Book>& books, int index, int count);
void   assignShelfLocation(vector<Book>& books);
void   removeBook(vector<Book>& books);
void   displayCatalogue(const vector<Book>& books);
void   displayAvailability(const vector<Book>& books);
void   displayShelfMap();

int    findDuplicateISBN(const vector<Book>& books, const string& isbn, int skipIndex);
int    countByCategory(const vector<Book>& books, int category);
string locationText(Book b);        // pass-by-value: the function only reads
void   printSearchHeader();

void bookCatalogueMenu(vector<Book>& books) {
    int choice;

    do {
        printHeader("BOOK CATALOGUE & INVENTORY");
        cout << "  1. Add New Book Title\n";
        cout << "  2. Search Book\n";
        cout << "  3. Update Book Details\n";
        cout << "  4. Add / Remove Copies\n";
        cout << "  5. Assign Shelf Location\n";
        cout << "  6. Remove Title\n";
        cout << "  7. Display Catalogue\n";
        cout << "  8. Display Availability Listing\n";
        cout << "  0. Back to Main Menu\n\n";

        choice = readInt("Enter choice: ", 0, 8);

        switch (choice) {
            case 1:
                addBook(books);
                pressEnterToContinue();
                break;
            case 2:
                searchBook(books);
                pressEnterToContinue();
                break;
            case 3:
                updateBook(books);
                pressEnterToContinue();
                break;
            case 4:
                manageCopies(books);
                pressEnterToContinue();
                break;
            case 5:
                assignShelfLocation(books);
                pressEnterToContinue();
                break;
            case 6:
                removeBook(books);
                pressEnterToContinue();
                break;
            case 7:
                displayCatalogue(books);
                pressEnterToContinue();
                break;
            case 8:
                displayAvailability(books);
                pressEnterToContinue();
                break;
            case 0:
                break;
        }
    } while (choice != 0);
}

// ---- helpers ----

// Returns the index of another ACTIVE title with the same ISBN, or -1.
// skipIndex lets updateBook() ignore the book being edited itself.
int findDuplicateISBN(const vector<Book>& books, const string& isbn, int skipIndex) {
    for (int i = 0; i < (int)books.size(); i++) {
        if (i == skipIndex || !books[i].active) continue;
        if (books[i].isbn == isbn) return i;
    }
    return -1;
}

// Formats the physical location as "S2-P3" (shelf 2, position 3).
// Taken BY VALUE on purpose: the function only reads the record.
string locationText(Book b) {
    if (b.shelf >= 1 && b.position >= 1)
        return "S" + to_string(b.shelf) + "-P" + to_string(b.position);
    return "-";
}

// Column headings shared by all search results and the catalogue listing.
void printSearchHeader() {
    cout << "\n" << left
         << setw(6)  << "ID"
         << setw(34) << "Title"
         << setw(20) << "Author"
         << setw(13) << "Category"
         << right
         << setw(5)  << "Tot"
         << setw(5)  << "Avl"
         << "  " << left << setw(9) << "Location" << '\n';
    printRule();
}

// Prints one aligned catalogue row. Shared by the searches and the listing.
void displayBookRow(Book b) {
    string category = (b.category >= 0 && b.category < MAX_CATEGORIES)
                    ? CATEGORY_NAME[b.category] : "Unknown";

    cout << left
         << setw(6)  << b.bookID
         << setw(34) << b.title.substr(0, 33)
         << setw(20) << b.author.substr(0, 19)
         << setw(13) << category.substr(0, 12)
         << right
         << setw(5)  << b.totalCopies
         << setw(5)  << b.availableCopies
         << "  " << left << setw(9) << locationText(b)
         << '\n';
}

// Menu option 1 - add a new title to the catalogue.
void addBook(vector<Book>& books) {
    printHeader("ADD NEW BOOK TITLE");

    // Capacity guard: MAX_BOOKS sizes reserveQueue and borrowCount, whose rows
    // are indexed by catalogue position, so the vector must not grow past it.
    if ((int)books.size() >= MAX_BOOKS) {
        cout << "\n  >> Catalogue is full (" << MAX_BOOKS << " titles).\n";
        return;
    }

    Book b;
    b.bookID = nextBookID(books);      // IDs are assigned by the system
    cout << "  New Book ID : " << b.bookID << " (assigned automatically)\n\n";

    b.title  = readNonEmptyLine("  Title: ", 60);
    b.author = readNonEmptyLine("  Author: ", 40);

    // while loop re-prompts until the ISBN is exactly 13 digits ...
    while (true) {
        b.isbn = readLine("  ISBN (13 digits): ");
        if (!isAllDigits(b.isbn) || (int)b.isbn.length() != ISBN_LEN) {
            cout << "  >> Invalid ISBN. Enter exactly " << ISBN_LEN << " digits.\n";
            continue;
        }
        // ... and does not already belong to another title.
        if (findDuplicateISBN(books, b.isbn, -1) != -1) {
            cout << "  >> Duplicate ISBN. This ISBN already exists in the catalogue.\n";
            continue;
        }
        break;
    }

    int category = readInt("  Category (1-6): ", 1, MAX_CATEGORIES);
    b.category = category - 1;         // stored as 0..5

    int copies = readInt("  Total copies (1-99): ", 1, 99);

    b.totalCopies     = copies;
    b.availableCopies = copies;        // every copy starts on the shelf
    b.reservedCopies  = 0;
    b.shelf           = 0;             // 0 = no physical location assigned yet
    b.position        = 0;
    b.active          = true;

    books.push_back(b);

    cout << "\n  Title added successfully.\n";
    cout << "  ID   : " << b.bookID << '\n';
    cout << "  Title: " << b.title << '\n';
    cout << "  Use menu option 5 to assign a shelf location.\n";
}

// Menu option 2 - search sub-menu driven by a switch.
void searchBook(const vector<Book>& books) {
    printHeader("SEARCH BOOK");
    cout << "  1. Search by Book ID\n";
    cout << "  2. Search by Title\n";
    cout << "  3. Search by Author\n";
    cout << "  4. Search by ISBN\n";
    cout << "  0. Cancel\n\n";

    int choice = readInt("Enter choice: ", 0, 4);

    switch (choice) {
        case 1: searchBookByID(books);     break;
        case 2: searchBookByTitle(books);  break;
        case 3: searchBookByAuthor(books); break;
        case 4: searchBookByISBN(books);   break;
        case 0: break;
    }
}

void searchBookByID(const vector<Book>& books) {
    int id = readInt("\n  Book ID to search: ");
    int index = findBookIndex(books, id);

    if (index == -1) {
        cout << "\n  >> Book Not Found.\n";
        return;
    }

    printSearchHeader();
    displayBookRow(books[index]);
}

// Substring matches below all ignore upper/lower case.
// static = local to this file only (same idea as the helpers in common.cpp).
static string toLowerCopy(const string& text) {
    string result = text;
    for (int i = 0; i < (int)result.length(); i++)
        result[i] = tolower((unsigned char)result[i]);
    return result;
}

void searchBookByTitle(const vector<Book>& books) {
    string target = toLowerCopy(readLine("\n  Title to search: "));
    bool found = false;

    for (int i = 0; i < (int)books.size(); i++) {
        if (!books[i].active) continue;

        if (toLowerCopy(books[i].title).find(target) != string::npos) {
            if (!found) printSearchHeader();
            displayBookRow(books[i]);
            found = true;
        }
    }

    if (!found) cout << "\n  >> Book Not Found.\n";
}

void searchBookByAuthor(const vector<Book>& books) {
    string target = toLowerCopy(readLine("\n  Author to search: "));
    bool found = false;

    for (int i = 0; i < (int)books.size(); i++) {
        if (!books[i].active) continue;

        if (toLowerCopy(books[i].author).find(target) != string::npos) {
            if (!found) printSearchHeader();
            displayBookRow(books[i]);
            found = true;
        }
    }

    if (!found) cout << "\n  >> Book Not Found.\n";
}

void searchBookByISBN(const vector<Book>& books) {
    string target = readLine("\n  ISBN to search: ");

    for (int i = 0; i < (int)books.size(); i++) {
        if (!books[i].active) continue;

        if (books[i].isbn == target) {
            printSearchHeader();
            displayBookRow(books[i]);
            return;
        }
    }

    cout << "\n  >> Book Not Found.\n";
}

// Menu option 3 - update details. Blank input keeps the current value.
// Copy counts are deliberately NOT edited here; option 4 keeps them consistent.
void updateBook(vector<Book>& books) {
    printHeader("UPDATE BOOK DETAILS");
    int id = readInt("  Book ID to update: ");
    int index = findBookIndex(books, id);

    if (index == -1) {
        cout << "\n  >> Book Not Found.\n";
        return;
    }

    Book& b = books[index];

    cout << "\n  Current Details\n";
    cout << "  Title   : " << b.title << '\n';
    cout << "  Author  : " << b.author << '\n';
    cout << "  ISBN    : " << b.isbn << '\n';
    cout << "  Category: " << CATEGORY_NAME[b.category] << '\n';
    cout << "\n  Press Enter without typing to keep the current value.\n";

    string input = readLine("\n  New Title: ");
    if (!input.empty()) b.title = input;

    input = readLine("  New Author: ");
    if (!input.empty()) b.author = input;

    while (true) {
        input = readLine("  New ISBN: ");
        if (input.empty()) break;

        if (!isAllDigits(input) || (int)input.length() != ISBN_LEN) {
            cout << "  >> Invalid ISBN. Enter exactly " << ISBN_LEN << " digits.\n";
            continue;
        }
        if (findDuplicateISBN(books, input, index) != -1) {
            cout << "  >> Duplicate ISBN. Already used by another title.\n";
            continue;
        }
        b.isbn = input;
        break;
    }

    input = readLine("  New Category (1-6): ");
    if (!input.empty()) {
        int c = 0;
        for (int i = 0; i < (int)input.length(); i++)
            c = c * 10 + (input[i] - '0');

        if (isAllDigits(input) && c >= 1 && c <= MAX_CATEGORIES)
            b.category = c - 1;
        else
            cout << "  >> Invalid category. Kept unchanged.\n";
    }

    cout << "\n  Details updated successfully.\n";
}

// Menu option 4 - add / remove copies sub-menu.
void manageCopies(vector<Book>& books) {
    printHeader("ADD / REMOVE COPIES");
    cout << "  1. Add Copies\n";
    cout << "  2. Remove Copies\n";
    cout << "  0. Cancel\n\n";

    int choice = readInt("Enter choice: ", 0, 2);
    if (choice == 0) return;

    int id = readInt("  Book ID: ");
    int index = findBookIndex(books, id);

    if (index == -1) {
        cout << "\n  >> Book Not Found.\n";
        return;
    }

    Book& b = books[index];
    cout << "\n  " << b.title << " : " << b.totalCopies << " total, "
         << b.availableCopies << " available\n";

    int count = readInt("  Number of copies: ", 1, 999);

    if (choice == 1) {
        addCopies(books, index, count);
    } else {
        // nested if: the title must exist AND the removal must not take away
        // copies that are currently on loan.
        if (removeCopies(books, index, count))
            cout << "\n  " << count << " copy/copies removed.\n";
    }

    cout << "  Now: " << b.totalCopies << " total, "
         << b.availableCopies << " available\n";
}

void addCopies(vector<Book>& books, int index, int count) {
    books[index].totalCopies     += count;
    books[index].availableCopies += count;
}

// Returns false when the removal would go below the copies on loan.
bool removeCopies(vector<Book>& books, int index, int count) {
    Book& b = books[index];

    if (count > b.availableCopies) {
        cout << "\n  >> Cannot remove " << count << " copies. Only "
             << b.availableCopies << " are on the shelf; the rest are on loan.\n";
        return false;
    }

    b.totalCopies     -= count;
    b.availableCopies -= count;
    return true;
}

// Menu option 5 - place a title somewhere in the physical shelf grid.
// shelfLayout[shelf][position] stores the bookID occupying that slot,
// so an occupied slot can be detected with one array lookup.
void assignShelfLocation(vector<Book>& books) {
    printHeader("ASSIGN SHELF LOCATION");
    int id = readInt("  Book ID: ");
    int index = findBookIndex(books, id);

    if (index == -1) {
        cout << "\n  >> Book Not Found.\n";
        return;
    }

    Book& b = books[index];
    cout << "\n  " << b.title << " currently at: " << locationText(b) << "\n";

    int shelf   = readInt("  Shelf   (1-" + to_string(MAX_SHELVES) + "): ", 1, MAX_SHELVES);
    int position = readInt("  Position (1-" + to_string(SLOTS_PER_SHELF) + "): ", 1, SLOTS_PER_SHELF);

    // Free the previous slot first, so re-assigning the same book works.
    if (b.shelf >= 1 && b.position >= 1)
        shelfLayout[b.shelf - 1][b.position - 1] = EMPTY;

    if (shelfLayout[shelf - 1][position - 1] != EMPTY) {
        cout << "\n  >> Slot S" << shelf << "-P" << position
             << " is occupied by book " << shelfLayout[shelf - 1][position - 1]
             << ". Location unchanged.\n";

        // Put the book back where it came from.
        if (b.shelf >= 1 && b.position >= 1)
            shelfLayout[b.shelf - 1][b.position - 1] = b.bookID;
        return;
    }

    shelfLayout[shelf - 1][position - 1] = b.bookID;
    b.shelf    = shelf;
    b.position = position;

    cout << "\n  '" << b.title << "' assigned to S" << shelf << "-P" << position << ".\n";
}

// Menu option 6 - SOFT DELETE only.
// The record stays (active = false) so loan/fine history in other modules can
// still show the title. A title with copies out on loan cannot be removed.
void removeBook(vector<Book>& books) {
    printHeader("REMOVE TITLE");
    int id = readInt("  Book ID to remove: ");
    int index = findBookIndex(books, id);

    if (index == -1) {
        cout << "\n  >> Book Not Found.\n";
        return;
    }

    Book& b = books[index];
    int onLoan = b.totalCopies - b.availableCopies;

    if (onLoan > 0) {
        cout << "\n  >> Cannot remove. " << onLoan
             << " copy/copies of this title are currently on loan.\n";
        return;
    }

    if (b.reservedCopies > 0) {
        cout << "\n  >> Cannot remove. A returned copy is being held for the"
             << " reservation queue.\n";
        return;
    }

    if (!confirmYesNo("\n  Remove '" + b.title + "' from the catalogue")) {
        cout << "\n  Removal cancelled.\n";
        return;
    }

    b.active = false;

    // Clear the physical shelf slot.
    if (b.shelf >= 1 && b.position >= 1) {
        shelfLayout[b.shelf - 1][b.position - 1] = EMPTY;
        b.shelf = 0;
        b.position = 0;
    }

    // Drop anyone still queuing for this title (row = catalogue index).
    for (int q = 0; q < MAX_QUEUE; q++)
        reserveQueue[index][q] = EMPTY;

    cout << "\n  Title removed. Historical records remain available.\n";
}

// Menu option 7 - full catalogue listing plus per-category totals.
void displayCatalogue(const vector<Book>& books) {
    printHeader("DISPLAY CATALOGUE");

    if (countActiveBooks(books) == 0) {
        cout << "\n  The catalogue is empty.\n";
        return;
    }

    printSearchHeader();

    int titles = 0;
    int totalCopies = 0;

    for (int i = 0; i < (int)books.size(); i++) {
        if (!books[i].active) continue;

        displayBookRow(books[i]);       // pass-by-value copy of the record
        titles++;
        totalCopies += books[i].totalCopies;
    }

    printRule();
    cout << "  Titles: " << titles << "   Total copies held: " << totalCopies << "\n\n";

    cout << left << setw(16) << "  Category" << right << setw(8) << "Titles" << '\n';
    printRule();

    // countByCategory() returns its answer by value.
    for (int c = 0; c < MAX_CATEGORIES; c++)
        cout << left << setw(16) << CATEGORY_NAME[c]
             << right << setw(8) << countByCategory(books, c) << '\n';
}

// Counts ACTIVE titles in one category. Returns the answer by value.
int countByCategory(const vector<Book>& books, int category) {
    int count = 0;
    for (int i = 0; i < (int)books.size(); i++)
        if (books[i].active && books[i].category == category)
            count++;
    return count;
}

// Menu option 8 - at-a-glance availability status of every active title.
void displayAvailability(const vector<Book>& books) {
    printHeader("DISPLAY AVAILABILITY LISTING");

    if (countActiveBooks(books) == 0) {
        cout << "\n  The catalogue is empty.\n";
        return;
    }

    cout << "\n" << left
         << setw(6)  << "ID"
         << setw(36) << "Title"
         << right
         << setw(5)  << "Tot"
         << setw(5)  << "Avl"
         << setw(7)  << "Held"
         << "  " << left << setw(22) << "Status" << '\n';
    printRule();

    for (int i = 0; i < (int)books.size(); i++) {
        if (!books[i].active) continue;

        string status;
        if (books[i].reservedCopies > 0)
            status = "RESERVED - copy held";       // waiting list exists
        else if (books[i].availableCopies == 0)
            status = "ALL COPIES ON LOAN";
        else
            status = "AVAILABLE";

        cout << left
             << setw(6)  << books[i].bookID
             << setw(36) << books[i].title.substr(0, 35)
             << right
             << setw(5)  << books[i].totalCopies
             << setw(5)  << books[i].availableCopies
             << setw(7)  << books[i].reservedCopies
             << "  " << left << setw(22) << status
             << '\n';
    }

    printRule();
    cout << "  Held = returned copies kept aside for the next member in the queue.\n";

    displayShelfMap();
}

// Reads the shared 2D shelfLayout array directly, so no parameter is needed.
void displayShelfMap() {
    cout << "\n  SHELF MAP\n";

    for (int s = 0; s < MAX_SHELVES; s++) {          // outer loop: shelves
        bool any = false;

        cout << "   Shelf " << setw(2) << s + 1 << " : ";

        for (int p = 0; p < SLOTS_PER_SHELF; p++) {  // inner loop: slots
            if (shelfLayout[s][p] != EMPTY) {
                cout << "[P" << p + 1 << "=" << shelfLayout[s][p] << "] ";
                any = true;
            }
        }

        if (!any) cout << "(empty)";
        cout << '\n';
    }
}
