#include "common.h"


// Module 2 - Book Catalogue & Inventory Management - chong wen kai

void bookCatalogueMenu(vector<Book>& books) {
    int choice = -1;

    do {
        printHeader("BOOK CATALOGUE & INVENTORY");
        cout << "1. Add New Book Title\n";
        cout << "2. Search Book\n";
        cout << "3. Update Book Details\n";
        cout << "4. Add / Remove Copies\n";
        cout << "5. Assign Shelf Location\n";
        cout << "6. Remove Title\n";
        cout << "7. Display Catalogue\n";
        cout << "8. Display Availability Listing\n";
        cout << "0. Back to Main Menu\n\n";

        choice = readInt("Enter choice: ");

        switch (choice) {
            case 1:
                // TODO: addBook(books);
                cout << "\n[Module 2] addBook() not implemented yet.";
                pressEnterToContinue();
                break;
            case 2:
                // TODO: searchByTitle / ByAuthor / ByISBN
                cout << "\n[Module 2] search functions not implemented yet.";
                pressEnterToContinue();
                break;
            case 3:
                // TODO: updateBook(books, index);
                cout << "\n[Module 2] updateBook() not implemented yet.";
                pressEnterToContinue();
                break;
            case 4:
                // TODO: addCopies(books, index); removeCopies(books, index);
                cout << "\n[Module 2] add/remove copies not implemented yet.";
                pressEnterToContinue();
                break;
            case 5:
                // TODO: assignShelfLocation(books, index);
                cout << "\n[Module 2] assignShelfLocation() not implemented yet.";
                pressEnterToContinue();
                break;
            case 6:
                // TODO: removeBook(books, index);
                cout << "\n[Module 2] removeBook() not implemented yet.";
                pressEnterToContinue();
                break;
            case 7:
                // TODO: displayCatalogue(books);
                cout << "\n[Module 2] displayCatalogue() not implemented yet.";
                pressEnterToContinue();
                break;
            case 8:
                // TODO: displayAvailability(books);
                cout << "\n[Module 2] displayAvailability() not implemented yet.";
                pressEnterToContinue();
                break;
            case 0:
                break;
            default:
                cout << "\nInvalid choice. Please try again.";
                pressEnterToContinue();
        }
    } while (choice != 0);
}