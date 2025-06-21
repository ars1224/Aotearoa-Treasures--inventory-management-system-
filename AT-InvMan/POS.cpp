#include "POS.h"
#include <sqlite3.h>
#include <iostream>
#include <iomanip>

using namespace std;

POS::POS() {
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

    string sql = "SELECT Product_Name, Product_Price FROM product WHERE Product_Code = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            double price = sqlite3_column_double(stmt, 1);

            int qty;
            cout << "Product: " << name << ", Price: $" << fixed << setprecision(2) << price << endl;
            cout << "Enter quantity: ";
            cin >> qty;

            cart.push_back({ code, name, price, qty });
            cout << "Item added to cart.\n";
        }
        else {
            cout << "Product not found.\n";
        }

        sqlite3_finalize(stmt);
    }
    else {
        cerr << "Failed to prepare statement.\n";
    }
}

void POS::printReceipt() {
    double total = 0.0;

    cout << "\n========== RECEIPT ==========\n";
    for (const auto& item : cart) {
        double itemTotal = item.price * item.quantity;
        cout << item.name << " x" << item.quantity
            << " @ $" << item.price << " = $" << itemTotal << "\n";
        total += itemTotal;
    }
    cout << "-----------------------------\n";
    cout << "Total: $" << fixed << setprecision(2) << total << "\n";
    cout << "=============================\n";
}

void testPOS() {
    POS pos;

    char more = 'y';
    while (more == 'y' || more == 'Y') {
        pos.scanItem();
        cout << "Add another item? (y/n): ";
        cin >> more;
    }

    pos.printReceipt();  // Even if empty for now
}



//TESTING ONLY
/*#include <iostream>
using namespace std;

void testPOS() {
    cout << "POS module is running correctly!" << endl;
}*/