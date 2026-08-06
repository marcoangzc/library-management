#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <limits>

using namespace std;

// =====================================================
// Global constants (shared by all modules)
// =====================================================
const int MAX_MEMBERS = 100;
const int MAX_BOOKS = 200;
const int MAX_QUEUE = 10;
const int LOAN_DAYS = 14;

const double FINE_PER_DAY = 0.30;
const double FINE_CAP = 20.00;

const int MAX_CATEGORIES = 6;
const int MAX_SHELVES = 10;
const int SLOTS_PER_SHELF = 20;

const int PHONE_MIN_LEN = 10;
const int PHONE_MAX_LEN = 11;
const int NAME_MAX_LEN = 50;

const int MAX_RENEW = 2;
const int EMPTY = 0;

const int BORROW_LIMIT_NORMAL = 3;
const int BORROW_LIMIT_PREMIUM = 6;

const int MONTHS = 12;

const double RATE_STUDENT = 1.00;
const double RATE_STAFF = 0.50;
const double RATE_PREMIUM = 0.00;

// Shared structures (same as report Section 2.4)
struct Member {
    int memID;
    string name;
    string phone;
    char tier;              // e.g. 'S' student, 'F' staff, 'P' premium, 'N' normal
    int booksOnLoan;
    double outstandingFine;
    bool suspended;
};

struct Book {
    int bookID;
    string title;
    string author;
    string isbn;
    int category;
    int totalCopies;
    int availableCopies;
    int shelf;
    int position;
};

struct Loan {
    int loanID;
    int memID;
    int bookID;
    string issueDate;       // format: YYYY-MM-DD
    string dueDate;
    string returnDate;
    int renewCount;
    char status;            // 'I' issued, 'R' returned, 'O' overdue
};

struct Fine {
    int fineID;
    int loanID;
    int memID;
    int daysOverdue;
    double amount;
    double paid;
    double balance;
    string payDate;
};


// Shared global arrays (defined in common.cpp)
extern int reserveQueue[MAX_BOOKS][MAX_QUEUE];
extern int memberLoanHistory[MAX_MEMBERS][MAX_CATEGORIES];
extern int shelfLayout[MAX_SHELVES][SLOTS_PER_SHELF];
extern double monthlyStats[MONTHS][MAX_CATEGORIES];
extern int borrowCount[MAX_BOOKS];
extern string CATEGORY_NAME[MAX_CATEGORIES];


// Shared helper functions (common.cpp)
void clearScreen();
void pressEnterToContinue();
void printHeader(const string& title);
int readInt(const string& prompt);

int findMemberIndex(const vector<Member>& members, int memID);
int findBookIndex(const vector<Book>& books, int bookID);
bool canBorrow(const Member& m, int limit);
bool decrementAvailable(vector<Book>& books, int index);
void incrementAvailable(vector<Book>& books, int index);


// Module functions 
void memberManagementMenu(vector<Member>& members);          // Module 1
void bookCatalogueMenu(vector<Book>& books);                 // Module 2
void loanMenu(vector<Loan>& loans, vector<Book>& books,
              vector<Member>& members);                      // Module 3
void fineMenu(vector<Fine>& fines, vector<Loan>& loans,
              vector<Member>& members,
              vector<Book>& books);                          // Module 4

#endif