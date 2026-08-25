#include "common.h"
#include <cmath>

// Module 3 - Loan, Return & Reservation Management - leong zheng yang
//
// The reservation waiting list lives in the shared 2D array reserveQueue:
// row = catalogue index of the title, column = position in the queue.
// Each cell holds the member ID of a waiting member, or EMPTY (0).
// A returned copy for a queued title is NOT put back on the shelf; instead
// reservedCopies is increased and the copy is held until the first member
// in the queue borrows it or cancels.

// ---- function prototypes ----
void   issueLoan(vector<Loan>& loans, vector<Book>& books, vector<Member>& members);
void   renewLoan(vector<Loan>& loans, const vector<Book>& books);
void   returnBook(vector<Loan>& loans, vector<Book>& books,
                  vector<Member>& members, vector<Fine>& fines);
void   requestReservation(const vector<Member>& members, vector<Book>& books);
void   requestCancellation(const vector<Member>& members, vector<Book>& books);
void   displayActiveLoans(const vector<Loan>& loans, const vector<Book>& books,
                          const vector<Member>& members);
void   displayReservationQueue(int queue[][MAX_QUEUE], const vector<Book>& books,
                               const vector<Member>& members);

Date   calcDueDate(Date issueDate);                 // issue date + LOAN_DAYS
bool   placeReservation(int queue[][MAX_QUEUE], int bookIndex, int memID);
bool   cancelReservation(int queue[][MAX_QUEUE], int bookIndex, int memID);
int    promoteNextInQueue(int queue[][MAX_QUEUE], int bookIndex);

static void joinQueueOffer(int bookIndex, int memID);
static int  findQueuePosition(int queue[][MAX_QUEUE], int bookIndex, int memID);
static int  firstFreeSlot(int queue[][MAX_QUEUE], int bookIndex);
static void removeFromQueueAt(int queue[][MAX_QUEUE], int row, int pos);

void loanMenu(vector<Loan>& loans, vector<Book>& books,
              vector<Member>& members, vector<Fine>& fines) {
    int choice;

    do {
        printHeader("LOAN, RETURN & RESERVATION");
        cout << "  1. Issue Loan\n";
        cout << "  2. Renew Loan\n";
        cout << "  3. Return Book\n";
        cout << "  4. Place Reservation\n";
        cout << "  5. Cancel Reservation\n";
        cout << "  6. Display Active Loans\n";
        cout << "  7. Display Reservation Queue\n";
        cout << "  0. Back to Main Menu\n\n";

        choice = readInt("Enter choice: ", 0, 7);

        switch (choice) {
            case 1:
                issueLoan(loans, books, members);
                pressEnterToContinue();
                break;
            case 2:
                renewLoan(loans, books);
                pressEnterToContinue();
                break;
            case 3:
                returnBook(loans, books, members, fines);
                pressEnterToContinue();
                break;
            case 4:
                requestReservation(members, books);
                pressEnterToContinue();
                break;
            case 5:
                requestCancellation(members, books);
                pressEnterToContinue();
                break;
            case 6:
                displayActiveLoans(loans, books, members);
                pressEnterToContinue();
                break;
            case 7:
                displayReservationQueue(reserveQueue, books, members);
                pressEnterToContinue();
                break;
            case 0:
                break;
        }
    } while (choice != 0);
}

// ---- date helper ----

// The due date is the issue date plus the standard loan period.
Date calcDueDate(Date issueDate) {
    return addDays(issueDate, LOAN_DAYS);
}

// ---- queue helpers (local to this module) ----

// Position of a member inside one title's queue, or -1 when not queuing.
static int findQueuePosition(int queue[][MAX_QUEUE], int bookIndex, int memID) {
    if (bookIndex < 0 || bookIndex >= MAX_BOOKS) return -1;

    for (int c = 0; c < MAX_QUEUE; c++)
        if (queue[bookIndex][c] == memID) return c;

    return -1;
}

// First free slot of one title's queue, or -1 when the list is full.
static int firstFreeSlot(int queue[][MAX_QUEUE], int bookIndex) {
    if (bookIndex < 0 || bookIndex >= MAX_BOOKS) return -1;

    for (int c = 0; c < MAX_QUEUE; c++)
        if (queue[bookIndex][c] == EMPTY) return c;

    return -1;
}

