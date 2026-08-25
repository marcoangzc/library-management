#include "common.h"

// Module 1 - Member Management - clement ho jun hao
//
// Each menu action is its own user-defined function. The validation helpers
// validateName() / validatePhone() are reused by addMember() and updateMember()
// so both paths apply exactly the same business rules.

// ---- function prototypes ----
void   addMember(vector<Member>& members);
void   searchMember(const vector<Member>& members);
void   searchMemberByID(const vector<Member>& members);
void   searchMemberByName(const vector<Member>& members);
void   searchMemberByPhone(const vector<Member>& members);
void   updateMember(vector<Member>& members);
void   suspendMember(vector<Member>& members);
void   deleteMember(vector<Member>& members);
void   displayAllMembers(const vector<Member>& members);
void   displayMemberCard(const vector<Member>& members, const vector<Loan>& loans);

bool   validateName(const string& name);
bool   validatePhone(const string& phone);
static string toLowerCopy(const string& text);
void   printMemberDetails(const Member& m);

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
                addMember(members);
                pressEnterToContinue();
                break;
            case 2:
                searchMember(members);
                pressEnterToContinue();
                break;
            case 3:
                updateMember(members);
                pressEnterToContinue();
                break;
            case 4:
                suspendMember(members);
                pressEnterToContinue();
                break;
            case 5:
                deleteMember(members);
                pressEnterToContinue();
                break;
            case 6:
                displayAllMembers(members);
                pressEnterToContinue();
                break;
            case 7:
                displayMemberCard(members, loans);
                pressEnterToContinue();
                break;
            case 0:
                break;
        }
    } while (choice != 0);
}

// ---- validation helpers ----

// A valid name is not blank, fits within NAME_MAX_LEN and contains no digits.
bool validateName(const string& name) {
    if (name.empty()) return false;
    if ((int)name.length() > NAME_MAX_LEN) return false;

    for (int i = 0; i < (int)name.length(); i++)
        if (isdigit((unsigned char)name[i])) return false;

    return true;
}

// A valid phone number is 10 to 11 digits (Malaysian mobile numbers).
bool validatePhone(const string& phone) {
    if (!isAllDigits(phone)) return false;
    if ((int)phone.length() < PHONE_MIN_LEN || (int)phone.length() > PHONE_MAX_LEN)
        return false;

    return true;
}

// Returns a lower-case copy so name searches can ignore capitalisation.
// static = local to this file only (same idea as the helpers in common.cpp).
static string toLowerCopy(const string& text) {
    string result = text;
    for (int i = 0; i < (int)result.length(); i++)
        result[i] = tolower((unsigned char)result[i]);
    return result;
}

// Menu option 1 - register a new member.
void addMember(vector<Member>& members) {
    printHeader("REGISTER NEW MEMBER");

    // Capacity guard: MAX_MEMBERS sizes the shared memberLoanHistory array,
    // so the vector must never grow past it.
    if ((int)members.size() >= MAX_MEMBERS) {
        cout << "\n  >> Registration failed. Maximum of "
             << MAX_MEMBERS << " members reached.\n";
        return;
    }

    Member m;
    m.memID = nextMemberID(members);   // IDs are assigned by the system
    cout << "  New Member ID : " << m.memID << " (assigned automatically)\n\n";

    // while loops keep re-prompting until the input passes validation.
    while (true) {
        m.name = readNonEmptyLine("  Name: ", NAME_MAX_LEN);
        if (validateName(m.name)) break;
        cout << "  >> Invalid name. No digits allowed, maximum "
             << NAME_MAX_LEN << " characters.\n";
    }

    while (true) {
        m.phone = readNonEmptyLine("  Phone (10-11 digits): ", PHONE_MAX_LEN);
        if (validatePhone(m.phone)) break;
        cout << "  >> Invalid phone number. Enter 10 to 11 digits only.\n";
    }

    m.tier = readChar("  Tier (S=Student, F=Staff, P=Premium): ", "SFP");

    m.booksOnLoan     = 0;
    m.outstandingFine = 0.00;
    m.suspended       = false;
    m.active          = true;

    members.push_back(m);

    cout << "\n  Member registered successfully.\n";
    cout << "  ID   : " << m.memID << '\n';
    cout << "  Name : " << m.name << '\n';
}

// Menu option 2 - search sub-menu driven by a switch.
void searchMember(const vector<Member>& members) {
    printHeader("SEARCH MEMBER");
    cout << "  1. Search by Member ID\n";
    cout << "  2. Search by Name\n";
    cout << "  3. Search by Phone\n";
    cout << "  0. Cancel\n\n";

    int choice = readInt("Enter choice: ", 0, 3);

    switch (choice) {
        case 1: searchMemberByID(members);   break;
        case 2: searchMemberByName(members); break;
        case 3: searchMemberByPhone(members); break;
        case 0: break;
    }
}

