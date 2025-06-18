#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
#include "auth.h"
#include <limits>
#include <sstream>
#include "InventoryManager.h"

using namespace std;

void addProduct()
{
    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    sqlite3_stmt* stmt = NULL;

    string name, catagory;
    int qtyCHCH = 0;
    int qtyALK = 0;
    int qtyWLG = 0;
    double price = 0.0;

    string code = generateNextSKU();
    cout << "Generated SKU: " << code << endl;

    cout << "Enter product name: ";
    getline(cin >> ws, name);

    cout << "Enter category: ";
    getline(cin >> ws, catagory);

    if (isAdmin()) {
        cout << "Enter quantity for CHCH: "; cin >> qtyCHCH;
        cout << "Enter quantity for ALK: "; cin >> qtyALK;
        cout << "Enter quantity for WLG: "; cin >> qtyWLG;
    }
    else if (isStoreManager()) {
        cout << "Using store's location: " << currentUser.branch << endl;

        cout << "Enter quantity for " << currentUser.branch << ": ";
        int qty = 0;
        cin >> qty;

        if (currentUser.branch == "CHCH") qtyCHCH = qty;
        else if (currentUser.branch == "ALK") qtyALK = qty;
        else if (currentUser.branch == "WLG") qtyWLG = qty;
    }
    else {
        cout << "You do not have permission to add products.\n";
        closeDatabase(db);
        return;
    }

    cout << "Enter price: ";
    cin >> price;

    // Prepare the SQL statement with parameter binding
    string sql = "INSERT INTO product (Product_Code, Product_Name, Catagory, ALK_Qty, CHCH_Qty, WLG_Qty, Product_Price) VALUES (?, ?, ?, ?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, catagory.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 4, qtyALK);
        sqlite3_bind_int(stmt, 5, qtyCHCH);
        sqlite3_bind_int(stmt, 6, qtyWLG);
        sqlite3_bind_double(stmt, 7, price);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Product added successfully!" << endl;
            inventory();
        }
        else {
            cout << "Insertion failed: " << sqlite3_errmsg(db) << endl;
        }
        sqlite3_finalize(stmt);
    }
    else {
        cout << "Failed to prepare insert: " << sqlite3_errmsg(db) << endl;
    }

    closeDatabase(db);
}