// Closes the gap after removing the entry at 'pos' so the queue keeps
// its order with no holes in it.
static void removeFromQueueAt(int queue[][MAX_QUEUE], int row, int pos) {
    for (int c = pos; c < MAX_QUEUE - 1; c++)
        queue[row][c] = queue[row][c + 1];

    queue[row][MAX_QUEUE - 1] = EMPTY;   // last slot becomes free
}

// Appends a member at the end of a title's waiting list and returns true.
// Callers reject duplicate entries first; only a full list fails here.
bool placeReservation(int queue[][MAX_QUEUE], int bookIndex, int memID) {
    int slot = firstFreeSlot(queue, bookIndex);

    if (slot == -1) return false;

    queue[bookIndex][slot] = memID;
    return true;
}

// Removes one member from a title's waiting list. True when found.
bool cancelReservation(int queue[][MAX_QUEUE], int bookIndex, int memID) {
    int pos = findQueuePosition(queue, bookIndex, memID);

    if (pos == -1) return false;

    removeFromQueueAt(queue, bookIndex, pos);
    return true;
}

// Pops the front member off a title's waiting list and returns their ID,
// or EMPTY when nobody is waiting. Used by returnBook().
int promoteNextInQueue(int queue[][MAX_QUEUE], int bookIndex) {
    if (bookIndex < 0 || bookIndex >= MAX_BOOKS) return EMPTY;

    int nextMember = queue[bookIndex][0];

    if (nextMember != EMPTY)
        removeFromQueueAt(queue, bookIndex, 0);

    return nextMember;
}

// Offers to add this member to the waiting list after "all copies on loan".
static void joinQueueOffer(int bookIndex, int memID) {
    if (!confirmYesNo("  Place a reservation instead"))
        return;

    if (findQueuePosition(reserveQueue, bookIndex, memID) != -1) {
        cout << "\n  >> You are already in the queue for this title.\n";
        return;
    }

    int slot = firstFreeSlot(reserveQueue, bookIndex);

    if (slot == -1) {
        cout << "\n  >> The waiting list is full (" << MAX_QUEUE << " members).\n";
        return;
    }

    placeReservation(reserveQueue, bookIndex, memID);
    cout << "\n  Reservation placed. Position in queue: " << slot + 1 << '\n';
}

// Menu option 1 - issue a loan.
void issueLoan(vector<Loan>& loans, vector<Book>& books, vector<Member>& members) {
    printHeader("ISSUE LOAN");

    // while loops re-prompt until a valid ID (or 0 to cancel) is entered.
    int mi;
    while (true) {
        int id = readInt("  Member ID (0 to cancel): ");
        if (id == 0) return;

        mi = findMemberIndex(members, id);
        if (mi != -1) break;

        cout << "  >> Member Not Found.\n";
    }

    // Module 1 owns the eligibility rules. canBorrow() answers with a code
    // and borrowStatusMessage() turns that code into a readable reason.
    int eligibility = canBorrow(members[mi]);

    if (eligibility != BORROW_OK) {
        cout << "\n  >> Loan refused: " << borrowStatusMessage(eligibility) << '\n';
        return;
    }

    int bi;
    while (true) {
        int id = readInt("  Book ID (0 to cancel): ");
        if (id == 0) return;

        bi = findBookIndex(books, id);
        if (bi != -1) break;

        cout << "  >> Book Not Found.\n";
    }

    Book& b = books[bi];
    Member& m = members[mi];

    // ---- nobody may jump the reservation queue ----
    int frontMember = reserveQueue[bi][0];
    bool servedFromHold = false;

    if (frontMember != EMPTY && frontMember != m.memID) {
        cout << "\n  >> Member " << frontMember
             << " is first in the reservation queue for this title.\n";
        joinQueueOffer(bi, m.memID);
        return;
    }

    if (frontMember == m.memID) {
        // Their turn has come. Take a shelf copy first; if none is on the
        // shelf, consume the copy that was held aside for them.
        bool gotCopy = false;

        if (decrementAvailable(books, bi)) {
            gotCopy = true;
        } else if (b.reservedCopies > 0) {
            b.reservedCopies--;
            gotCopy = true;
            servedFromHold = true;
        }

        if (!gotCopy) {
            cout << "\n  >> No copy of this title is currently in the library.\n";
            return;
        }

        cancelReservation(reserveQueue, bi, m.memID);   // leave the queue
    } else {
        // Nobody is waiting: normal issue straight off the shelf.
        if (!decrementAvailable(books, bi)) {
            cout << "\n  >> All Copies On Loan.\n";
            joinQueueOffer(bi, m.memID);
            return;
        }
    }

    // ---- choose the issue date ----
    Date issueDate = systemDate;

    if (confirmYesNo("  Use a different issue date")) {
        while (true) {
            issueDate = readDate("  Issue date (YYYY-MM-DD): ");
            if (compareDate(issueDate, systemDate) >= 0) break;
            cout << "  >> Past-dated issue date is not allowed.\n";
        }
    }

    // ---- build the loan record ----
    Loan l;
    l.loanID     = nextLoanID(loans);
    l.memID      = m.memID;
    l.bookID     = b.bookID;
    l.issueDate  = issueDate;
    l.dueDate    = calcDueDate(issueDate);   // issue date + LOAN_DAYS
    l.returnDate = issueDate;                // ignored until the book returns
    l.renewCount = 0;
    l.status     = LOAN_ISSUED;

    loans.push_back(l);

    // ---- update the shared counters owned by other modules ----
    m.booksOnLoan++;

    if (bi < MAX_BOOKS)
        borrowCount[bi]++;                                   // Module 4 popularity report

    if (mi < MAX_MEMBERS && b.category >= 0 && b.category < MAX_CATEGORIES)
        memberLoanHistory[mi][b.category]++;                 // recommendation extra

    // ---- loan slip ----
    printRule();
    cout << "                    LOAN SLIP\n";
    printRule();
    cout << left << setw(16) << "  Loan ID"     << ": " << l.loanID << '\n';
    cout << left << setw(16) << "  Member"      << ": " << m.memID << " - " << m.name << '\n';
    cout << left << setw(16) << "  Title"       << ": " << b.title << '\n';
    cout << left << setw(16) << "  Issue Date"  << ": " << dateToString(l.issueDate) << '\n';
    cout << left << setw(16) << "  Due Date"    << ": " << dateToString(l.dueDate) << '\n';
    cout << left << setw(16) << "  Renewals"    << ": " << l.renewCount << " of max "
         << MAX_RENEW << '\n';

    if (servedFromHold)
        cout << "\n  This copy was held aside for you from the reservation queue.\n";

    printRule();
    cout << "  Fines are RM " << fixed << setprecision(2) << FINE_PER_DAY
         << " per day after the due date.\n";
}

