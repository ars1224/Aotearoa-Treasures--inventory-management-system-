#include "POS.h"         // For POS class
#include "auth.h"        // For getCurrentUserBranch()
#include "db-conn.h"     // For getDB()

void startPOS() {
    std::string branch = getCurrentUserBranch();
    if (branch == "all") {
        std::cout << "Select branch (Auckland, Christchurch, Wellington): ";
        std::cin >> branch;
    }

    POS pos(branch);  // <-- make sure this constructor exists
    char more = 'y';
    while (more == 'y' || more == 'Y') {
        pos.scanItem();
        std::cout << "Add another item? (y/n): ";
        std::cin >> more;
    }

    pos.printReceipt();
}