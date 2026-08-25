#include "common.h"
#include <cstdlib>
#include <sstream>
//this file contains the implementation of the shared data and helper functions

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

// The simulated current date. Changed only from the System Date utility.
Date systemDate = { 6, 8, 2026 };

// DATE ARITHMETIC
// The following functions are used to perform date arithmetic and comparisons.
bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}
 
int daysInMonth(int month, int year) {
    const int DIM[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (month < 1 || month > 12) return 0;
    if (month == 2 && isLeapYear(year)) return 29;
    return DIM[month - 1];
}
 
bool isValidDate(Date d) {
    if (d.year < 1900 || d.year > 2999) return false;
    if (d.month < 1 || d.month > 12)    return false;
    if (d.day < 1 || d.day > daysInMonth(d.month, d.year)) return false;
    return true;
}
 
// Converts a date into a running day count, so that two dates can be
// subtracted directly. Day 1 is 1 January of year 1.
int toDayNumber(Date d) {
    int completedYears = d.year - 1;
    int total = completedYears * 365
              + completedYears / 4
              - completedYears / 100
              + completedYears / 400;
 
    for (int m = 1; m < d.month; m++)
        total += daysInMonth(m, d.year);
 
    return total + d.day;
}
 
Date fromDayNumber(int dayNumber) {
    Date d;
    int year = 1;
 
    while (dayNumber > (isLeapYear(year) ? 366 : 365)) {
        dayNumber -= (isLeapYear(year) ? 366 : 365);
        year++;
    }
 
    int month = 1;
    while (dayNumber > daysInMonth(month, year)) {
        dayNumber -= daysInMonth(month, year);
        month++;
    }
 
    d.day   = dayNumber;
    d.month = month;
    d.year  = year;
    return d;
}
 
Date addDays(Date d, int days) {
    return fromDayNumber(toDayNumber(d) + days);
}
 
int daysBetween(Date from, Date to) {
    return toDayNumber(to) - toDayNumber(from);
}
 
int compareDate(Date a, Date b) {
    int diff = daysBetween(b, a);
    if (diff < 0) return -1;
    if (diff > 0) return 1;
    return 0;
}
 
string dateToString(Date d) {
    std::ostringstream out;
    out << std::setfill('0') << std::setw(4) << d.year  << '-'
        << std::setfill('0') << std::setw(2) << d.month << '-'
        << std::setfill('0') << std::setw(2) << d.day;
    return out.str();
}

// Local helper: converts a string of digits into an int without stoi().
static int digitsToInt(const string& text) {
    int value = 0;
    for (size_t i = 0; i < text.length(); i++)
        value = value * 10 + (text[i] - '0');
    return value;
}

// Parses a date string in the format "YYYY-MM-DD" into a Date struct.
// Returns true if successful, false if the string is invalid.
bool parseDate(const string& text, Date& result) {
    if (text.length() != 10)                 return false;
    if (text[4] != '-' || text[7] != '-')    return false;
 
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit((unsigned char)text[i])) return false;
    }
 
    Date d;
    d.year  = digitsToInt(text.substr(0, 4));
    d.month = digitsToInt(text.substr(5, 2));
    d.day   = digitsToInt(text.substr(8, 2));
 
    if (!isValidDate(d)) return false;
 
    result = d;
    return true;
}

// UI helpers
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printRule(char symbol, int width) {
    for (int i = 0; i < width; i++) cout << symbol;
    cout << '\n';
}

void pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

// Reads a line of input from the user, trims whitespace, and returns it.
void printHeader(const string& title) {
    clearScreen();
    printRule('=');
    cout << "  " << title << '\n';
    cout << "  System Date: " << dateToString(systemDate) << '\n';
    printRule('=');
    cout << '\n';
}

// Reads a line of input from the user, trims whitespace, and returns it.
string trim(const string& text) {
    size_t start = 0;
    size_t end   = text.length();
 
    while (start < end && isspace((unsigned char)text[start])) start++;
    while (end > start && isspace((unsigned char)text[end - 1])) end--;
 
    return text.substr(start, end - start);
}
 
// Checks if a string consists entirely of digits.
bool isAllDigits(const string& text) {
    if (text.empty()) return false;
    for (size_t i = 0; i < text.length(); i++)
        if (!isdigit((unsigned char)text[i])) return false;
    return true;
}

// Reads an integer from the user, with optional range checking.
int readInt(const string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minVal && value <= maxVal) {
            // Discard the newline so a later getline() is not skipped.
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  >> Invalid input. Enter a whole number from "
             << minVal << " to " << maxVal << ".\n";
    }
}

