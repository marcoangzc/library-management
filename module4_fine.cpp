#include "common.h"
#include <cmath>

// Module 4 function prototypes
double money2dp(double value);

int findFineIndexByLoan(const vector<Fine>& fines, int loanID);

void recalcMemberOutstanding(vector<Member>& members,
                             const vector<Fine>& fines,
                             int memID);

bool processPayment(double amountDue,
                    double amountPaid,
                    double& change,
                    double& balance);

void payFineFunction(vector<Fine>& fines,
                     vector<Member>& members,
                     const vector<Loan>& loans,
                     const vector<Book>& books);

void calculateOverdueFine(vector<Fine>& fines,
                          vector<Loan>& loans,
                          vector<Member>& members,
                          const vector<Book>& books);

void receiptFunction(const vector<Fine>& fines,
                     const vector<Member>& members,
                     const vector<Loan>& loans,
                     const vector<Book>& books);

void fineCollectionReport(const vector<Fine>& fines,
                          const vector<Member>& members);

void mostBorrowedReport(const vector<Book>& books,
                        const int borrowCnt[]);

void overdueMemberReport(const vector<Member>& members,
                         const vector<Loan>& loans,
                         const vector<Book>& books);

void monthlyStatisticsReport(const double stats[][MAX_CATEGORIES]);
//main menu

// This function is called from main() to run the Fine module.
void fineMenu(vector<Fine>& fines, vector<Loan>& loans,
              vector<Member>& members, vector<Book>& books) {
    int choice;

    do {
        printHeader("FINE PAYMENT & REPORTING");

        cout << "  1. Calculate Overdue Fine\n";
        cout << "  2. Process Payment\n";
        cout << "  3. Print Receipt\n";
        cout << "  4. Fine Collection Report\n";
        cout << "  5. Most Borrowed Title Report\n";
        cout << "  6. Overdue Member Report\n";
        cout << "  7. Monthly Statistics Report\n";
        cout << "  0. Back to Main Menu\n\n";

        choice = readInt("Enter choice: ", 0, 7);

        switch (choice) {
            case 1:
                calculateOverdueFine(fines, loans, members, books);
                break;

            case 2:
                payFineFunction(fines, members, loans, books);
                break;

            case 3:
                receiptFunction(fines, members, loans, books);
                break;

            case 4:
                fineCollectionReport(fines, members);
                break;

            case 5:
                mostBorrowedReport(books, borrowCount);
                break;

            case 6:
                overdueMemberReport(members, loans, books);
                break;

            case 7:
                monthlyStatisticsReport(monthlyStats);
                break;

            case 0:
                break;
        }
    } while (choice != 0);
}

//helper functions for menu 1
// Rounds a money value to 2 decimal places
// A double cannot store 0.30 exactly, so after several part
// payments a fully-paid fine could be left with a balance of
// 0.0000000001 and would never be marked as settled.
double money2dp(double value) {
    return round(value * 100.0) / 100.0;
}

// Helper function for menu 1
// find fine index by loanID
// One loan should normally have only one fine record.
int findFineIndexByLoan(const vector<Fine>& fines, int loanID) {
    for (int i = 0; i < (int)fines.size(); i++) {
        if (fines[i].loanID == loanID)
            return i;
    }

    return -1;
}

// helper function for menu 2 to recalc the member's outstanding fine whenever a fine is added, updated or settled.
// Module 1 can never drift apart from the Fine records owned here/
void recalcMemberOutstanding(vector<Member>& members,
                             const vector<Fine>& fines,
                             int memID) {
    int mi = findMemberIndexAny(members, memID);

    if (mi == -1)
        return;

    double total = 0.0;

    for (int i = 0; i < (int)fines.size(); i++) {
        if (fines[i].memID == memID && !fines[i].settled) {
            total += fineBalance(fines[i]);
        }
    }

    members[mi].outstandingFine = money2dp(total);
}

