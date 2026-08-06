#include "common.h"

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
                // TODO: calcFine(daysOverdue, FINE_PER_DAY, FINE_CAP)
                //       then multiply by getFineRate(member.tier)
                cout << "\n  [Module 4] calcFine() not implemented yet.";
                pressEnterToContinue();
                break;
            case 2:
                // TODO: processPayment(...)
                //   - update f.paid; settled = (fineBalance(f) == 0)
                //   - update member.outstandingFine to match
                //   - monthlyStats[month-1][book.category] += amountPaid
                cout << "\n  [Module 4] processPayment() not implemented yet.";
                pressEnterToContinue();
                break;
            case 3:
                // TODO: printReceipt(f, m, b);
                cout << "\n  [Module 4] printReceipt() not implemented yet.";
                pressEnterToContinue();
                break;
            case 4:
                // TODO: fineCollectionReport(fines);
                cout << "\n  [Module 4] fineCollectionReport() not implemented yet.";
                pressEnterToContinue();
                break;
            case 5:
                // TODO: mostBorrowedReport(books, borrowCount);
                cout << "\n  [Module 4] mostBorrowedReport() not implemented yet.";
                pressEnterToContinue();
                break;
            case 6:
                // TODO: overdueMemberReport(members, loans, systemDate);
                cout << "\n  [Module 4] overdueMemberReport() not implemented yet.";
                pressEnterToContinue();
                break;
            case 7:
                // TODO: monthlyStatisticsReport(monthlyStats);
                cout << "\n  [Module 4] monthlyStatisticsReport() not implemented yet.";
                pressEnterToContinue();
                break;
            case 0:
                break;
        }
    } while (choice != 0);
}