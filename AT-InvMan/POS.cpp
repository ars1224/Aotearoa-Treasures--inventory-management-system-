// Dhona created this snippet to implement the POS system
// It handles scanning, cart management, inventory deduction, and receipt printing.

#include "POS.h"
#include <sqlite3.h>
#include <iostream>
#include <iomanip>
#include "auth.h"  // To determine the user's assigned branch
#include "salesLogger.h" // For logging sales into branch-specific tables

using namespace std;

// Constructor using user branch from session (dynamic)
POS::POS(const std::string& branch) : userBranch(branch) {
    connectDB();
}

// Default constructor if branch isn't passed (fallback to Wellington)
POS::POS() : userBranch("Wellington") {
    connectDB();
}

// Destructor to clean up DB connection
POS::~POS() {
    closeDB();
}

// Establish connection to SQLite database
void POS::connectDB() {
    int rc = sqlite3_open("../db/at-invman.db", &db);
    if (rc) {
        cerr << "Error opening DB: " << sqlite3_errmsg(db) << endl;
        db = nullptr;
    }
}

// Safely close the SQLite DB connection
void POS::closeDB() {
    if (db) sqlite3_close(db);
}

// Scans an item by code and adds to cart if available
void POS::scanItem() {
    string code;
    cout << "Enter Product Code (e.g. AT1001): ";
    cin >> code;

    // Choose correct column based on user's branch
    string qtyColumn;
    if (userBranch == "Auckland") qtyColumn = "ALK_Qty";
    else if (userBranch == "Christchurch") qtyColumn = "CHCH_Qty";
    else qtyColumn = "WLG_Qty";

    // SQL to retrieve product details and current stock
    string sql = "SELECT Product_Name, Product_Price, " + qtyColumn +
        " FROM product WHERE Product_Code = ?;";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            double price = sqlite3_column_double(stmt, 1);
            int stockQty = sqlite3_column_int(stmt, 2);

            // Check stock availability
            if (stockQty <= 0) {
                cout << "\nWarning: \"" << name << "\" is currently out of stock at " << userBranch << ".\n";
                cout << "Would you like to:\n1. Try another product\n2. Return to POS menu\nSelect an option: ";
                int choice;
                cin >> choice;
                if (choice == 1) return scanItem();
                else return;
            }

            // Prompt for quantity
            int qty;
            cout << "Product: " << name << ", Price: $" << fixed << setprecision(2) << price << endl;
            cout << "Enter quantity to purchase: ";
            cin >> qty;

            if (qty > stockQty) {
                cout << "Only " << stockQty << " units available. Please reduce quantity.\n";
            }
            else {
                cart.push_back({ code, name, price, qty });
                cout << "Item added to cart.\n";
            }
        }
        else {
            // If code not found
            cout << "Product not found. Please check the code and try again.\n";
            cout << "Would you like to:\n1. Try again\n2. Return to POS menu\nSelect an option: ";
            int choice;
            cin >> choice;
            if (choice == 1) return scanItem();
            else return;
        }

        sqlite3_finalize(stmt);
    }
    else {
        cerr << "SQL preparation failed: " << sqlite3_errmsg(db) << "\n";
    }
}

// Prints receipt and deducts stock only upon confirmed checkout
void POS::printReceipt() {
    if (cart.empty()) {
        cout << "\n Cannot checkout: your cart is empty.\n";
        return;
    }

    // Step 3: Confirm before committing sale
    char confirm;
    cout << "\nProceed to checkout and print receipt? (y/n): ";
    cin >> confirm;
    if (tolower(confirm) != 'y') {
        cout << "Checkout cancelled. Returning to POS menu.\n";
        return;
    }

    double total = 0.0;

    // Log the confirmed sale
    logSaleToBranchTable(userBranch, cart); 

    // Receipt formatting
    cout << "\n========== RECEIPT ==========\n";
    for (const auto& item : cart) {
        double itemTotal = item.price * item.quantity;
        cout << item.name << " x" << item.quantity
            << " @ $" << item.price << " = $" << itemTotal << "\n";
        total += itemTotal;

        // Update inventory for each item purchased
        updateInventory(item.code, item.quantity); 
    }
    cout << "-------------------------------\n";
    cout << "Total: $" << fixed << setprecision(2) << total << "\n";
    cout << "Branch: " << userBranch << "\n";
    cout << "===============================\n";
    cout << "\n Sale completed for branch: " << userBranch << "\n";
}

// Exposes the cart (read-only) for display or external use
std::vector<CartItem> POS::getCart() const {
    return cart;
}

// Main POS launcher with cashier-friendly interface
void processPointOfSale() {
    string branch = getCurrentUserBranch();
    cout << "\n Starting POS for branch: " << branch << "\n";

    POS pos(branch);
    char choice;

    do {
        // Cashier POS Menu
        cout << "\n========== Point of Sale Menu ==========\n";
        cout << "Logged-in Branch: " << branch << "\n";
        cout << "1. Scan item\n";
        cout << "2. View cart & total\n";
        cout << "3. Checkout & print receipt\n";
        cout << "4. Cancel transaction\n";
        cout << "Select an option: ";
        cin >> choice;

        switch (choice) {
        case '1':
            pos.scanItem();
            break;

        case '2': {
            double total = 0.0;
            cout << "\n------- Current Cart -------\n";
            for (const auto& item : pos.getCart()) {
                double itemTotal = item.price * item.quantity;
                cout << item.name << " x" << item.quantity
                    << " @ $" << item.price << " = $" << itemTotal << "\n";
                total += itemTotal;
            }
            cout << "----------------------------\n";
            cout << "Total so far: $" << fixed << setprecision(2) << total << "\n";
            break;
        }

        case '3':
            cout << "\n Finalizing transaction...\n";
            pos.printReceipt();
            return;

        case '4':
            cout << "\n Transaction canceled. Cart cleared.\n";
            return;

        default:
            cout << "Invalid option. Try again.\n";
        }

    } while (true);
}

// Updates product stock in DB based on user's location
void POS::updateInventory(const std::string& code, int qty) {
    std::string col;
    if (userBranch == "Auckland") col = "ALK_Qty";
    else if (userBranch == "Christchurch") col = "CHCH_Qty";
    else if (userBranch == "Wellington") col = "WLG_Qty";
    else {
        std::cerr << "Invalid branch.\n";
        return;
    }

    std::string sql = "UPDATE product SET " + col + " = " + col + " - ? WHERE Product_Code = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, qty);
        sqlite3_bind_text(stmt, 2, code.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Failed to update inventory.\n";
        }

        sqlite3_finalize(stmt);
    }
    else {
        std::cerr << "SQL prepare error.\n";
    }
}


//TESTING ONLY
/*#include <iostream>
using namespace std;

void testPOS() {
    cout << "POS module is running correctly!" << endl;
}*/