// helper function for menu2
// calculate payment result only
// Works out the result of ONE payment transaction. It performs no
// input or output at all - it is a pure calculation, so it can be
// tested on its own and reused by any caller.
//
//   amountDue  - pass-by-value     : balance still owed on the fine
//   amountPaid - pass-by-value     : cash tendered by the member
//   change     - pass-by-reference : receives the change to return
//   balance    - pass-by-reference : receives the amount still owed
//
// Returns true when the payment is acceptable, false otherwise.
// A function can only return one value, but this operation must
// send back three (status, change, balance), so change and balance
// are returned through reference parameters instead.

bool processPayment(double amountDue, double amountPaid,
                    double& change, double& balance) {
    change  = 0.0;
    balance = money2dp(amountDue);

    // Zero or negative payment is not allowed.
    if (amountPaid <= 0.0)
        return false;

    // Nothing to pay.
    if (amountDue <= 0.0)
        return false;

    // Full settlement or overpayment.
    if (amountPaid >= amountDue) {
        change  = money2dp(amountPaid - amountDue);
        balance = 0.0;
    }
    // Part payment.
    else {
        change  = 0.0;
        balance = money2dp(amountDue - amountPaid);
    }

    return true;
}

// Menu option 1 - Calculate overdue fine and optionally create a fine record for it.
// This function only calculates and displays the fine.
// It does not create a Fine record.
void calculateOverdueFine(vector<Fine>& fines,
                          vector<Loan>& loans,
                          vector<Member>& members,
                          const vector<Book>& books) {
    printHeader("CALCULATE OVERDUE FINE");

    cout << left
         << setw(8)  << "LoanID"
         << setw(8)  << "MemID"
         << setw(22) << "Member"
         << setw(8)  << "BookID"
         << setw(26) << "Title"
         << setw(12) << "Due Date"
         << setw(6)  << "Days"
         << setw(14) << "Fine Status"
         << '\n';

    printRule();

    int count = 0;

    for (int i = 0; i < (int)loans.size(); i++) {
        int days = daysOverdue(loans[i], systemDate);

        if (days <= 0)
            continue;

        int mi = findMemberIndexAny(members, loans[i].memID);
        int bi = findBookIndexAny(books, loans[i].bookID);

        string memberName = (mi == -1) ? "<unknown>" : members[mi].name;
        string bookTitle  = (bi == -1) ? "<deleted>" : books[bi].title;

        int existingFineIndex = findFineIndexByLoan(fines, loans[i].loanID);

        string fineStatus;

        if (existingFineIndex == -1)
            fineStatus = "No fine";
        else if (fines[existingFineIndex].settled)
            fineStatus = "Settled";
        else
            fineStatus = "Unpaid";

        cout << left
             << setw(8)  << loans[i].loanID
             << setw(8)  << loans[i].memID
             << setw(22) << memberName.substr(0, 21)
             << setw(8)  << loans[i].bookID
             << setw(26) << bookTitle.substr(0, 25)
             << setw(12) << dateToString(loans[i].dueDate)
             << setw(6)  << days
             << setw(14) << fineStatus
             << '\n';

        count++;
    }

    printRule();

    if (count == 0) {
        cout << "\n  No overdue or late-return loans found.\n";
        pressEnterToContinue();
        return;
    }

    int loanID = readInt("\nEnter Loan ID to calculate fine (0 to cancel): ");

    if (loanID == 0)
        return;

    int li = findLoanIndex(loans, loanID);

    if (li == -1) {
        cout << "\n  Loan record not found.";
        pressEnterToContinue();
        return;
    }

    int days = daysOverdue(loans[li], systemDate);

    if (days <= 0) {
        cout << "\n  This loan is not overdue; no fine is required.";
        pressEnterToContinue();
        return;
    }

    int mi = findMemberIndexAny(members, loans[li].memID);
    int bi = findBookIndexAny(books, loans[li].bookID);

    double baseFine   = days * FINE_PER_DAY;
    double cappedFine = (baseFine > FINE_CAP) ? FINE_CAP : baseFine;
    double rate       = (mi == -1) ? RATE_STUDENT : getFineRate(members[mi].tier);
    double finalFine  = money2dp(cappedFine * rate);

    cout << "\n";
    printRule();
    cout << "  OVERDUE FINE CALCULATION\n";
    printRule();

    cout << left << setw(22) << "  Loan ID"
         << " : " << loans[li].loanID << '\n';

    cout << left << setw(22) << "  Member"
         << " : " << ((mi == -1) ? "<unknown>" : members[mi].name) << '\n';

    cout << left << setw(22) << "  Membership Tier"
         << " : " << ((mi == -1) ? "Unknown" : getTierName(members[mi].tier)) << '\n';

    cout << left << setw(22) << "  Book"
         << " : " << ((bi == -1) ? "<deleted>" : books[bi].title) << '\n';

    cout << left << setw(22) << "  Due Date"
         << " : " << dateToString(loans[li].dueDate) << '\n';

    if (loans[li].status == LOAN_RETURNED) {
        cout << left << setw(22) << "  Return Date"
             << " : " << dateToString(loans[li].returnDate) << '\n';
    }

    cout << left << setw(22) << "  Days Overdue"
         << " : " << days << '\n';

    cout << fixed << setprecision(2);

    cout << left << setw(22) << "  Base Fine"
         << " : RM " << baseFine << '\n';

    cout << left << setw(22) << "  Fine Cap"
         << " : RM " << FINE_CAP << '\n';

    cout << left << setw(22) << "  Capped Fine"
         << " : RM " << cappedFine << '\n';

    cout << left << setw(22) << "  Tier Rate"
         << " : " << rate << '\n';

    cout << left << setw(22) << "  Final Fine"
         << " : RM " << finalFine << '\n';

    printRule();

    // If this loan already has a fine record, do not create another one.
    int existingFineIndex = findFineIndexByLoan(fines, loanID);

    if (existingFineIndex != -1) {
        cout << "\n  A fine record already exists for this loan.\n";
        cout << "  Fine ID : " << fines[existingFineIndex].fineID << '\n';
        cout << "  Amount  : RM " << fines[existingFineIndex].amount << '\n';
        cout << "  Paid    : RM " << fines[existingFineIndex].paid << '\n';
        cout << "  Balance : RM " << fineBalance(fines[existingFineIndex]) << '\n';

        if (fines[existingFineIndex].settled)
            cout << "  Status  : SETTLED\n";
        else
            cout << "  Status  : UNPAID / PART PAID\n";

        pressEnterToContinue();
        return;
    }

    // Premium members may have zero fine.
    if (finalFine <= 0.0) {
        cout << "\n  No fine is required for this member tier.\n";
        pressEnterToContinue();
        return;
    }

    // Optionally create a Fine record.
    if (confirmYesNo("Create fine record for this loan?")) {
        Fine f;

        f.fineID      = nextFineID(fines);
        f.loanID      = loans[li].loanID;
        f.memID       = loans[li].memID;
        f.daysOverdue = days;
        f.amount      = finalFine;
        f.paid        = 0.00;
        f.payDate     = systemDate;
        f.settled     = false;

        fines.push_back(f);

        recalcMemberOutstanding(members, fines, f.memID);

        cout << "\n  Fine record created successfully.\n";
        cout << "  Fine ID : " << f.fineID << '\n';
        cout << "  Amount  : RM " << f.amount << '\n';
    } else {
        cout << "\n  Fine record was not created.\n";
    }

    pressEnterToContinue();
}

