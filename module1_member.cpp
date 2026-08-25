#include "common.h"

// Module 1 - Member Management - clement ho jun hao
void addMember(vector<Member>& members); 
void searchMember(const vector<Member>& members); 
void updateMember(vector<Member>& members);
void suspendMember(vector<Member>& members);
void deleteMember(vector<Member>& members);
void displayAllMembers(const vector<Member>& members);
void displayMemberCard(const vector<Member>& members);

void memberManagementMenu(vector<Member>& members, const vector<Loan>& loans) {
    int choice;
 
    do {
        printHeader("MEMBER MANAGEMENT");
        cout << "  1. Register New Member\n";
        cout << "  2. Search Member\n";
        cout << "  3. Update Member Particulars\n";
        cout << "  4. Suspend / Reactivate Member\n";
        cout << "  5. Delete Member\n";
        cout << "  6. Display All Members\n";
        cout << "  7. Display Member Record Card\n";
        cout << "  0. Back to Main Menu\n\n";
 
        choice = readInt("Enter choice: ", 0, 7);
 
        switch (choice) {
            case 1:
                // TODO: addMember(members);
                //   - reject when members.size() >= MAX_MEMBERS
                //   - use nextMemberID(members) for the new ID
                //   - set active = true
                addMember(members);
                pressEnterToContinue();
                break;
            case 2:
                // TODO: searchMemberByID / ByName / ByPhoe
                searchMember(members);
                pressEnterToContinue();
                break;
            case 3:
                // TODO: updateMember(members, index);
                updateMember(members);
                pressEnterToContinue();
                break;
            case 4:
                // TODO: suspendMember(members, index);
                suspendMember(members);
                pressEnterToContinue();
                break;
            case 5:
                // TODO: deleteMember(members, index);
                //   SOFT DELETE ONLY - set active = false, never erase()
                deleteMember(members);
                pressEnterToContinue();
                break;
            case 6:
                // TODO: displayAllMembers(members);  skip !active records
                displayAllMembers(members);
                pressEnterToContinue();
                break;
            case 7:
                // TODO: displayMemberCard(m, loans);
                displayMemberCard(members);
                pressEnterToContinue();
                break;
            case 0:
                break;
        }
    } while (choice != 0);

    //Fuction addMember();
void addMember(vector<Member>& members) {
    printHeader("REGISTER NEW MEMBER");
    
    if (members.size() >= MAX_MEMBERS) {
        cout << "Error: Maximum number of members reached (" << MAX_MEMBERS << ").\n";
        return;
    }

    Member m;
    m.memID = readInt("Enter New Member ID: ");
    
    if (findMemberIndex(members, m.memID) != -1) {
        cout << "Error: Member ID " << m.memID << " already exists.\n";
        return;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter Name: ";
    getline(cin, m.name);

    cout << "Enter Phone: ";
    getline(cin, m.phone);

    cout << "Enter Tier (S=Student, F=Staff, P=Premium, N=Normal): ";
    cin >> m.tier;
    m.tier = toupper(m.tier);

    m.booksOnLoan = 0;
    m.outstandingFine = 0.0;
    m.suspended = false;

    members.push_back(m);
    cout << "\nMember successfully registered!\n";
}

//Function searchMember()
void searchMember(const vector<Member>& members) {
    printHeader("SEARCH MEMBER");
    cout << "1. Search by Member ID\n";
    cout << "2. Search by Name\n";
    cout << "3. Search by Phone\n";
    cout << "0. Cancel\n\n";

    int choice = readInt("Enter choice: ");
    
    if (choice == 1) {
        int id = readInt("Enter Member ID to search: ");
        int index = findMemberIndex(members, id);
        if (index != -1) {
            const Member& m = members[index];
            cout << "\n--- Member Found ---\n";
            cout << "ID: " << m.memID << "\nName: " << m.name << "\nPhone: " << m.phone 
                 << "\nTier: " << m.tier << "\nStatus: " << (m.suspended ? "Suspended" : "Active") 
                 << "\nBooks on Loan: " << m.booksOnLoan << "\nOutstanding Fine: $" << m.outstandingFine << "\n";
        } else {
            cout << "\nMember not found.\n";
        }
    } else if (choice == 2) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Name to search (exact match or partial): ";
        string name;
        getline(cin, name);
        bool found = false;
        for (const auto& m : members) {
            // Case-insensitive or sensitive? We will do simple find for now.
            if (m.name.find(name) != string::npos) {
                if (!found) cout << "\n--- Matching Members ---\n";
                cout << "ID: " << m.memID << " | Name: " << m.name << " | Phone: " << m.phone << "\n";
                found = true;
            }
        }
        if (!found) cout << "\nMember not found.\n";
    } else if (choice == 3) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Phone to search: ";
        string phone;
        getline(cin, phone);
        bool found = false;
        for (const auto& m : members) {
            if (m.phone.find(phone) != string::npos) {
                if (!found) cout << "\n--- Matching Members ---\n";
                cout << "ID: " << m.memID << " | Name: " << m.name << " | Phone: " << m.phone << "\n";
                found = true;
            }
        }
        if (!found) cout << "\nMember not found.\n";
    } else if (choice != 0) {
        cout << "\nInvalid choice.\n";
    }
}

