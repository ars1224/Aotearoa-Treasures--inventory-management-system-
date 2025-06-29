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

void updateProduct() {
    ListOnly();
    string code;
    cout << "\nEnter product code to update (e.g., AT0001): ";
    cin >> code;

    // Check if product exists
    sqlite3* dbCheck = connectToDatabase();
    if (dbCheck == nullptr) return;
    const char* checkSQL = "SELECT 1 FROM product WHERE Product_Code = ?;";
    sqlite3_stmt* checkStmt;
    bool exists = false;

    if (sqlite3_prepare_v2(dbCheck, checkSQL, -1, &checkStmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(checkStmt, 1, code.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(checkStmt) == SQLITE_ROW) {
            exists = true;
        }
        sqlite3_finalize(checkStmt);
    }
    else {
        cout << "❌ Failed to check product existence: " << sqlite3_errmsg(dbCheck) << endl;
        closeDatabase(dbCheck);
        return;
    }

    closeDatabase(dbCheck);

    if (!exists) {
        cout << "❌ Product code not found.\n";
        return;
    }

    // Main update menu
    char choice;
    do {
        cout << "\nWhat would you like to update?\n";
        cout << "1. Product Name\n";
        cout << "2. Category\n";
        cout << "3. Price\n";
        cout << "4. Quantity at a specific location\n";
        cout << "5. Transfer stock between locations\n";
        cout << "6. Back To Inventory Menu\n";
        cout << "7. Back To Main Menu\n";
        cout << "Select an option (1-7): ";
        cin >> choice;
        cin.ignore();

        sqlite3* db = connectToDatabase();
        if (db == nullptr) return;

        sqlite3_stmt* stmt = nullptr;
        string sql;
        bool execute = false;

        switch (choice) {
        case '1': {
            string newName;
            cout << "Enter new product name: ";
            getline(cin, newName);

            sql = "UPDATE product SET Product_Name = ? WHERE Product_Code = ?;";
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
                sqlite3_bind_text(stmt, 1, newName.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 2, code.c_str(), -1, SQLITE_TRANSIENT);
                execute = true;
            }
            break;
        }

        case '2': {
            string newCategory;
            cout << "Enter new category: ";
            getline(cin, newCategory);

            sql = "UPDATE product SET Catagory = ? WHERE Product_Code = ?;";
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
                sqlite3_bind_text(stmt, 1, newCategory.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 2, code.c_str(), -1, SQLITE_TRANSIENT);
                execute = true;
            }
            break;
        }

        case '3': {
            if (!isAdmin() && !isStoreManager()) {
                cout << "❌ Price update is only allowed for Administrator or Store Manager.\n";
                break;
            }

            double newPrice;
            cout << "Enter new price: ";
            cin >> newPrice;

            sql = "UPDATE product SET Product_Price = ? WHERE Product_Code = ?;";
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
                sqlite3_bind_double(stmt, 1, newPrice);
                sqlite3_bind_text(stmt, 2, code.c_str(), -1, SQLITE_TRANSIENT);
                execute = true;
            }
            break;
        }

        case '4': {
            string column;
            int newQty;

            if (isAdmin()) {
                string location;
                cout << "Enter location to update (Christchurch, Auckland, Wellington): ";
                cin >> location;

                if (location == "Christchurch") column = "CHCH_Qty";
                else if (location == "Auckland") column = "ALK_Qty";
                else if (location == "Wellington") column = "WLG_Qty";
                else {
                    cout << "❌ Invalid location.\n";
                    break;
                }
            }
            else {
                if (currentUser.branch == "Christchurch") column = "CHCH_Qty";
                else if (currentUser.branch == "Auckland") column = "ALK_Qty";
                else if (currentUser.branch == "Wellington") column = "WLG_Qty";
                else {
                    cout << "❌ Invalid branch.\n";
                    break;
                }
            }

            cout << "Enter new quantity: ";
            cin >> newQty;

            sql = "UPDATE product SET " + column + " = ? WHERE Product_Code = ?;";
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
                sqlite3_bind_int(stmt, 1, newQty);
                sqlite3_bind_text(stmt, 2, code.c_str(), -1, SQLITE_TRANSIENT);
                execute = true;
            }
            break;
        }

        case '5': {
            if (!isAdmin() && !isStoreManager()) {
                cout << "❌ Transfer allowed only for Administrator or Store Manager.\n";
                break;
            }

            string fromLoc, toLoc;
            int transferQty;

            cout << "Enter FROM location (Christchurch, Auckland, Wellington): ";
            cin >> fromLoc;
            cout << "Enter TO location (Christchurch, Auckland, Wellington): ";
            cin >> toLoc;

            if (fromLoc == toLoc) {
                cout << "❌ Cannot transfer within the same location.\n";
                break;
            }

            cout << "Enter quantity to transfer: ";
            cin >> transferQty;

            string fromCol, toCol;
            if (fromLoc == "Christchurch") fromCol = "CHCH_Qty";
            else if (fromLoc == "Auckland") fromCol = "ALK_Qty";
            else if (fromLoc == "Wellington") fromCol = "WLG_Qty";
            else {
                cout << "❌ Invalid FROM location.\n";
                break;
            }

            if (toLoc == "Christchurch") toCol = "CHCH_Qty";
            else if (toLoc == "Auckland") toCol = "ALK_Qty";
            else if (toLoc == "Wellington") toCol = "WLG_Qty";
            else {
                cout << "❌ Invalid TO location.\n";
                break;
            }

            // Fetch current stock
            string fetchSQL = "SELECT " + fromCol + ", " + toCol + " FROM product WHERE Product_Code = ?;";
            if (sqlite3_prepare_v2(db, fetchSQL.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
                cerr << "❌ Failed to prepare select: " << sqlite3_errmsg(db) << endl;
                break;
            }

            sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_TRANSIENT);

            int fromStock = 0, toStock = 0;
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                fromStock = sqlite3_column_int(stmt, 0);
                toStock = sqlite3_column_int(stmt, 1);
            }
            else {
                cout << "❌ Product not found.\n";
                sqlite3_finalize(stmt);
                break;
            }
            sqlite3_finalize(stmt);

            if (transferQty > fromStock) {
                cout << "❌ Not enough stock in " << fromLoc << ". Available: " << fromStock << endl;
                break;
            }

            int newFrom = fromStock - transferQty;
            int newTo = toStock + transferQty;

            string updateSQL = "UPDATE product SET " + fromCol + " = ?, " + toCol + " = ? WHERE Product_Code = ?;";
            if (sqlite3_prepare_v2(db, updateSQL.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
                sqlite3_bind_int(stmt, 1, newFrom);
                sqlite3_bind_int(stmt, 2, newTo);
                sqlite3_bind_text(stmt, 3, code.c_str(), -1, SQLITE_TRANSIENT);
                execute = true;
            }
            break;
        }

        case '6':
            closeDatabase(db);
            inventory();
            return;

        case '7':
            closeDatabase(db);
            mainMenu();
            return;

        default:
            cout << "❌ Invalid choice. Try again.\n";
            closeDatabase(db);
            continue;
        }

        if (execute) {
            if (sqlite3_step(stmt) == SQLITE_DONE) {
                cout << "✅ Update successful.\n";
            }
            else {
                cout << "❌ Update failed: " << sqlite3_errmsg(db) << endl;
            }
            sqlite3_finalize(stmt);
        }

        closeDatabase(db);
        ListOnly();

    } while (true);
}