void searchMemberByID(const vector<Member>& members) {
    int id = readInt("\n  Member ID to search: ");
    int index = findMemberIndex(members, id);

    if (index == -1) {
        cout << "\n  >> Member Not Found.\n";
        return;
    }

    printMemberDetails(members[index]);
}

// Substring match on the name, ignoring upper/lower case.
void searchMemberByName(const vector<Member>& members) {
    string target = toLowerCopy(readLine("\n  Name to search: "));

    bool found = false;
    for (int i = 0; i < (int)members.size(); i++) {
        if (!members[i].active) continue;

        if (toLowerCopy(members[i].name).find(target) != string::npos) {
            if (!found) {
                cout << "\n" << left
                     << setw(8)  << "ID"
                     << setw(22) << "Name"
                     << setw(14) << "Phone"
                     << setw(6)  << "Tier"
                     << '\n';
                printRule();
            }
            cout << left
                 << setw(8)  << members[i].memID
                 << setw(22) << members[i].name
                 << setw(14) << members[i].phone
                 << setw(6)  << members[i].tier
                 << '\n';
            found = true;
        }
    }

    if (!found) cout << "\n  >> Member Not Found.\n";
}

// Substring match on the phone number.
void searchMemberByPhone(const vector<Member>& members) {
    string target = readLine("\n  Phone to search: ");

    bool found = false;
    for (int i = 0; i < (int)members.size(); i++) {
        if (!members[i].active) continue;

        if (members[i].phone.find(target) != string::npos) {
            if (!found) {
                cout << "\n" << left
                     << setw(8)  << "ID"
                     << setw(22) << "Name"
                     << setw(14) << "Phone"
                     << setw(6)  << "Tier"
                     << '\n';
                printRule();
            }
            cout << left
                 << setw(8)  << members[i].memID
                 << setw(22) << members[i].name
                 << setw(14) << members[i].phone
                 << setw(6)  << members[i].tier
                 << '\n';
            found = true;
        }
    }

    if (!found) cout << "\n  >> Member Not Found.\n";
}

// Prints the full record card for one member.
// Shared by searchMemberByID() and displayMemberCard().
void printMemberDetails(const Member& m) {
    cout << fixed << setprecision(2);

    cout << "\n  -----------------------------------------\n";
    cout <<           "           MEMBER RECORD CARD\n";
    cout <<           "  -----------------------------------------\n";
    cout << left << setw(18) << "  ID"               << ": " << m.memID << '\n';
    cout << left << setw(18) << "  Name"             << ": " << m.name << '\n';
    cout << left << setw(18) << "  Phone"            << ": " << m.phone << '\n';
    cout << left << setw(18) << "  Tier"             << ": " << m.tier
         << " (" << getTierName(m.tier) << ")\n";
    cout << left << setw(18) << "  Status"           << ": "
         << (m.suspended ? "Suspended" : "Active") << '\n';
    cout << left << setw(18) << "  Books on Loan"    << ": " << m.booksOnLoan
         << " (limit " << getBorrowLimit(m.tier) << ")\n";
    cout << left << setw(18) << "  Outstanding Fine" << ": RM " << m.outstandingFine << '\n';
    cout <<           "  -----------------------------------------\n";
}

// Menu option 3 - update particulars. Blank input keeps the current value.
void updateMember(vector<Member>& members) {
    printHeader("UPDATE MEMBER PARTICULARS");
    int id = readInt("  Member ID to update: ");
    int index = findMemberIndex(members, id);

    if (index == -1) {
        cout << "\n  >> Member Not Found.\n";
        return;
    }

    Member& m = members[index];

    cout << "\n  Current Details\n";
    cout << "  Name  : " << m.name << '\n';
    cout << "  Phone : " << m.phone << '\n';
    cout << "  Tier  : " << m.tier << " (" << getTierName(m.tier) << ")\n";
    cout << "\n  Press Enter without typing to keep the current value.\n";

    string input = readLine("\n  New Name: ");
    if (!input.empty()) {
        if (validateName(input))
            m.name = input;
        else
            cout << "  >> Invalid name. Kept unchanged.\n";
    }

    input = readLine("  New Phone: ");
    if (!input.empty()) {
        if (validatePhone(input))
            m.phone = input;
        else
            cout << "  >> Invalid phone number. Kept unchanged.\n";
    }

    input = readLine("  New Tier (S/F/P): ");
    if (!input.empty()) {
        char tier = toupper((unsigned char)input[0]);
        if (input.length() == 1 && (tier == TIER_STUDENT || tier == TIER_STAFF || tier == TIER_PREMIUM))
            m.tier = tier;
        else
            cout << "  >> Invalid tier. Kept unchanged.\n";
    }

    cout << "\n  Particulars updated successfully.\n";
}