int readInt(const string& prompt) {
    return readInt(prompt, numeric_limits<int>::min(),
                           numeric_limits<int>::max());
}

double readDouble(const string& prompt, double minVal, double maxVal) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minVal && value <= maxVal) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  >> Invalid amount. Enter a number from "
             << fixed << setprecision(2) << minVal
             << " to " << maxVal << ".\n";
    }
}
 
string readLine(const string& prompt) {
    string text;
    cout << prompt;
    getline(cin, text);
    return trim(text);
}
 
string readNonEmptyLine(const string& prompt, int maxLen) {
    while (true) {
        string text = readLine(prompt);
        if (text.empty()) {
            cout << "  >> This field cannot be left blank.\n";
        } else if ((int)text.length() > maxLen) {
            cout << "  >> Too long. Maximum " << maxLen << " characters.\n";
        } else {
            return text;
        }
    }
}
 
char readChar(const string& prompt, const string& validChars) {
    while (true) {
        string text = readLine(prompt);
        if (text.length() == 1) {
            char c = toupper(text[0]);
            if (validChars.find(c) != string::npos) return c;
        }
        cout << "  >> Invalid entry. Expected one of: " << validChars << '\n';
    }
}
 
Date readDate(const string& prompt) {
    Date d;
    while (true) {
        string text = readLine(prompt);
        if (parseDate(text, d)) return d;
        cout << "  >> Invalid date. Use the format YYYY-MM-DD, "
             << "e.g. " << dateToString(systemDate) << ".\n";
    }
}
 
bool confirmYesNo(const string& prompt) {
    return readChar(prompt + " (Y/N): ", "YN") == 'Y';
}


// Shared business helpers
//functions that are used across modules to find members, books, and check borrowing eligibility
int findMemberIndex(const vector<Member>& members, int memID) {
    for (size_t i = 0; i < members.size(); i++)
        if (members[i].active && members[i].memID == memID)
            return (int)i;
    return -1;
}
 
// Same as above, but includes soft-deleted members.
int findMemberIndexAny(const vector<Member>& members, int memID) {
    for (size_t i = 0; i < members.size(); i++)
        if (members[i].memID == memID)
            return (int)i;
    return -1;
}

//functions that are used across modules to find books, and check borrowing eligibility
int findBookIndex(const vector<Book>& books, int bookID) {
    for (size_t i = 0; i < books.size(); i++)
        if (books[i].active && books[i].bookID == bookID)
            return (int)i;
    return -1;
}

int findBookIndexAny(const vector<Book>& books, int bookID) {
    for (size_t i = 0; i < books.size(); i++)
        if (books[i].bookID == bookID)
            return (int)i;
    return -1;
}
 
int findLoanIndex(const vector<Loan>& loans, int loanID) {
    for (size_t i = 0; i < loans.size(); i++)
        if (loans[i].loanID == loanID)
            return (int)i;
    return -1;
}
 
int nextMemberID(const vector<Member>& members) {
    int highest = 1000;
    for (size_t i = 0; i < members.size(); i++)
        if (members[i].memID > highest) highest = members[i].memID;
    return highest + 1;
}
 
int nextBookID(const vector<Book>& books) {
    int highest = 2000;
    for (size_t i = 0; i < books.size(); i++)
        if (books[i].bookID > highest) highest = books[i].bookID;
    return highest + 1;
}
 
int nextLoanID(const vector<Loan>& loans) {
    int highest = 5000;
    for (size_t i = 0; i < loans.size(); i++)
        if (loans[i].loanID > highest) highest = loans[i].loanID;
    return highest + 1;
}
 
int nextFineID(const vector<Fine>& fines) {
    int highest = 9000;
    for (size_t i = 0; i < fines.size(); i++)
        if (fines[i].fineID > highest) highest = fines[i].fineID;
    return highest + 1;
}
 
int countActiveMembers(const vector<Member>& members) {
    int count = 0;
    for (size_t i = 0; i < members.size(); i++)
        if (members[i].active) count++;
    return count;
}
 
int countActiveBooks(const vector<Book>& books) {
    int count = 0;
    for (size_t i = 0; i < books.size(); i++)
        if (books[i].active) count++;
    return count;
}
 
int getBorrowLimit(char tier) {
    switch (toupper(tier)) {
        case TIER_PREMIUM: return LIMIT_PREMIUM;
        case TIER_STAFF:   return LIMIT_STAFF;
        default:           return LIMIT_STUDENT;
    }
}
 
double getFineRate(char tier) {
    switch (toupper(tier)) {
        case TIER_PREMIUM: return RATE_PREMIUM;
        case TIER_STAFF:   return RATE_STAFF;
        default:           return RATE_STUDENT;
    }
}
 