// Menu option 2 - accepts a payment against an unsettled fine.
// This function does all the screen work and all the updating of
// shared data; the arithmetic itself is delegated to
// processPayment() above.
void payFineFunction(vector<Fine>& fines,
                     vector<Member>& members,
                     const vector<Loan>& loans,
                     const vector<Book>& books) {
    printHeader("PROCESS FINE PAYMENT");

    cout << left
         << setw(8)  << "FineID"
         << setw(8)  << "MemID"
         << setw(24) << "Member Name"
         << setw(6)  << "Days"
         << right
         << setw(10) << "Amount"
         << setw(10) << "Paid"
         << setw(10) << "Balance"
         << '\n';

    printRule();

    int unsettledCount = 0;

    for (int i = 0; i < (int)fines.size(); i++) {
        if (fines[i].settled)
            continue;

        int mi = findMemberIndexAny(members, fines[i].memID);
        string name = (mi == -1) ? "<unknown>" : members[mi].name;

        cout << left
             << setw(8)  << fines[i].fineID
             << setw(8)  << fines[i].memID
             << setw(24) << name.substr(0, 23)
             << setw(6)  << fines[i].daysOverdue
             << right
             << fixed << setprecision(2)
             << setw(10) << fines[i].amount
             << setw(10) << fines[i].paid
             << setw(10) << fineBalance(fines[i])
             << '\n';

        unsettledCount++;
    }

    printRule();

    if (unsettledCount == 0) {
        cout << "\n  There are no outstanding fines to pay.\n";
        pressEnterToContinue();
        return;
    }

    int fineID = readInt("\nEnter Fine ID to pay (0 to cancel): ");

    if (fineID == 0)
        return;

    int fi = -1;

    for (int i = 0; i < (int)fines.size(); i++) {
        if (fines[i].fineID == fineID) {
            fi = i;
            break;
        }
    }

    if (fi == -1) {
        cout << "\n  Fine record not found.";
        pressEnterToContinue();
        return;
    }

    if (fines[fi].settled) {
        cout << "\n  This fine has already been settled.";
        pressEnterToContinue();
        return;
    }

    double due = money2dp(fineBalance(fines[fi]));

    cout << "\n  Amount due : RM " << fixed << setprecision(2) << due << "\n";

    if (due <= 0.0) {
        fines[fi].settled = true;
        fines[fi].payDate = systemDate;

        recalcMemberOutstanding(members, fines, fines[fi].memID);

        cout << "\n  This fine has no remaining balance and is now marked SETTLED.\n";
        pressEnterToContinue();
        return;
    }

    double amountPaid = 0.0;
    double change = 0.0;
    double balance = 0.0;
    bool accepted = false;

    while (!accepted) {
        amountPaid = readDouble("Amount tendered (RM): ", 0.0, 9999.99);

        accepted = processPayment(due, amountPaid, change, balance);

        if (!accepted) {
            cout << "  Invalid payment amount. Payment must be more than RM 0.00.\n";
        }
    }

    double credited = money2dp(amountPaid - change);

    fines[fi].paid    = money2dp(fines[fi].paid + credited);
    fines[fi].payDate = systemDate;
    fines[fi].settled = (balance == 0.0);

    recalcMemberOutstanding(members, fines, fines[fi].memID);

    int category = -1;
    int li = findLoanIndex(loans, fines[fi].loanID);

    if (li != -1) {
        int bi = findBookIndexAny(books, loans[li].bookID);

        if (bi != -1)
            category = books[bi].category;
    }

    int monthIndex = fines[fi].payDate.month - 1;

    if (category >= 0 && category < MAX_CATEGORIES &&
        monthIndex >= 0 && monthIndex < MONTHS) {
        monthlyStats[monthIndex][category] += credited;
    }

    cout << "\n  Payment recorded.\n";
    cout << fixed << setprecision(2);
    cout << "  Amount paid : RM " << setw(8) << credited << '\n';
    cout << "  Change      : RM " << setw(8) << change << '\n';
    cout << "  Balance     : RM " << setw(8) << balance << '\n';

    if (fines[fi].settled)
        cout << "  Status      : SETTLED\n";
    else
        cout << "  Status      : PART PAYMENT - balance carried forward\n";

    pressEnterToContinue();
}


