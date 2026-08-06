#include "common.h"
#include <cstdlib>


// Shared global arrays
int reserveQueue[MAX_BOOKS][MAX_QUEUE] = {};
int memberLoanHistory[MAX_MEMBERS][MAX_CATEGORIES] = {};
int shelfLayout[MAX_SHELVES][SLOTS_PER_SHELF] = {};
double monthlyStats[MONTHS][MAX_CATEGORIES] = {};
int borrowCount[MAX_BOOKS] = {};

string CATEGORY_NAME[MAX_CATEGORIES] = {
    "Fiction", "Non-Fiction", "Science",
    "Technology", "History", "Children"
};


// UI helpers
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void printHeader(const string& title) {
    clearScreen();
    cout << "=================================================\n";
    cout << "  " << title << "\n";
    cout << "=================================================\n\n";
}

int readInt(const string& prompt) {
    int value;
    cout << prompt;
    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a number: ";
    }
    return value;
}


// Shared helpers used across modules
int findMemberIndex(const vector<Member>& members, int memID) {
    for (size_t i = 0; i < members.size(); i++) {
        if (members[i].memID == memID)
            return (int)i;
    }
    return -1;
}

int findBookIndex(const vector<Book>& books, int bookID) {
    for (size_t i = 0; i < books.size(); i++) {
        if (books[i].bookID == bookID)
            return (int)i;
    }
    return -1;
}

bool canBorrow(const Member& m, int limit) {
    if (m.suspended) return false;
    if (m.outstandingFine > 0.0) return false;
    if (m.booksOnLoan >= limit) return false;
    return true;
}

bool decrementAvailable(vector<Book>& books, int index) {
    if (index < 0 || index >= (int)books.size()) return false;
    if (books[index].availableCopies <= 0) return false;
    books[index].availableCopies--;
    return true;
}

void incrementAvailable(vector<Book>& books, int index) {
    if (index >= 0 && index < (int)books.size())
        books[index].availableCopies++;
}