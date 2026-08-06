#include "common.h"


// Module 3 - Loan, Return & Reservation Management leong zheng yang


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
                // TODO: issueLoan(...)
                //   - check canBorrow(member), print borrowStatusMessage()
                //   - dueDate = addDays(systemDate, LOAN_DAYS)
                //   - decrementAvailable(); members[i].booksOnLoan++
                //   - borrowCount[bookIndex]++
                //   - memberLoanHistory[memberIndex][book.category]++
                cout << "\n  [Module 3] issueLoan() not implemented yet.";
                pressEnterToContinue();
                break;
            case 2:
                // TODO: renewLoan(...)  reject if renewCount >= MAX_RENEW,
                //       if already overdue, or if someone is in the queue
                cout << "\n  [Module 3] renewLoan() not implemented yet.";
                pressEnterToContinue();
                break;
            case 3:
                // TODO: returnBook(...)
                //   - status = LOAN_RETURNED; returnDate = systemDate
                //   - if daysOverdue() > 0, create a Fine record in 'fines'
                //   - promoteNextInQueue() -> reservedCopies++ instead of
                //     availableCopies++ when someone is waiting
                cout << "\n  [Module 3] returnBook() not implemented yet.";
                pressEnterToContinue();
                break;
            case 4:
                // TODO: placeReservation(reserveQueue, bookIndex, memID);
                cout << "\n  [Module 3] placeReservation() not implemented yet.";
                pressEnterToContinue();
                break;
            case 5:
                // TODO: cancelReservation(reserveQueue, bookIndex, memID);
                cout << "\n  [Module 3] cancelReservation() not implemented yet.";
                pressEnterToContinue();
                break;
            case 6:
                // TODO: displayActiveLoans(...)  use isOverdue(l, systemDate)
                cout << "\n  [Module 3] displayActiveLoans() not implemented yet.";
                pressEnterToContinue();
                break;
            case 7:
                // TODO: displayReservationQueue(reserveQueue, books);
                cout << "\n  [Module 3] displayReservationQueue() not implemented yet.";
                pressEnterToContinue();
                break;
            case 0:
                break;
        }
    } while (choice != 0);
}