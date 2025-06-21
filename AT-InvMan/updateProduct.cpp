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
    cout << endl;
    cout << "Enter product code to update (e.g., AT0001): ";
    cin >> code;

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
        cout << "Select an option above (1-7): ";
        cin >> choice;
        cin.ignore();
        cout << endl;

        sqlite3* db = connectToDatabase();
        if (db == nullptr) return;

        sqlite3_stmt* stmt = nullptr;
        std::string sql;

        switch (choice) {
        case '1': {
            string newName;
            cout << "Enter new product name: ";
            getline(cin, newName);

            sql = "UPDATE product SET Product_Name = ? WHERE Product_Code = ?;";
            sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
            sqlite3_bind_text(stmt, 1, newName.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, code.c_str(), -1, SQLITE_TRANSIENT);
            break;
        }
        case '2': {
            string newCategory;
            cout << "Enter new category: ";
            getline(cin, newCategory);

            sql = "UPDATE product SET Catagory = ? WHERE Product_Code = ?;";
            sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
            sqlite3_bind_text(stmt, 1, newCategory.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, code.c_str(), -1, SQLITE_TRANSIENT);
            break;
        }
        case '3': {
            double newPrice;
            cout << "Enter new price: ";
            cin >> newPrice;

            sql = "UPDATE product SET Product_Price = ? WHERE Product_Code = ?;";
            sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
            sqlite3_bind_double(stmt, 1, newPrice);
            sqlite3_bind_text(stmt, 2, code.c_str(), -1, SQLITE_TRANSIENT);
            cout << endl;
            ListOnly();
            break;
        }
        case '4': {
            int newQty;

            string column;

            if (isAdmin()) {
                // Administrator can choose which location
                string location;
                cout << "Enter location to update (CHCH, ALK, WLG): ";
                cin >> location;

                if (location == "Christchurch") column = "CHCH_Qty";
                else if (location == "Auckland") column = "ALK_Qty";
                else if (location == "Wellington") column = "WLG_Qty";
                else {
                    cout << "Invalid location.\n";
                    continue;
                }
            }
            else if (isStoreManager() ||
                isEmployee()) {
                // Store manager or employee can ONLY modify their own branch
                if (currentUser.branch == "Christchurch") column = "CHCH_Qty";
                else if (currentUser.branch == "Auckland") column = "ALK_Qty";
                else if (currentUser.branch == "Wellington") column = "WLG_Qty";

                cout << "Enter new quantity for " << currentUser.branch << ": ";
                cin >> newQty;

                // Now prepare the statement
                sql = "UPDATE product SET " + column + " = ? WHERE Product_Code = ?;";
                sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
                sqlite3_bind_int(stmt, 1, newQty);
                sqlite3_bind_text(stmt, 2, code.c_str(), -1, SQLITE_TRANSIENT);
                break;

            }

            cout << "Enter new quantity: ";
            cin >> newQty;

            sql = "UPDATE product SET " + column + " = ? WHERE Product_Code = ?;";
            sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
            sqlite3_bind_int(stmt, 1, newQty);
            sqlite3_bind_text(stmt, 2, code.c_str(), -1, SQLITE_TRANSIENT);
            break;

        }

        case '5': {
            if (!isAdmin() && !isStoreManager()) {
                cout << "Transfer is only allowed for Administrator or Store Manager.\n";
                return;
            }

            string fromLoc, toLoc;
            int transferQty = 0;

            if (isStoreManager()) {
                fromLoc = currentUser.branch;
                toLoc = currentUser.branch;

                cout << "Transferring FROM your store: " << fromLoc << endl;
                cout << "Enter quantity to transfer within your store: ";
                cin >> transferQty;
            }
            else if (isAdmin()) {
                cout << "Enter location to transfer FROM (Christchurch, Auckland, Wellington): ";
                cin >> fromLoc;

                cout << "Enter location to transfer TO (Christchurch, Auckland, Wellington): ";
                cin >> toLoc;

                if (fromLoc == toLoc) {
                    cout << "Source and destination must be different.\n";
                    return;
                }

                cout << "Enter quantity to transfer: ";
                cin >> transferQty;
            }

            // Map to correct DB column names
            string fromCol, toCol;
            if (fromLoc == "Christchurch") fromCol = "CHCH_Qty";
            else if (fromLoc == "Auckland") fromCol = "ALK_Qty";
            else if (fromLoc == "Wellington") fromCol = "WLG_Qty";
            else {
                cout << "Invalid 'from' location.\n";
                return;
            }

            if (toLoc == "Christchurch") toCol = "CHCH_Qty";
            else if (toLoc == "Auckland") toCol = "ALK_Qty";
            else if (toLoc == "Wellington") toCol = "WLG_Qty";
            else {
                cout << "Invalid 'to' location.\n";
                return;
            }

            // Fetch current stock
            string fetchSQL = "SELECT " + fromCol + ", " + toCol + " FROM product WHERE Product_Code = ?;";
            sqlite3_stmt* stmt = nullptr;

            if (sqlite3_prepare_v2(db, fetchSQL.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
                cerr << "Failed to prepare select: " << sqlite3_errmsg(db) << endl;
                return;
            }

            sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_TRANSIENT);

            int fromStock = 0, toStock = 0;
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                fromStock = sqlite3_column_int(stmt, 0);
                toStock = sqlite3_column_int(stmt, 1);
            }
            else {
                cout << "Product not found.\n";
                sqlite3_finalize(stmt);
                return;
            }

            sqlite3_finalize(stmt);

            if (transferQty > fromStock) {
                cout << "Not enough stock in " << fromLoc << ". Available: " << fromStock << endl;
                return;
            }

            int newFromStock = fromStock - transferQty;
            int newToStock = toStock + transferQty;

            string updateSQL = "UPDATE product SET " + fromCol + " = ?, " + toCol + " = ? WHERE Product_Code = ?;";
            if (sqlite3_prepare_v2(db, updateSQL.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
                cerr << "Failed to prepare update: " << sqlite3_errmsg(db) << endl;
                return;
            }

            sqlite3_bind_int(stmt, 1, newFromStock);
            sqlite3_bind_int(stmt, 2, newToStock);
            sqlite3_bind_text(stmt, 3, code.c_str(), -1, SQLITE_TRANSIENT);

            if (sqlite3_step(stmt) == SQLITE_DONE) {
                cout << "Transfer successful.\n";
            }
            else {
                cerr << "Transfer failed: " << sqlite3_errmsg(db) << endl;
            }

            sqlite3_finalize(stmt);
        }

        cout << endl;
        ListOnly();

        case '6':
            cout << endl;
            inventory();
			break;

        case '7':
            cout << endl;
            mainMenu();

        default:
            cout << "Invalid choice. Try again.\n";
            continue;
        }

        // Execute update
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Update successful.\n";
        }
        else {
            cout << "Update failed: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);

    } while (true);
}