// Menu option 3 - Print receipt for a fine record.
void receiptFunction(const vector<Fine>& fines,
                     const vector<Member>& members,
                     const vector<Loan>& loans,
                     const vector<Book>& books) {
    printHeader("PAYMENT RECEIPT");

    if (fines.empty()) {
        cout << "\n  There are no fine records.\n";
        pressEnterToContinue();
        return;
    }

    cout << left
         << setw(8)  << "FineID"
         << setw(8)  << "LoanID"
         << setw(8)  << "MemID"
         << setw(22) << "Member"
         << right
         << setw(10) << "Amount"
         << setw(10) << "Paid"
         << setw(10) << "Balance"
         << "   "
         << left
         << setw(15) << "Status"
         << '\n';

    printRule();

    for (int i = 0; i < (int)fines.size(); i++) {
        int mi = findMemberIndexAny(members, fines[i].memID);
        string memberName = (mi == -1) ? "<unknown>" : members[mi].name;

        string status;

        if (fines[i].settled || fineBalance(fines[i]) <= 0.0)
            status = "SETTLED";
        else if (fines[i].paid > 0.0)
            status = "PART PAID";
        else
            status = "UNPAID";

        cout << left
             << setw(8)  << fines[i].fineID
             << setw(8)  << fines[i].loanID
             << setw(8)  << fines[i].memID
             << setw(22) << memberName.substr(0, 21)
             << right
             << fixed << setprecision(2)
             << setw(10) << fines[i].amount
             << setw(10) << fines[i].paid
             << setw(10) << fineBalance(fines[i])
             << "   "
             << left
             << setw(15) << status
             << '\n';
    }

    printRule();

    int fineID = readInt("\nEnter Fine ID to print receipt (0 to cancel): ");

    if (fineID == 0)
        return;

    int fi = -1;

    for (int i = 0; i < (int)fines.size(); i++) {
        if (fines[i].fineID == fineID) {
            fi = i;
            break;
        }
    }

    if (fi == -1) {
        cout << "\n  Fine record not found.";
        pressEnterToContinue();
        return;
    }

    int mi = findMemberIndexAny(members, fines[fi].memID);
    int li = findLoanIndex(loans, fines[fi].loanID);
    int bi = -1;

    if (li != -1)
        bi = findBookIndexAny(books, loans[li].bookID);

    string memberName = (mi == -1) ? "<unknown>" : members[mi].name;
    string tierName   = (mi == -1) ? "Unknown" : getTierName(members[mi].tier);
    double rate       = (mi == -1) ? RATE_STUDENT : getFineRate(members[mi].tier);
    string bookTitle  = (bi == -1) ? "<deleted>" : books[bi].title;

    string status;

    if (fines[fi].settled || fineBalance(fines[fi]) <= 0.0)
        status = "SETTLED";
    else if (fines[fi].paid > 0.0)
        status = "PART PAYMENT";
    else
        status = "UNPAID";

    cout << "\n";
    printRule('*');
    cout << "                   LIBRARY FINE RECEIPT\n";
    printRule('*');

    cout << fixed << setprecision(2);

    cout << left << setw(20) << "Receipt Date"
         << " : " << dateToString(fines[fi].payDate) << '\n';

    cout << left << setw(20) << "Fine ID"
         << " : " << fines[fi].fineID << '\n';

    cout << left << setw(20) << "Loan ID"
         << " : " << fines[fi].loanID << '\n';

    cout << left << setw(20) << "Member ID"
         << " : " << fines[fi].memID << '\n';

    cout << left << setw(20) << "Member Name"
         << " : " << memberName << '\n';

    cout << left << setw(20) << "Membership Tier"
         << " : " << tierName << '\n';

    cout << left << setw(20) << "Book Title"
         << " : " << bookTitle << '\n';

    cout << left << setw(20) << "Days Overdue"
         << " : " << fines[fi].daysOverdue << '\n';

    cout << left << setw(20) << "Fine Rate"
         << " : " << rate << '\n';

    cout << left << setw(20) << "Fine Amount"
         << " : RM " << fines[fi].amount << '\n';

    cout << left << setw(20) << "Amount Paid"
         << " : RM " << fines[fi].paid << '\n';

    cout << left << setw(20) << "Balance"
         << " : RM " << fineBalance(fines[fi]) << '\n';

    cout << left << setw(20) << "Status"
         << " : " << status << '\n';

    printRule('*');

    pressEnterToContinue();
}

