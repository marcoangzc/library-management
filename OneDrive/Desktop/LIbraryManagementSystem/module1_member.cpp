#include "common.h"

// Module 1 - Member Management - clement ho jun hao

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
                cout << "\n  [Module 1] addMember() not implemented yet.";
                pressEnterToContinue();
                break;
            case 2:
                // TODO: searchMemberByID / ByName / ByPhone
                cout << "\n  [Module 1] search functions not implemented yet.";
                pressEnterToContinue();
                break;
            case 3:
                // TODO: updateMember(members, index);
                cout << "\n  [Module 1] updateMember() not implemented yet.";
                pressEnterToContinue();
                break;
            case 4:
                // TODO: suspendMember(members, index);
                cout << "\n  [Module 1] suspendMember() not implemented yet.";
                pressEnterToContinue();
                break;
            case 5:
                // TODO: deleteMember(members, index);
                //   SOFT DELETE ONLY - set active = false, never erase()
                cout << "\n  [Module 1] deleteMember() not implemented yet.";
                pressEnterToContinue();
                break;
            case 6:
                // TODO: displayAllMembers(members);  skip !active records
                cout << "\n  [Module 1] displayAllMembers() not implemented yet.";
                pressEnterToContinue();
                break;
            case 7:
                // TODO: displayMemberCard(m, loans);
                cout << "\n  [Module 1] displayMemberCard() not implemented yet.";
                pressEnterToContinue();
                break;
            case 0:
                break;
        }
    } while (choice != 0);
}