//Fucntion UpdateMember()
void updateMember(vector<Member>& members) {
    printHeader("UPDATE MEMBER PARTICULARS");
    int id = readInt("Enter Member ID to update: ");
    int index = findMemberIndex(members, id);
    if (index == -1) {
        cout << "\nMember not found.\n";
        return;
    }

    Member& m = members[index];
    cout << "\n--- Current Details ---\n";
    cout << "Name: " << m.name << "\nPhone: " << m.phone << "\nTier: " << m.tier << "\n";
    
    cout << "\nEnter New Name (leave blank to keep current): ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string temp;
    getline(cin, temp);
    if (!temp.empty()) m.name = temp;

    cout << "Enter New Phone (leave blank to keep current): ";
    getline(cin, temp);
    if (!temp.empty()) m.phone = temp;

    cout << "Enter New Tier (S=Student, F=Staff, P=Premium, N=Normal, leave blank to keep current): ";
    getline(cin, temp);
    if (!temp.empty()) m.tier = toupper(temp[0]);

    cout << "\nMember particulars updated successfully!\n";
}

void suspendMember(vector<Member>& members) {
    printHeader("SUSPEND / REACTIVATE MEMBER");
    int id = readInt("Enter Member ID: ");
    int index = findMemberIndex(members, id);
    if (index == -1) {
        cout << "\nMember not found.\n";
        return;
    }

    Member& m = members[index];
    cout << "\nCurrent Status: " << (m.suspended ? "Suspended" : "Active") << "\n";
    cout << "Do you want to toggle status? (Y/N): ";
    char ch;
    cin >> ch;
    if (toupper(ch) == 'Y') {
        m.suspended = !m.suspended;
        cout << "\nMember status updated to: " << (m.suspended ? "Suspended" : "Active") << "\n";
    } else {
        cout << "\nNo changes made.\n";
    }
}

void deleteMember(vector<Member>& members) {
    printHeader("DELETE MEMBER");
    int id = readInt("Enter Member ID to delete: ");
    int index = findMemberIndex(members, id);
    if (index == -1) {
        cout << "\nMember not found.\n";
        return;
    }
    
    Member& m = members[index];
    if (m.booksOnLoan > 0 || m.outstandingFine > 0) {
        cout << "\nCannot delete member! Member has books on loan or outstanding fines.\n";
        return;
    }

    cout << "Are you sure you want to delete " << m.name << "? (Y/N): ";
    char ch;
    cin >> ch;
    if (toupper(ch) == 'Y') {
        members.erase(members.begin() + index);
        cout << "\nMember deleted successfully.\n";
    } else {
        cout << "\nDeletion cancelled.\n";
    }
}

void displayAllMembers(const vector<Member>& members) {
    printHeader("DISPLAY ALL MEMBERS");
    if (members.empty()) {
        cout << "No members registered yet.\n";
        return;
    }

    cout << left << setw(10) << "ID" 
         << setw(20) << "Name" 
         << setw(15) << "Phone" 
         << setw(10) << "Tier" 
         << setw(10) << "Status" 
         << setw(10) << "Loans" 
         << "Fines\n";
    cout << "--------------------------------------------------------------------------------\n";
    
    for (const auto& m : members) {
        cout << left << setw(10) << m.memID 
             << setw(20) << m.name 
             << setw(15) << m.phone 
             << setw(10) << m.tier 
             << setw(10) << (m.suspended ? "Suspended" : "Active") 
             << setw(10) << m.booksOnLoan 
             << "$" << fixed << setprecision(2) << m.outstandingFine << "\n";
    }
}

void displayMemberCard(const vector<Member>& members) {
    printHeader("DISPLAY MEMBER RECORD CARD");
    int id = readInt("Enter Member ID: ");
    int index = findMemberIndex(members, id);
    if (index == -1) {
        cout << "\nMember not found.\n";
        return;
    }

    const Member& m = members[index];
    cout << "\n=========================================\n";
    cout << "           MEMBER RECORD CARD            \n";
    cout << "=========================================\n";
    cout << "ID:              " << m.memID << "\n";
    cout << "Name:            " << m.name << "\n";
    cout << "Phone:           " << m.phone << "\n";
    cout << "Tier:            " << m.tier << "\n";
    cout << "Status:          " << (m.suspended ? "Suspended" : "Active") << "\n";
    cout << "Books on Loan:   " << m.booksOnLoan << "\n";
    cout << "Outstanding Fine:$" << fixed << setprecision(2) << m.outstandingFine << "\n";
    cout << "=========================================\n";
}
}