// Menu option 4 - Fine collection report.
void fineCollectionReport(const vector<Fine>& fines,
                          const vector<Member>& members) {
    printHeader("FINE COLLECTION REPORT");

    if (fines.empty()) {
        cout << "\n  There are no fine records.\n";
        pressEnterToContinue();
        return;
    }

    cout << left
         << setw(8)  << "FineID"
         << setw(8)  << "LoanID"
         << setw(8)  << "MemID"
         << setw(22) << "Member"
         << right
         << setw(6)  << "Days"
         << setw(10) << "Amount"
         << setw(10) << "Paid"
         << setw(10) << "Balance"
         << "   "
         << left
         << setw(12) << "Status"
         << '\n';

    printRule();

    double totalAmount      = 0.0;
    double totalCollected   = 0.0;
    double totalOutstanding = 0.0;

    int settledCount   = 0;
    int unsettledCount = 0;

    for (int i = 0; i < (int)fines.size(); i++) {
        int mi = findMemberIndexAny(members, fines[i].memID);
        string memberName = (mi == -1) ? "<unknown>" : members[mi].name;

        double balance = fineBalance(fines[i]);

        string status;

        if (fines[i].settled || balance <= 0.0) {
            status = "SETTLED";
            settledCount++;
        } else {
            if (fines[i].paid > 0.0)
                status = "PART PAID";
            else
                status = "UNPAID";

            unsettledCount++;
        }

        cout << left
             << setw(8)  << fines[i].fineID
             << setw(8)  << fines[i].loanID
             << setw(8)  << fines[i].memID
             << setw(22) << memberName.substr(0, 21)
             << right
             << fixed << setprecision(2)
             << setw(6)  << fines[i].daysOverdue
             << setw(10) << fines[i].amount
             << setw(10) << fines[i].paid
             << setw(10) << balance
             << "   "
             << left
             << setw(12) << status
             << '\n';

        totalAmount      += fines[i].amount;
        totalCollected   += fines[i].paid;
        totalOutstanding += balance;
    }

    printRule();

    cout << fixed << setprecision(2);

    cout << "\n  Total fine records        : " << fines.size() << '\n';
    cout << "  Settled records           : " << settledCount << '\n';
    cout << "  Unsettled records         : " << unsettledCount << '\n';

    cout << "\n  Total fine amount         : RM " << totalAmount << '\n';
    cout << "  Total collected           : RM " << totalCollected << '\n';
    cout << "  Total outstanding balance : RM " << totalOutstanding << '\n';

    pressEnterToContinue();
}

