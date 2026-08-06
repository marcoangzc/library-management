#ifndef COMMON_H
#define COMMON_H
//this file contains shared constants, records and helper declarations across all modules
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <limits>
#include <sstream>
#include <cctype>

using namespace std;

//Global constants (shared by all modules)

//CAPACITY CONSTANTS
//These size the shared 2D arrays. Every module MUST check against
//them before push_back(), otherwise the 2D arrays overflow.
const int MAX_MEMBERS = 100; // rows of memberLoanHistory
const int MAX_BOOKS = 200; // rows of reserveQueue, size of borrowCount
const int MAX_QUEUE = 10; // columns of reserveQueue
const int MAX_CATEGORIES  = 6;
const int MAX_SHELVES     = 10;
const int SLOTS_PER_SHELF = 20;
const int MONTHS          = 12;

//BUSINESS RULE CONSTANTS
const int LOAN_DAYS = 14;   // loan period
const int MAX_RENEW = 2;    // maximum renewals per loan
const int EMPTY     = 0;    // sentinel for a free reservation slot

const double FINE_PER_DAY = 0.30;
const double FINE_CAP = 20.00;

//Membership tier table
// One tier code -> one borrow limit -> one fine rate multiplier.
// Do NOT hard-code these numbers inside a module; call the two
// look-up functions getBorrowLimit() / getFineRate() instead.
const char TIER_STUDENT = 'S';
const char TIER_STAFF   = 'F';
const char TIER_PREMIUM = 'P';
 
const int LIMIT_STUDENT = 3;
const int LIMIT_STAFF   = 4;
const int LIMIT_PREMIUM = 6;
 
const double RATE_STUDENT = 1.00;   // pays full fine
const double RATE_STAFF   = 0.50;   // pays half
const double RATE_PREMIUM = 0.00;   // fine waived

//Validation limits 
const int PHONE_MIN_LEN = 10;
const int PHONE_MAX_LEN = 11;
const int NAME_MAX_LEN  = 50;
const int ISBN_LEN      = 13;

//Loan status codes
//note: 'O' is not stored in the loan record, it is derived from the due date and system date
const char LOAN_ISSUED   = 'I';
const char LOAN_RETURNED = 'R';

//canBorrow() result codes
const int BORROW_OK        = 0;
const int BORROW_INACTIVE  = 1;
const int BORROW_SUSPENDED = 2;
const int BORROW_HAS_FINE  = 3;
const int BORROW_AT_LIMIT  = 4;

//DATE RECORD AND DATE ARITHMETIC
//  Dates are stored as a struct, not a string, so that adding days, subtracting days, and comparing dates is easier and more reliable than parsing strings.
struct Date {
    int day;
    int month;
    int year;
};

bool   isLeapYear(int year);
int    daysInMonth(int month, int year);
bool   isValidDate(Date d);
 
int    toDayNumber(Date d);            // date  -> serial day number
Date   fromDayNumber(int dayNumber);   // serial day number -> date
Date   addDays(Date d, int days);      // due date = addDays(issue, LOAN_DAYS)
int    daysBetween(Date from, Date to);// negative if 'to' is earlier
int    compareDate(Date a, Date b);    // -1 a<b, 0 equal, 1 a>b
 
string dateToString(Date d);                        // "2026-08-06"
bool   parseDate(const string& text, Date& result); // "2026-08-06" -> Date

// Shared structures (same as report Section 2.4)
struct Member {
    int    memID;
    string name;
    string phone;
    char   tier;             // 'S' student, 'F' staff, 'P' premium
    int    booksOnLoan;
    double outstandingFine;
    bool   suspended;
    bool   active;           // false = deleted (soft delete, see note below)
};

struct Book {
    int    bookID;
    string title;
    string author;
    string isbn;
    int    category;         // 0 .. MAX_CATEGORIES-1
    int    totalCopies;
    int    availableCopies;  // copies on the shelf and free to borrow
    int    reservedCopies;   // returned copies held for the queue
    int    shelf;
    int    position;
    bool   active;           // false = removed from catalogue
};

struct Loan {
    int  loanID;
    int  memID;
    int  bookID;
    Date issueDate;
    Date dueDate;
    Date returnDate;         // only meaningful when status == LOAN_RETURNED
    int  renewCount;
    char status;             // LOAN_ISSUED or LOAN_RETURNED
};

