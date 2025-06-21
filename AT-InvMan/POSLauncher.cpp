// Dhona Created this snippet to implement the POS system launcher
#include "POS.h"         // For POS class handling the point of sale process
#include "auth.h"        // For getting the currently logged-in user's branch
#include "db-conn.h"     // For potential shared DB connection utilities (if needed)

// Entry point for launching the POS system
void startPOS() {
    // Retrieve the branch associated with the current logged-in user
    std::string branch = getCurrentUserBranch();

    // If the user has access to all branches (e.g., admin), prompt for branch selection
    if (branch == "all") {
        std::cout << "Select branch (Auckland, Christchurch, Wellington): ";
        std::cin >> branch;
    }

    // Initialize the POS system with the selected or assigned branch
    POS pos(branch);  // Ensure this constructor is defined in the POS class

    // Main POS loop: allows scanning multiple items until the user chooses to stop
    char more = 'y';
    while (more == 'y' || more == 'Y') {
        pos.scanItem();
        std::cout << "Add another item? (y/n): ";
        std::cin >> more;
    }

    // After all items are scanned, proceed to generate and print the receipt
    pos.printReceipt();
}