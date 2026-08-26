#include "common.h"

void showIntroScreen();
void showExitScreen();
void systemDateMenu();

int main() {
    // The four shared collections. They are declared once here and passed
    // by reference into each module, so all modules work on the same data.
    vector<Member> members;
    vector<Book> books;
    vector<Loan> loans;
    vector<Fine> fines;

    seedData(members, books, loans, fines);
 
    showIntroScreen();

    int choice;

    do {
        printHeader("LIBRARY MANAGEMENT SYSTEM - MAIN MENU");

        cout << "  1. Member Management\n";
        cout << "  2. Book Catalogue & Inventory\n";
        cout << "  3. Loan, Return & Reservation\n";
        cout << "  4. Fine Payment & Reporting\n";
        cout << "  5. System Date (testing utility)\n";
        cout << "  0. Exit\n\n";
        printRule();
        cout << "  Members: " << countActiveMembers(members)
             << "   Titles: "  << countActiveBooks(books)
             << "   Loans: "   << loans.size() << '\n';
        printRule();
        cout << '\n';
 
        choice = readInt("Enter choice: ", 0, 5);

        switch (choice) {
            case 1:
                memberManagementMenu(members, loans);
                break;
            case 2:
                bookCatalogueMenu(books, members);
                break;
            case 3:
                loanMenu(loans, books, members, fines);
                break;
            case 4:
                fineMenu(fines, loans, members, books);
                break;
            case 5:
                systemDateMenu();
                break;
            case 0:
                break;
        }
    } while (choice != 0);
 
    showExitScreen();
    return 0;
}

//  Introduction screen shown once at start-up.
void showIntroScreen() {
    clearScreen();
    printRule('*');
    cout << "\n";
    cout << "            LIBRARY MANAGEMENT SYSTEM (LMS)\n";
    cout << "     Tunku Abdul Rahman University of Management\n";
    cout << "                  and Technology\n\n";
    cout << "     AMCS2123 Systems and Programming Concepts\n";
    cout << "     Programme DCS2S1     Tutorial Group G5\n\n";
    printRule('*');
    cout << "\n  This system manages library members, the book\n";
    cout << "  catalogue, loans and reservations, and the collection\n";
    cout << "  of overdue fines, together with management reports.\n\n";
    cout << "  System date is set to " << dateToString(systemDate)
         << ". It can be changed\n";
    cout << "  from main menu option 5 to demonstrate overdue cases.\n";
    pressEnterToContinue();
}

//  Termination message shown when the user selects Exit.
void showExitScreen() {
    clearScreen();
    printRule('*');
    cout << "\n     Thank you for using the Library Management System.\n";
    cout << "     All session data has been closed.\n\n";
    printRule('*');
    cout << '\n';
}

//  System date utility.
//  Every due-date and overdue calculation reads the global systemDate,
//  so advancing it here makes overdue loans and fines appear at once
//  during testing and during the presentation.

void systemDateMenu() {
    int choice;
    do {
        printHeader("SYSTEM DATE (TESTING UTILITY)");
        cout << "  Current system date : " << dateToString(systemDate) << "\n\n";
        cout << "  1. Advance by a number of days\n";
        cout << "  2. Set a specific date\n";
        cout << "  0. Back to Main Menu\n\n";
 
        choice = readInt("Enter choice: ", 0, 2);
 
        switch (choice) {
            case 1: {
                int days = readInt("Number of days to advance (1-365): ", 1, 365);
                systemDate = addDays(systemDate, days);
                cout << "\n  System date is now "
                     << dateToString(systemDate) << ".";
                pressEnterToContinue();
                break;
            }
            case 2: {
                Date newDate = readDate("Enter new date (YYYY-MM-DD): ");
                systemDate = newDate;
                cout << "\n  System date is now "
                     << dateToString(systemDate) << ".";
                pressEnterToContinue();
                break;
            }
            case 0:
                break;
        }
    } while (choice != 0);
}