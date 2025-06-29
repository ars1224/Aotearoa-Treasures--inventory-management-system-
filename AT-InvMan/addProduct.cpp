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

bool productExists(sqlite3* db, const string& name) {
    const char* checkSQL = "SELECT 1 FROM product WHERE Product_Name = ? LIMIT 1;";
    sqlite3_stmt* stmt;
    bool exists = false;

    if (sqlite3_prepare_v2(db, checkSQL, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            exists = true;
        }

        sqlite3_finalize(stmt);
    }
    return exists;
}

void updateBranchStock(sqlite3* db, const string& name, const string& branch, int qty) {
    string sql;
    if (branch == "Christchurch") sql = "UPDATE product SET CHCH_Qty = CHCH_Qty + ? WHERE Product_Name = ?;";
    else if (branch == "Auckland")    sql = "UPDATE product SET ALK_Qty = ALK_Qty + ? WHERE Product_Name = ?;";
    else if (branch == "Wellington")  sql = "UPDATE product SET WLG_Qty = WLG_Qty + ? WHERE Product_Name = ?;";
    else {
        cout << "⚠ Unknown branch.\n";
        return;
    }

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, qty);
        sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "\u2705 Stock updated for " << branch << " branch.\n";
        }
        else {
            cout << "❌ Failed to update stock: " << sqlite3_errmsg(db) << endl;
        }
        sqlite3_finalize(stmt);
    }
    else {
        cout << "❌ Prepare failed: " << sqlite3_errmsg(db) << endl;
    }
}


void addProduct() {
    char choice;
    do {
        sqlite3* db = connectToDatabase();
        if (db == nullptr) return;

        Product prod;

        cout << "\nEnter Product Details:\n";
        cout << "--------------------------------\n";

        // Name
        cout << "Product Name: ";
        getline(cin >> ws, prod.Product_Name);

        // Check if product already exists
        if (productExists(db, prod.Product_Name)) {
            cout << "\u26A0 Product already exists.\n";

            if (!isAdmin()) {
                int qty;
                cout << "Enter quantity to add to " << currentUser.branch << ": ";
                cin >> qty;
                cin.ignore();

                updateBranchStock(db, prod.Product_Name, currentUser.branch, qty);
                closeDatabase(db);
                return;
            }
            else {
                cout << "Admins should update stock via a separate function (not from here).\n";
                closeDatabase(db);
                return;
            }
        }

        // Continue new product entry
        prod.Product_Code = generateNextSKU();
        cout << "Generated Product Code: " << prod.Product_Code << endl;

        cout << "Category: ";
        getline(cin >> ws, prod.Category);

        prod.CHCH_Qty = 0; prod.ALK_Qty = 0; prod.WLG_Qty = 0;

        if (isAdmin()) {
            cout << "Enter quantity for Christchurch: "; cin >> prod.CHCH_Qty;
            cout << "Enter quantity for Auckland: "; cin >> prod.ALK_Qty;
            cout << "Enter quantity for Wellington: "; cin >> prod.WLG_Qty;
        }
        else if (isStoreManager() || isStockClerk()) {
            cout << "Using store's location: " << currentUser.branch << endl;
            int qty;
            cout << "Enter quantity for " << currentUser.branch << ": ";
            cin >> qty;

            if (currentUser.branch == "Christchurch") prod.CHCH_Qty = qty;
            else if (currentUser.branch == "Auckland") prod.ALK_Qty = qty;
            else if (currentUser.branch == "Wellington") prod.WLG_Qty = qty;
        }

        cout << "Enter product price: $";
        cin >> prod.price;

        const char* sql = "INSERT INTO product (Product_Code, Product_Name, Catagory, ALK_Qty, CHCH_Qty, WLG_Qty, Product_Price) VALUES (?, ?, ?, ?, ?, ?, ?);";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, prod.Product_Code.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, prod.Product_Name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, prod.Category.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 4, prod.ALK_Qty);
            sqlite3_bind_int(stmt, 5, prod.CHCH_Qty);
            sqlite3_bind_int(stmt, 6, prod.WLG_Qty);
            sqlite3_bind_double(stmt, 7, prod.price);

            if (sqlite3_step(stmt) == SQLITE_DONE) {
                cout << "\n\u2705 Product added successfully!\n";
            }
            else {
                cout << "❌ Insert failed: " << sqlite3_errmsg(db) << endl;
            }

            sqlite3_finalize(stmt);
        }
        else {
            cout << "❌ Prepare failed: " << sqlite3_errmsg(db) << endl;
        }

        closeDatabase(db);

        cout << "\nDo you want to add another product? (y/n): ";
        cin >> choice;
        cin.ignore();

    } while (choice == 'y' || choice == 'Y');

    cout << endl;
    productList(); // Refresh the product list
}

string generateNextSKU()
{
    sqlite3* db = connectToDatabase();
    if (db == nullptr) {
        cerr << "Unable to connect to database!" << endl;
        return ""; // Returning an empty string to indicate failure
    }

    sqlite3_stmt* stmt = NULL;

    string nextSKU = "AT0001";

    const char* sql = "SELECT MAX(CAST(SUBSTR(Product_Code, 3) AS INTEGER)) FROM product;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int maxCode = sqlite3_column_int(stmt, 0);
            maxCode++;
            stringstream ss;
            ss << "AT" << setfill('0') << setw(4) << maxCode;
            nextSKU = ss.str();
        }
        else {
            // If there are no rows or something went awry, fallback to AT0001
            cout << "No existing Product_Code found. Starting from AT0001." << endl;
        }
    }
    else {
        cerr << "Failed to prepare SKU query: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    closeDatabase(db);

    return nextSKU;
}