string getTierName(char tier) {
    switch (toupper(tier)) {
        case TIER_PREMIUM: return "Premium";
        case TIER_STAFF:   return "Staff";
        case TIER_STUDENT: return "Student";
        default:           return "Unknown";
    }
}
// Returns a reason code rather than a plain bool, so the calling module
// can tell the user exactly why the loan was refused.
int canBorrow(const Member& m) {
    if (!m.active)                                  return BORROW_INACTIVE;
    if (m.suspended)                                return BORROW_SUSPENDED;
    if (m.outstandingFine > 0.0)                    return BORROW_HAS_FINE;
    if (m.booksOnLoan >= getBorrowLimit(m.tier))    return BORROW_AT_LIMIT;
    return BORROW_OK;
}
 
string borrowStatusMessage(int statusCode) {
    switch (statusCode) {
        case BORROW_OK:        return "Member is eligible to borrow.";
        case BORROW_INACTIVE:  return "Membership record is no longer active.";
        case BORROW_SUSPENDED: return "Membership is suspended.";
        case BORROW_HAS_FINE:  return "Outstanding fine must be settled first.";
        case BORROW_AT_LIMIT:  return "Borrowing limit for this tier reached.";
        default:               return "Unknown status.";
    }
}
 
double fineBalance(const Fine& f) {
    double balance = f.amount - f.paid;
    return (balance < 0.0) ? 0.0 : balance;
}
 
bool isOverdue(const Loan& l, Date today) {
    if (l.status != LOAN_ISSUED) return false;
    return daysBetween(l.dueDate, today) > 0;
}
 
int daysOverdue(const Loan& l, Date today) {
    Date reference = (l.status == LOAN_RETURNED) ? l.returnDate : today;
    int days = daysBetween(l.dueDate, reference);
    return (days > 0) ? days : 0;
}
 
bool decrementAvailable(vector<Book>& books, int index) {
    if (index < 0 || index >= (int)books.size()) return false;
    if (books[index].availableCopies <= 0)       return false;
    books[index].availableCopies--;
    return true;
}
 
void incrementAvailable(vector<Book>& books, int index) {
    if (index >= 0 && index < (int)books.size())
        books[index].availableCopies++;
}
// load a set of sample data into the four shared collections for testing and demonstration.
// Local helpers keep seedData() readable.
static Member makeMember(int id, string name, string phone, char tier,
                         int onLoan, double fine, bool suspended) {
    Member m;
    m.memID = id;  m.name = name;  m.phone = phone;  m.tier = tier;
    m.booksOnLoan = onLoan;  m.outstandingFine = fine;
    m.suspended = suspended;  m.active = true;
    return m;
}
 
static Book makeBook(int id, string title, string author, string isbn,
                     int category, int total, int available,
                     int shelf, int position) {
    Book b;
    b.bookID = id;  b.title = title;  b.author = author;  b.isbn = isbn;
    b.category = category;  b.totalCopies = total;
    b.availableCopies = available;  b.reservedCopies = 0;
    b.shelf = shelf;  b.position = position;  b.active = true;
    return b;
}
 