// Menu option 4 - toggle suspended <-> active.
void suspendMember(vector<Member>& members) {
    printHeader("SUSPEND / REACTIVATE MEMBER");
    int id = readInt("  Member ID: ");
    int index = findMemberIndex(members, id);

    if (index == -1) {
        cout << "\n  >> Member Not Found.\n";
        return;
    }

    Member& m = members[index];
    cout << "\n  Current Status : " << (m.suspended ? "Suspended" : "Active") << '\n';

    if (confirmYesNo("  Toggle this status")) {
        m.suspended = !m.suspended;
        cout << "\n  Status updated to: "
             << (m.suspended ? "Suspended" : "Active") << '\n';
    } else {
        cout << "\n  No changes made.\n";
    }
}

// Menu option 5 - SOFT DELETE only.
// The record stays in the vector with active = false so that loan and fine
// history owned by other modules can still be traced back to this member.
void deleteMember(vector<Member>& members) {
    printHeader("DELETE MEMBER");
    int id = readInt("  Member ID to delete: ");
    int index = findMemberIndex(members, id);

    if (index == -1) {
        cout << "\n  >> Member Not Found.\n";
        return;
    }

    Member& m = members[index];

    // A member who still holds books or owes money cannot leave the library.
    if (m.booksOnLoan > 0 || m.outstandingFine > 0.0) {
        cout << "\n  >> Cannot delete. Member still has books on loan"
             << " or an outstanding fine.\n";
        return;
    }

    if (!confirmYesNo("\n  Delete member '" + m.name + "'")) {
        cout << "\n  Deletion cancelled.\n";
        return;
    }

    m.active = false;
    cout << "\n  Member deleted. Historical records remain available.\n";
}

// Menu option 6 - listing of every active member, aligned with setw().
void displayAllMembers(const vector<Member>& members) {
    printHeader("DISPLAY ALL MEMBERS");

    int total = countActiveMembers(members);

    if (total == 0) {
        cout << "\n  No members registered yet.\n";
        return;
    }

    cout << left
         << setw(8)  << "ID"
         << setw(24) << "Name"
         << setw(14) << "Phone"
         << setw(9)  << "Tier"
         << setw(11) << "Status"
         << right
         << setw(6)  << "Loans"
         << setw(10) << "Fine(RM)"
         << '\n';
    printRule();

    cout << fixed << setprecision(2);

    for (int i = 0; i < (int)members.size(); i++) {
        if (!members[i].active) continue;   // skip soft-deleted records

        cout << left
             << setw(8)  << members[i].memID
             << setw(24) << members[i].name.substr(0, 23)
             << setw(14) << members[i].phone
             << setw(9)  << getTierName(members[i].tier).substr(0, 8)
             << setw(11) << (members[i].suspended ? "SUSPENDED" : "ACTIVE")
             << right
             << setw(6)  << members[i].booksOnLoan
             << setw(10) << members[i].outstandingFine
             << '\n';
    }

    printRule();
    cout << "  Total active members: " << total << '\n';
}

// Menu option 7 - record card plus the loans this member currently holds.
// The loan data belongs to Module 3; it is passed in read-only.
void displayMemberCard(const vector<Member>& members, const vector<Loan>& loans) {
    printHeader("MEMBER RECORD CARD");
    int id = readInt("  Member ID: ");
    int index = findMemberIndex(members, id);

    if (index == -1) {
        cout << "\n  >> Member Not Found.\n";
        return;
    }

    printMemberDetails(members[index]);

    cout << "\n  Current Loans\n";

    int loanCount = 0;
    for (int i = 0; i < (int)loans.size(); i++) {
        if (loans[i].memID != members[index].memID) continue;
        if (loans[i].status != LOAN_ISSUED) continue;

        cout << "   Loan " << loans[i].loanID
             << " | Book " << loans[i].bookID
             << " | Issued " << dateToString(loans[i].issueDate)
             << " | Due " << dateToString(loans[i].dueDate);

        if (isOverdue(loans[i], systemDate))
            cout << " | OVERDUE " << daysOverdue(loans[i], systemDate) << " day(s)";

        cout << '\n';
        loanCount++;
    }

    if (loanCount == 0) cout << "   (none)\n";

    cout << "  -----------------------------------------\n";
}