// Menu option 2 - extend a loan by another full loan period.
void renewLoan(vector<Loan>& loans, const vector<Book>& books) {
    printHeader("RENEW LOAN");
    int loanID = readInt("  Loan ID (0 to cancel): ");

    if (loanID == 0) return;

    int li = findLoanIndex(loans, loanID);

    if (li == -1) {
        cout << "\n  >> Loan Record Not Found.\n";
        return;
    }

    Loan& l = loans[li];

    if (l.status != LOAN_ISSUED) {
        cout << "\n  >> This loan was already returned.\n";
        return;
    }

    // Rule 1 - every loan allows only a limited number of renewals.
    if (l.renewCount >= MAX_RENEW) {
        cout << "\n  >> Renewal refused. Maximum of " << MAX_RENEW
             << " renewals already used.\n";
        return;
    }

    // Rule 2 - an overdue loan must be returned and paid, not extended.
    if (isOverdue(l, systemDate)) {
        cout << "\n  >> Renewal refused. This loan is overdue by "
             << daysOverdue(l, systemDate) << " day(s).\n";
        return;
    }

    // Rule 3 - other members are waiting, so no renewal is fair.
    int bi = findBookIndexAny(books, l.bookID);

    if (bi != -1 && reserveQueue[bi][0] != EMPTY) {
        cout << "\n  >> Renewal refused. Other members are waiting for this title.\n";
        return;
    }

    l.renewCount++;
    l.dueDate = calcDueDate(l.dueDate);   // another full period from today's due date

    cout << "\n  Loan renewed (" << l.renewCount << " of max " << MAX_RENEW << ").\n";
    cout << "  New due date : " << dateToString(l.dueDate) << '\n';
}