// Menu option 5 - Most borrowed title report.
// borrowCnt is the global borrowCount array.
void mostBorrowedReport(const vector<Book>& books,
                        const int borrowCnt[]) {
    printHeader("MOST BORROWED TITLE REPORT");

    int n = (int)books.size();

    if (n > MAX_BOOKS)
        n = MAX_BOOKS;

    if (n == 0) {
        cout << "\n  There are no books in the catalogue.\n";
        pressEnterToContinue();
        return;
    }

    cout << left
         << setw(6)  << "Rank"
         << setw(8)  << "BookID"
         << setw(35) << "Title"
         << setw(15) << "Category"
         << right
         << setw(10) << "Borrows"
         << '\n';

    printRule();

    vector<int> used(n, 0);

    int limit = (n < 5) ? n : 5;

    for (int rank = 1; rank <= limit; rank++) {
        int best = -1;

        for (int i = 0; i < n; i++) {
            if (used[i])
                continue;

            if (best == -1 || borrowCnt[i] > borrowCnt[best])
                best = i;
        }

        if (best == -1)
            break;

        used[best] = 1;

        string title = books[best].title;

        if (!books[best].active)
            title += " *";

        string categoryName = "Unknown";

        if (books[best].category >= 0 && books[best].category < MAX_CATEGORIES)
            categoryName = CATEGORY_NAME[books[best].category];

        cout << left
             << setw(6)  << rank
             << setw(8)  << books[best].bookID
             << setw(35) << title.substr(0, 34)
             << setw(15) << categoryName.substr(0, 14)
             << right
             << setw(10) << borrowCnt[best]
             << '\n';
    }

    printRule();

    cout << "\n  * Soft-deleted title, shown for borrowing history.\n";

    pressEnterToContinue();
}

