#include "common.h"
#include <cmath>

// Module 4 - Fine Payment Processing & Reporting - ang zi chen

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
                viewOverdueFine(loans, members, books);
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

void viewOverdueFine(const vector<Loan>& loans,
                     const vector<Member>& members,
                     const vector<Book>& books);
void receiptFunction(const vector<Fine>& fines, const vector<Member>& members,
                     const vector<Loan>& loans, const vector<Book>& books);
void fineCollectionReport(const vector<Fine>& fines,
                          const vector<Member>& members);
void mostBorrowedReport(const vector<Book>& books, const int borrowCnt[]);
void overdueMemberReport(const vector<Member>& members,
                         const vector<Loan>& loans,
                         const vector<Book>& books);
void monthlyStatisticsReport(const double stats[][MAX_CATEGORIES]);
// Rounds a money value to 2 decimal places
// A double cannot store 0.30 exactly, so after several part
// payments a fully-paid fine could be left with a balance of
// 0.0000000001 and would never be marked as settled.
double money2dp(double value) {
    return round(value * 100.0) / 100.0;
}

// Rebuilds a member's outstandingFine from the fine records.
// The total is never adjusted by subtracting from the old value;
// it is recalculated from scratch so the Member record owned by
// Module 1 can never drift apart from the Fine records owned here.
void recalcMemberOutstanding(vector<Member>& members,
                             const vector<Fine>& fines, int memID) {
    int mi = findMemberIndexAny(members, memID);
    if (mi == -1) return;

    double total = 0.0;
    for (int i = 0; i < (int)fines.size(); i++) {
        if (fines[i].memID == memID && !fines[i].settled) {
            total += fineBalance(fines[i]);
        }
    }
    members[mi].outstandingFine = money2dp(total);
}

// processPayment
//
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
    balance = money2dp(amountDue);      // unchanged if we reject

    if (amountPaid <= 0.0) return false;   // zero or negative
    if (amountDue  <= 0.0) return false;   // nothing owed

    if (amountPaid >= amountDue) {         // full settlement
        change  = money2dp(amountPaid - amountDue);
        balance = 0.0;                     // set exactly, not computed
    } else {                               // part payment
        change  = 0.0;
        balance = money2dp(amountDue - amountPaid);
    }
    return true;
}

// Menu option 2 - accepts a payment against an unsettled fine.
// This function does all the screen work and all the updating of
// shared data; the arithmetic itself is delegated to
// processPayment() above.

void payFineFunction(vector<Fine>& fines, vector<Member>& members,
                     const vector<Loan>& loans, const vector<Book>& books) {
    printHeader("PROCESS FINE PAYMENT");

    //1. List every fine that is still unsettled
    cout << left  << setw(8)  << "FineID" << setw(8) << "MemID"
         << setw(24) << "Member Name"     << setw(6) << "Days"
         << right << setw(10) << "Amount" << setw(10) << "Paid"
         << setw(10) << "Balance" << '\n';
    printRule();

    int unsettledCount = 0;
    for (int i = 0; i < (int)fines.size(); i++) {
        if (fines[i].settled) continue;

        int mi = findMemberIndexAny(members, fines[i].memID);
        string name = (mi == -1) ? "<unknown>" : members[mi].name;

        cout << left  << setw(8) << fines[i].fineID
             << setw(8) << fines[i].memID
             << setw(24) << name.substr(0, 23)
             << setw(6)  << fines[i].daysOverdue
             << right << fixed << setprecision(2)
             << setw(10) << fines[i].amount
             << setw(10) << fines[i].paid
             << setw(10) << fineBalance(fines[i]) << '\n';
        unsettledCount++;
    }
    printRule();

    if (unsettledCount == 0) {
        cout << "\n  There are no outstanding fines to pay.";
        pressEnterToContinue();
        return;
    }

    //2. Locate the chosen fine 
    int fineID = readInt("\n  Enter Fine ID to pay (0 to cancel): ");
    if (fineID == 0) return;

    int fi = -1;
    for (int i = 0; i < (int)fines.size(); i++) {
        if (fines[i].fineID == fineID) { fi = i; break; }
    }

    if (fi == -1) {
        cout << "\n  Fine Record Not Found.";
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

    // 3. Re-prompt until the amount is accepted
    // readDouble() blocks anything outside the range; the while loop
    // enforces the business rule that 0.00 is not a payment.
    double amountPaid = 0.0, change = 0.0, balance = 0.0;
    bool accepted = false;

    while (!accepted) {
        amountPaid = readDouble("  Amount tendered (RM): ", 0.0, 9999.99);
        accepted = processPayment(due, amountPaid, change, balance);
        if (!accepted) {
            cout << "  Invalid Payment Amount - must be more than 0.00.\n";
        }
    }

    //4. Write the results back into the shared data 
    // Only the amount actually kept is credited: if the member
    // over-tenders, the excess is handed back as change.
    double credited = money2dp(amountPaid - change);

    fines[fi].paid    = money2dp(fines[fi].paid + credited);
    fines[fi].payDate = systemDate;
    fines[fi].settled = (balance == 0.0);   // safe: set exactly in the
                                            // full-settlement branch

    recalcMemberOutstanding(members, fines, fines[fi].memID);

    //5. Accumulate into the 2D statistics array
    // fine -> loan -> book -> category. Each hop may fail, because
    // a title can be soft-deleted after the loan was made, so
    // findBookIndexAny() is used and both indexes are checked.
    int cat = -1;
    int li = findLoanIndex(loans, fines[fi].loanID);
    if (li != -1) {
        int bi = findBookIndexAny(books, loans[li].bookID);
        if (bi != -1) cat = books[bi].category;
    }

    int monthIndex = fines[fi].payDate.month - 1;   // month 1-12 -> index 0-11
    if (cat >= 0 && cat < MAX_CATEGORIES &&
        monthIndex >= 0 && monthIndex < MONTHS) {
        monthlyStats[monthIndex][cat] += credited;
    }

    // 6. Confirm the outcome 
    cout << "\n  Payment recorded.\n";
    cout << "  Amount paid : RM " << setw(8) << credited << '\n';
    cout << "  Change      : RM " << setw(8) << change   << '\n';
    cout << "  Balance     : RM " << setw(8) << balance  << '\n';

    if (fines[fi].settled) cout << "  Status      : SETTLED\n";
    else                   cout << "  Status      : PART PAYMENT - balance carried forward\n";

    pressEnterToContinue();
}