// Menu option 3 - return a book, fine it if late, hand the copy over.
void returnBook(vector<Loan>& loans, vector<Book>& books,
                vector<Member>& members, vector<Fine>& fines) {
    printHeader("RETURN BOOK");
    int loanID = readInt("  Loan ID (0 to cancel): ");

    if (loanID == 0) return;

    int li = findLoanIndex(loans, loanID);

    if (li == -1) {
        cout << "\n  >> Loan Record Not Found.\n";
        return;
    }

    if (loans[li].status != LOAN_ISSUED) {
        cout << "\n  >> This loan has already been returned.\n";
        return;
    }

    Loan& l = loans[li];
    l.status     = LOAN_RETURNED;
    l.returnDate = systemDate;

    // Give the member back their borrowing allowance.
    int mi = findMemberIndexAny(members, l.memID);

    if (mi != -1 && members[mi].booksOnLoan > 0)
        members[mi].booksOnLoan--;

    string memberName = (mi == -1) ? "<unknown>" : members[mi].name;
    int bi = findBookIndexAny(books, l.bookID);
    string bookTitle  = (bi == -1) ? "<removed title>" : books[bi].title;

    cout << "\n  '" << bookTitle << "' returned by " << memberName << ".\n";

    // ---- fine handling: Module 4 reads and settles these records later ----
    int days = daysOverdue(l, systemDate);

    if (days > 0) {
        double base   = days * FINE_PER_DAY;
        double capped = (base > FINE_CAP) ? FINE_CAP : base;   // cap before tier rate

        double rate   = (mi == -1) ? RATE_STUDENT : getFineRate(members[mi].tier);
        double amount = round(capped * rate * 100.0) / 100.0;  // money in 2 dp

        if (amount <= 0.0) {
            cout << "  Returned " << days << " day(s) late, but this tier is exempt from fines.\n";
        } else {
            // One loan should never be fined twice.
            bool exists = false;
            for (int i = 0; i < (int)fines.size(); i++) {
                if (fines[i].loanID == l.loanID) { exists = true; break; }
            }

            if (exists) {
                cout << "  A fine record for this loan already exists.\n";
            } else {
                Fine f;
                f.fineID      = nextFineID(fines);
                f.loanID      = l.loanID;
                f.memID       = l.memID;
                f.daysOverdue = days;
                f.amount      = amount;
                f.paid        = 0.00;
                f.payDate     = systemDate;
                f.settled     = false;

                fines.push_back(f);

                // Keep the member's outstanding total in step with the records.
                if (mi != -1)
                    members[mi].outstandingFine = round((members[mi].outstandingFine + amount) * 100.0) / 100.0;

                cout << fixed << setprecision(2);
                cout << "\n  Days overdue     : " << days << '\n';
                cout << "  Base fine        : RM " << base << '\n';
                cout << "  Fine cap         : RM " << FINE_CAP << '\n';
                cout << "  Tier rate        : x" << rate << '\n';
                cout << "  Fine charged     : RM " << amount << '\n';
                cout << "  A fine record has been created. Please pay at the counter.\n";
            }
        }
    } else {
        cout << "  Returned on time. No fine.\n";
    }

    // ---- hand the copy over: reservation queue first, shelf second ----
    if (bi == -1) return;   // title was removed from the catalogue

    int nextMember = promoteNextInQueue(reserveQueue, bi);

    if (nextMember != EMPTY) {
        books[bi].reservedCopies++;   // hold the returned copy for them
        cout << "\n  Copy held for member " << nextMember
             << " - next in the reservation queue.\n";
    } else {
        incrementAvailable(books, bi);
    }
}

// Menu option 4 - join the waiting list of a fully-loaned title.
void requestReservation(const vector<Member>& members, vector<Book>& books) {
    printHeader("PLACE RESERVATION");

    int mi;
    while (true) {
        int id = readInt("  Member ID (0 to cancel): ");
        if (id == 0) return;

        mi = findMemberIndex(members, id);
        if (mi != -1) break;

        cout << "  >> Member Not Found.\n";
    }

    int bi;
    while (true) {
        int id = readInt("  Book ID (0 to cancel): ");
        if (id == 0) return;

        bi = findBookIndex(books, id);
        if (bi != -1) break;

        cout << "  >> Book Not Found.\n";
    }

    Book& b = books[bi];
    const Member& m = members[mi];
    if (b.availableCopies > 0) {
        cout << "\n  >> " << b.availableCopies
             << " copy/copies are on the shelf. Please borrow instead of reserving.\n";
        return;
    }

    // Duplicate reservation for the same title is rejected.
    if (findQueuePosition(reserveQueue, bi, m.memID) != -1) {
        cout << "\n  >> Duplicate reservation. You are already in the queue for this title.\n";
        return;
    }

    int slot = firstFreeSlot(reserveQueue, bi);

    if (slot == -1) {
        cout << "\n  >> The waiting list for this title is full (" << MAX_QUEUE << ").\n";
        return;
    }

    if (!confirmYesNo("  Join the waiting list at position " + to_string(slot + 1)))
        return;

    placeReservation(reserveQueue, bi, m.memID);
    cout << "\n  Reservation placed for '" << b.title << "'.\n";
    cout << "  You will be served as soon as a copy comes back.\n";
}