// Menu option 6 - Overdue member report.
void overdueMemberReport(const vector<Member>& members,
                         const vector<Loan>& loans,
                         const vector<Book>& books) {
    printHeader("OVERDUE MEMBER REPORT");

    int totalOverdueMembers = 0;
    int totalOverdueLoans   = 0;

    for (int i = 0; i < (int)members.size(); i++) {
        if (!members[i].active)
            continue;

        vector<int> overdueLoanIndexes;

        for (int j = 0; j < (int)loans.size(); j++) {
            if (loans[j].memID == members[i].memID &&
                isOverdue(loans[j], systemDate)) {
                overdueLoanIndexes.push_back(j);
            }
        }

        if (overdueLoanIndexes.empty())
            continue;

        totalOverdueMembers++;

        cout << "\n";
        printRule();

        cout << "  Member ID          : " << members[i].memID << '\n';
        cout << "  Member Name        : " << members[i].name << '\n';
        cout << "  Membership Tier    : " << getTierName(members[i].tier) << '\n';
        cout << fixed << setprecision(2);
        cout << "  Outstanding Fine   : RM " << members[i].outstandingFine << '\n';

        cout << '\n';

        cout << left
             << setw(8)  << "LoanID"
             << setw(8)  << "BookID"
             << setw(32) << "Title"
             << setw(12) << "Due Date"
             << right
             << setw(8)  << "Days"
             << '\n';

        printRule();

        for (int k = 0; k < (int)overdueLoanIndexes.size(); k++) {
            int li = overdueLoanIndexes[k];

            int bi = findBookIndexAny(books, loans[li].bookID);

            string bookTitle = (bi == -1) ? "<deleted>" : books[bi].title;

            int days = daysOverdue(loans[li], systemDate);

            cout << left
                 << setw(8)  << loans[li].loanID
                 << setw(8)  << loans[li].bookID
                 << setw(32) << bookTitle.substr(0, 31)
                 << setw(12) << dateToString(loans[li].dueDate)
                 << right
                 << setw(8)  << days
                 << '\n';

            totalOverdueLoans++;
        }
    }

    printRule();

    if (totalOverdueMembers == 0) {
        cout << "\n  No members currently have overdue loans.\n";
    } else {
        cout << "\n  Total members with overdue loans : " << totalOverdueMembers << '\n';
        cout << "  Total overdue loans              : " << totalOverdueLoans << '\n';
    }

    pressEnterToContinue();
}

// Menu option 7 - Monthly statistics report.
void monthlyStatisticsReport(const double stats[][MAX_CATEGORIES]) {
    printHeader("MONTHLY STATISTICS REPORT");

    int month = readInt("Enter month (1-12): ", 1, 12);

    string monthName[MONTHS] = {
        "January", "February", "March",
        "April",   "May",      "June",
        "July",    "August",   "September",
        "October", "November", "December"
    };

    cout << "\n  Fine collection statistics for "
         << monthName[month - 1]
         << "\n\n";

    cout << left
         << setw(20) << "Category"
         << right
         << setw(18) << "Amount Collected"
         << '\n';

    printRule();

    double total = 0.0;

    cout << fixed << setprecision(2);

    for (int c = 0; c < MAX_CATEGORIES; c++) {
        double amount = stats[month - 1][c];

        cout << left
             << setw(20) << CATEGORY_NAME[c]
             << right
             << setw(18) << amount
             << '\n';

        total += amount;
    }

    printRule();

    cout << left
         << setw(20) << "Total"
         << right
         << setw(18) << total
         << '\n';

    pressEnterToContinue();
}