static Loan makeLoan(int id, int memID, int bookID, Date issue,
                     int renewCount, char status) {
    Loan l;
    l.loanID = id;  l.memID = memID;  l.bookID = bookID;
    l.issueDate = issue;
    l.dueDate = addDays(issue, LOAN_DAYS);
    l.returnDate = issue;
    l.renewCount = renewCount;  l.status = status;
    return l;
}
// Populates the four shared collections with sample data for testing and demonstration.
void seedData(vector<Member>& members, vector<Book>& books,
              vector<Loan>& loans, vector<Fine>& fines) {
 
    //Members 
    members.push_back(makeMember(1001, "Tan Wei Ming",   "0123456789", TIER_STUDENT, 2, 0.00,  false));
    members.push_back(makeMember(1002, "Nurul Aisyah",   "0198765432", TIER_STAFF,   2, 0.00,  false));
    members.push_back(makeMember(1003, "Ravi Kumar",     "01122334455",TIER_PREMIUM, 0, 0.00,  false));
    members.push_back(makeMember(1004, "Lim Siew Ling",  "0176543210", TIER_STUDENT, 1, 4.50,  false));
    members.push_back(makeMember(1005, "Chong Kai Xin",  "0134567890", TIER_STUDENT, 0, 0.00,  true));
    members.push_back(makeMember(1006, "Ahmad Faizal",   "0111222333", TIER_STAFF,   0, 0.00,  false));
 
    //Books
    //cat 0 = Fiction, 1 = Non-Fiction, 2 = Science, 3 = Technology, 4 = History, 5 = Children
    //tot = total copies, avl = available copies, sh = shelf, pos = position
    //                       ID    title                      author              isbn            cat tot avl sh pos
    books.push_back(makeBook(2001, "The Silent Patient",      "Alex Michaelides", "9781250301697", 0,  4,  3, 1,  1));
    books.push_back(makeBook(2002, "Sapiens",                 "Yuval Noah Harari","9780062316097", 1,  3,  2, 1,  2));
    books.push_back(makeBook(2003, "A Brief History of Time", "Stephen Hawking",  "9780553380163", 2,  2,  0, 2,  1));
    books.push_back(makeBook(2004, "Clean Code",              "Robert C. Martin", "9780132350884", 3,  5,  5, 2,  2));
    books.push_back(makeBook(2005, "Guns, Germs and Steel",   "Jared Diamond",    "9780393317558", 4,  2,  2, 3,  1));
    books.push_back(makeBook(2006, "Matilda",                 "Roald Dahl",       "9780142410370", 5,  6,  6, 3,  2));
    books.push_back(makeBook(2007, "The Pragmatic Programmer","Andrew Hunt",      "9780135957059", 3,  3,  3, 2,  3));
    books.push_back(makeBook(2008, "Educated",                "Tara Westover",    "9780399590504", 1,  2,  1, 1,  3));
 
    // ---- Shelf layout grid ------------------------------------------
    for (size_t i = 0; i < books.size(); i++)
        shelfLayout[books[i].shelf][books[i].position] = books[i].bookID;
 
    // ---- Loans -------------------------------------------------------
    // One loan is already 8 days overdue relative to the default system
    // date, so the fine calculation can be demonstrated immediately.
    Date d0 = {  1, 6, 2026 };   // due 15-06, returned 30-06 : 15 days late
    Date d1 = { 25, 7, 2026 };   // due 08-08 : still current
    Date d2 = { 10, 7, 2026 };   // due 24-07 : overdue
    Date d3 = { 15, 7, 2026 };   // due 29-07 : overdue
    Date d4 = { 30, 7, 2026 };   // due 13-08 : still current
    Date d5 = { 20, 7, 2026 };   // due 03-08 : overdue
 
    // A completed loan that was returned late - this is the loan the
    // outstanding fine record below refers to.
    Loan returned = makeLoan(5000, 1004, 2006, d0, 0, LOAN_RETURNED);
    returned.returnDate = Date{ 30, 6, 2026 };   // standard C++ brace init
    loans.push_back(returned);
 
    loans.push_back(makeLoan(5001, 1001, 2001, d1, 0, LOAN_ISSUED));
    loans.push_back(makeLoan(5002, 1002, 2003, d2, 1, LOAN_ISSUED));
    loans.push_back(makeLoan(5003, 1004, 2002, d3, 0, LOAN_ISSUED));
    loans.push_back(makeLoan(5004, 1002, 2008, d4, 0, LOAN_ISSUED));
    loans.push_back(makeLoan(5005, 1001, 2003, d5, 0, LOAN_ISSUED));
 
    //Reservation queue (row = book index, not book ID)
    int index2003 = findBookIndex(books, 2003);   // no copies available
    if (index2003 >= 0) {
        reserveQueue[index2003][0] = 1003;
        reserveQueue[index2003][1] = 1006;
    }
 
    //Borrow statistics
    int history[8] = { 12, 9, 15, 7, 3, 11, 5, 6 };
    for (size_t i = 0; i < books.size(); i++)
        borrowCount[i] = history[i];
 
    //Member borrowing history by category
    memberLoanHistory[0][0] = 4;  memberLoanHistory[0][3] = 2;
    memberLoanHistory[1][1] = 3;  memberLoanHistory[1][2] = 5;
    memberLoanHistory[2][3] = 6;
    memberLoanHistory[3][0] = 2;  memberLoanHistory[3][5] = 1;
 
    //One outstanding fine, matching member 1004
    Fine f;
    // Loan 5000: due 2026-06-15, returned 2026-06-30 = 15 days late.
    // Student tier pays the full rate: 15 * 0.30 = 4.50, under the cap.
    f.fineID = 9001;  f.loanID = 5000;  f.memID = 1004;
    f.daysOverdue = 15;  f.amount = 4.50;  f.paid = 0.00;
    f.payDate = systemDate;  f.settled = false;
    fines.push_back(f);
 
    //Fine revenue already collected this year
    monthlyStats[5][0] = 12.60;   // June, Fiction
    monthlyStats[5][2] =  8.40;   // June, Science
    monthlyStats[6][0] =  6.90;   // July, Fiction
    monthlyStats[6][3] = 15.30;   // July, Technology
}