// Menu option 5 - leave a waiting list.
void requestCancellation(const vector<Member>& members, vector<Book>& books) {
    printHeader("CANCEL RESERVATION");

    int mi;
    while (true) {
        int id = readInt("  Member ID (0 to cancel): ");
        if (id == 0) return;

        mi = findMemberIndex(members, id);
        if (mi != -1) break;

        cout << "  >> Member Not Found.\n";
    }

    int bi;
    while (true) {
        int id = readInt("  Book ID (0 to cancel): ");
        if (id == 0) return;

        bi = findBookIndex(books, id);
        if (bi != -1) break;

        cout << "  >> Book Not Found.\n";
    }

    int pos = findQueuePosition(reserveQueue, bi, members[mi].memID);

    if (pos == -1) {
        cout << "\n  >> No reservation found for this title.\n";
        return;
    }

    // If they were first AND a returned copy was being held for them,
    // release that copy back to the shelf before they leave the queue.
    if (pos == 0 && books[bi].reservedCopies > 0) {
        books[bi].reservedCopies--;
        incrementAvailable(books, bi);
        cout << "\n  Held copy released back to the shelf.\n";
    }

    cancelReservation(reserveQueue, bi, members[mi].memID);
    cout << "  Reservation cancelled.\n";
}

// Menu option 6 - all loans still out, flagging the overdue ones.
void displayActiveLoans(const vector<Loan>& loans, const vector<Book>& books,
                        const vector<Member>& members) {
    printHeader("ACTIVE LOANS");

    cout << left
         << setw(8)  << "LoanID"
         << setw(8)  << "MemID"
         << setw(20) << "Member"
         << setw(8)  << "BookID"
         << setw(26) << "Title"
         << setw(12) << "Due Date"
         << right
         << setw(14) << "Status"
         << '\n';
    printRule();

    int count = 0;
    int overdueCount = 0;

    for (int i = 0; i < (int)loans.size(); i++) {
        if (loans[i].status != LOAN_ISSUED) continue;

        int mi = findMemberIndexAny(members, loans[i].memID);
        int bi = findBookIndexAny(books, loans[i].bookID);

        string memberName = (mi == -1) ? "<unknown>" : members[mi].name;
        string bookTitle  = (bi == -1) ? "<removed>"  : books[bi].title;

        int late = daysOverdue(loans[i], systemDate);
        string status = (late > 0) ? "OVERDUE " + to_string(late) + " d" : "ON LOAN";

        cout << left
             << setw(8)  << loans[i].loanID
             << setw(8)  << loans[i].memID
             << setw(20) << memberName.substr(0, 19)
             << setw(8)  << loans[i].bookID
             << setw(26) << bookTitle.substr(0, 25)
             << setw(12) << dateToString(loans[i].dueDate)
             << right
             << setw(14) << status
             << '\n';

        count++;
        if (late > 0) overdueCount++;
    }

    printRule();
    cout << "  Active loans: " << count << "   Overdue: " << overdueCount << '\n';
}

// Menu option 7 - who is waiting for which title, printed with nested loops.
void displayReservationQueue(int queue[][MAX_QUEUE], const vector<Book>& books,
                             const vector<Member>& members) {
    printHeader("RESERVATION QUEUE");

    int titlesShown = 0;

    for (int bIdx = 0; bIdx < (int)books.size(); bIdx++) {   // outer loop: titles
        if (!books[bIdx].active) continue;
        if (queue[bIdx][0] == EMPTY) continue;               // nobody waiting

        cout << "\n  [" << books[bIdx].bookID << "] " << books[bIdx].title << '\n';

        for (int c = 0; c < MAX_QUEUE; c++) {                // inner loop: queue order
            if (queue[bIdx][c] == EMPTY) break;

            int mi = findMemberIndexAny(members, queue[bIdx][c]);
            string name = (mi == -1) ? "<unknown>" : members[mi].name;

            cout << "     " << (c + 1) << ". "
                 << queue[bIdx][c] << "  " << name;

            if (c == 0 && books[bIdx].reservedCopies > 0)
                cout << "   <- copy held, collect at counter";

            cout << '\n';
        }

        titlesShown++;
    }

    if (titlesShown == 0)
        cout << "\n  No reservations in any queue.\n";
}