struct Fine {
    int    fineID;
    int    loanID;
    int    memID;
    int    daysOverdue;
    double amount;           // fine charged, after tier rate and cap
    double paid;             // total paid so far
    Date   payDate;
    bool   settled;
};


// SHARED GLOBAL DATA (defined in common.cpp)
// note: these are not passed into each module, they are shared by all modules
//extern used here to avoid multiple definition errors when linking
extern int    reserveQueue[MAX_BOOKS][MAX_QUEUE];        // written by Module 3
extern int    memberLoanHistory[MAX_MEMBERS][MAX_CATEGORIES]; // written by Module 3
extern int    shelfLayout[MAX_SHELVES][SLOTS_PER_SHELF]; // written by Module 2
extern double monthlyStats[MONTHS][MAX_CATEGORIES];      // written by Module 4
extern int    borrowCount[MAX_BOOKS];                    // written by Module 3
extern string CATEGORY_NAME[MAX_CATEGORIES];

extern Date systemDate; /// The simulated "today". Every overdue / due-date calculation reads this
//note: this is not a real-time clock, it is a testing utility to demonstrate overdue cases

//USER INTERFACE HELPERS (common.cpp)
//  All input goes through these so prompts and error messages are
//  identical across the four modules.
void clearScreen();
void pressEnterToContinue();
void printHeader(const string& title);
void printRule(char symbol = '-', int width = 65);

int    readInt(const string& prompt);                                  // any int
int    readInt(const string& prompt, int minVal, int maxVal);          // range checked
double readDouble(const string& prompt, double minVal, double maxVal);
string readLine(const string& prompt);                                 // trimmed
string readNonEmptyLine(const string& prompt, int maxLen);
char   readChar(const string& prompt, const string& validChars);       // upper-cased
Date   readDate(const string& prompt);
bool   confirmYesNo(const string& prompt);
 
string trim(const string& text);
bool   isAllDigits(const string& text);

//SHARED BUSINESS HELPERS
// Look-ups return the vector INDEX, or -1 when not found.
// The "Any" versions also match soft-deleted records; use them when
// printing history that refers to a deleted member or title.
int findMemberIndex(const vector<Member>& members, int memID);
int findMemberIndexAny(const vector<Member>& members, int memID);
int findBookIndex(const vector<Book>& books, int bookID);
int findBookIndexAny(const vector<Book>& books, int bookID);
int findLoanIndex(const vector<Loan>& loans, int loanID);
 
int  nextMemberID(const vector<Member>& members);
int  nextBookID(const vector<Book>& books);
int  nextLoanID(const vector<Loan>& loans);
int  nextFineID(const vector<Fine>& fines);
 
int  countActiveMembers(const vector<Member>& members);
int  countActiveBooks(const vector<Book>& books);

// Membership tier look-ups
int    getBorrowLimit(char tier);
double getFineRate(char tier);
string getTierName(char tier);

// Borrowing eligibility check
//// Returns one of the BORROW_* codes so the caller can print the reason.
int    canBorrow(const Member& m);
string borrowStatusMessage(int statusCode);

// Derived values - never stored, always calculated.
double fineBalance(const Fine& f);
bool   isOverdue(const Loan& l, Date today);
int    daysOverdue(const Loan& l, Date today);
 
bool decrementAvailable(vector<Book>& books, int index);
void incrementAvailable(vector<Book>& books, int index);
 
// Seed the four shared collections with sample data for testing and demonstration.
void seedData(vector<Member>& members, vector<Book>& books,
              vector<Loan>& loans, vector<Fine>& fines);

// MODULE ENTRY POINTS
void memberManagementMenu(vector<Member>& members,
                          const vector<Loan>& loans);// Module 1
 
void bookCatalogueMenu(vector<Book>& books); // Module 2
 
void loanMenu(vector<Loan>& loans, vector<Book>& books,
              vector<Member>& members, vector<Fine>& fines);// Module 3
 
void fineMenu(vector<Fine>& fines, vector<Loan>& loans,
              vector<Member>& members, vector<Book>& books);// Module 4

#endif