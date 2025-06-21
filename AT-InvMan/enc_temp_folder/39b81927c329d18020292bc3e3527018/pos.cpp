// Dhona created this snippet to implement the POS system
#include "POS.h"
#include <sqlite3.h>
#include <iostream>
#include <iomanip>
#include "auth.h"  // For getCurrentUserBranch()
#include "salesLogger.h"

using namespace std;

// Constructor with branch passed from session
POS::POS(const std::string& branch) : userBranch(branch) {
    connectDB();
}

// Default constructor (fallback to Wellington)
POS::POS() : userBranch("Wellington") {
    connectDB();
}

POS::~POS() {
    closeDB();
}

void POS::connectDB() {
    int rc = sqlite3_open("../db/at-invman.db", &db);
    if (rc) {
        cerr << "Error opening DB: " << sqlite3_errmsg(db) << endl;
        db = nullptr;
    }
}

void POS::closeDB() {
    if (db) sqlite3_close(db);
}

void POS::scanItem() {
    string code;
    cout << "Enter Product Code (e.g. AT1001): ";
    cin >> code;

    // Determine which column to use for stock quantity
    string qtyColumn;
    if (userBranch == "Auckland") qtyColumn = "ALK_Qty";
    else if (userBranch == "Christchurch") qtyColumn = "CHCH_Qty";
    else qtyColumn = "WLG_Qty";

    string sql = "SELECT Product_Name, Product_Price, " + qtyColumn +
        " FROM product WHERE Product_Code = ?;";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            double price = sqlite3_column_double(stmt, 1);
            int stockQty = sqlite3_column_int(stmt, 2);

            if (stockQty <= 0) {
                cout << "\nWarning: \"" << name << "\" is currently out of stock at " << userBranch << ".\n";
                cout << "Would you like to:\n1. Try another product\n2. Return to POS menu\nSelect an option: ";
                int choice;
                cin >> choice;
                if (choice == 1) return scanItem();
                else return;
            }

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

void POS::printReceipt() {
    if (cart.empty()) {
        cout << "\n Cannot checkout: your cart is empty.\n";
        return;
    }

    // Step 3: Confirm checkout
    char confirm;
    cout << "\nProceed to checkout and print receipt? (y/n): ";
    cin >> confirm;
    if (tolower(confirm) != 'y') {
        cout << "Checkout cancelled. Returning to POS menu.\n";
        return;
    }

    double total = 0.0;

    logSaleToBranchTable(userBranch, cart);  // ✅ Log the sale only if confirmed

    cout << "\n========== RECEIPT ==========\n";
    for (const auto& item : cart) {
        double itemTotal = item.price * item.quantity;
        cout << item.name << " x" << item.quantity
            << " @ $" << item.price << " = $" << itemTotal << "\n";
        total += itemTotal;

        updateInventory(item.code, item.quantity);  // ✅ Stock deducted only here
    }
    cout << "-------------------------------\n";
    cout << "Total: $" << fixed << setprecision(2) << total << "\n";
    cout << "Branch: " << userBranch << "\n";
    cout << "===============================\n";
    cout << "\n Sale completed for branch: " << userBranch << "\n";
}

// Getter for external receipt/cart preview
std::vector<CartItem> POS::getCart() const {
    return cart;
}

// Full cashier-like flow
void processPointOfSale() {
    string branch = getCurrentUserBranch();
    cout << "\n Starting POS for branch: " << branch << "\n";

    POS pos(branch);
    char choice;

    do {
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
            cout << "Sale completed for branch: " << branch << "\n";
            return;

        case '4':
            cout << "\n Transaction canceled. Cart cleared.\n";
            return;

        default:
            cout << "Invalid option. Try again.\n";
        }

    } while (true);
}

// Inventory deduction for user's branch
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