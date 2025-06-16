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


string generateNextSKU(sqlite3* db) {
    string nextSKU = "AT0001";
    const char* sql = "SELECT MAX(CAST(SUBSTR(Product_Code, 3) AS INTEGER)) FROM product;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int maxCode = sqlite3_column_int(stmt, 0);
            maxCode++;
            stringstream ss;
            ss << "AT" << setfill('0') << setw(4) << maxCode;
            nextSKU = ss.str();
        }
    }
    else {
        cerr << "Failed to prepare SKU query: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    return nextSKU;
}

void inventory() {
    char choice;
    sqlite3* db;

    if (sqlite3_open("../db/at-invman.db", &db) != SQLITE_OK) {
        cerr << "Failed to open database: " << sqlite3_errmsg(db) << endl;
        return;
    }

    do {
        cout << "\n========== Inventory Management ==========\n";
        cout << "1. View Product List\n";
        cout << "2. Add New Product\n";
        cout << "3. Update Existing Product\n";
        cout << "4. Delete Product\n";
        cout << "5. Return to Main Menu\n";
        cout << "Select an option: ";
        cin >> choice;
		cout << endl;   
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
        case '1': productList(db); break;
        case '2': addProduct(db); break;
        case '3': updateProduct(db); break;
        case '4': deleteProduct(db); break;
        case '5': sqlite3_close(db); return;
        default: cout << "\u274C Invalid selection. Please try again.\n";
        }
    } while (true);
}

void productList(sqlite3* db) {
    sqlite3_stmt* stmt = NULL;

    // Filter based on role
    string sql;

    int choice = 0;

    if (isAdmin()) {
        // Administrator — view everything
        sql = "SELECT Product_Code, Product_Name, Catagory, CHCH_Qty, ALK_Qty, WLG_Qty, Product_Price FROM product;";
    }
    else if (isStoreManager()) {
        cout << "View (1) this store or (2) all stores? ";
        cin >> choice;

        switch (choice) {
        case 1:
            if (currentUser.branch == "Christchurch") {
                sql = "SELECT Product_Code, Product_Name, Catagory, CHCH_Qty, Product_Price FROM product;";
            }
            else if (currentUser.branch == "Auckland") {
                sql = "SELECT Product_Code, Product_Name, Catagory, ALK_Qty, Product_Price FROM product;";
            }
            else if (currentUser.branch == "Wellington") {
                sql = "SELECT Product_Code, Product_Name, Catagory, WLG_Qty, Product_Price FROM product;";
            }
            break;

        case 2:
            // View everything
            sql = "SELECT Product_Code, Product_Name, Catagory, CHCH_Qty, ALK_Qty, WLG_Qty, Product_Price FROM product;";
            break;

        default:
            cout << "Invalid choice.\n";
            return;
        }
    }

    else if (isEmployee()) {
        // Employees — view their own store only
        if (currentUser.branch == "Christchurch") {
            sql = "SELECT Product_Code, Product_Name, Catagory, CHCH_Qty, Product_Price FROM product;";
        }
        else if (currentUser.branch == "Auckland") {
            sql = "SELECT Product_Code, Product_Name, Catagory, ALK_Qty, Product_Price FROM product;";
        }
        else if (currentUser.branch == "Wellington") {
            sql = "SELECT Product_Code, Product_Name, Catagory, WLG_Qty, Product_Price FROM product;";
        }
    }

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        cout << left << setw(12) << "SKU"
            << setw(35) << "Product"
            << setw(17) << "Catagory";

        if (isAdmin()) {
            cout << setw(10) << "CHCH Qty" << setw(10) << "ALK Qty" << setw(10) << "WLG Qty";
        }
        else if (isStoreManager()) {
            switch (choice){

                case 1:
                if (currentUser.branch == "Christchurch") {
                    cout << setw(10) << "CHCH Qty";
                }
                else if (currentUser.branch == "Auckland") {
                    cout << setw(10) << "ALK Qty";
                }
                else if (currentUser.branch == "Wellington") {
                    cout << setw(10) << "WLG Qty";
                }
				break;
                case 2:
					cout << setw(10) << "CHCH Qty" << setw(10) << "ALK Qty" << setw(10) << "WLG Qty";
            default:
                break;
            }
        }
        else if (isEmployee()) {
            cout << setw(10) << "Qty";
        }

        cout << setw(30) << "Price" << endl;

        cout << string(90, '-') << endl;

        // Loop through the results
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            string code = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            string catagory = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

            cout << left << setw(12) << code << setw(35) << name << setw(17) << catagory;

            if (isAdmin()) {
                cout << setw(10) << sqlite3_column_int(stmt, 3)
                    << setw(10) << sqlite3_column_int(stmt, 4)
                    << setw(10) << sqlite3_column_int(stmt, 5);
                cout << "$" << fixed << setprecision(2) << sqlite3_column_double(stmt, 6) << endl;
            }
            else if (isStoreManager()) {
                if (choice == 2) {
                    cout << setw(10) << sqlite3_column_int(stmt, 3)
                        << setw(10) << sqlite3_column_int(stmt, 4)
                        << setw(10) << sqlite3_column_int(stmt, 5);
                    cout << "$" << fixed << setprecision(2) << sqlite3_column_double(stmt, 6) << endl;
                }
                else if (choice == 1) {
                    cout << setw(10) << sqlite3_column_int(stmt, 3);
                    cout << "$" << fixed << setprecision(2) << sqlite3_column_double(stmt, 4) << endl;
                }
            }
            else if (isEmployee()) {
                cout << setw(10) << sqlite3_column_int(stmt, 3);
                cout << "$" << fixed << setprecision(2) << sqlite3_column_double(stmt, 4) << endl;
            }
        }

        sqlite3_finalize(stmt);
    }
    else {
        cerr << "Failed to fetch product list: " << sqlite3_errmsg(db) << endl;
    }
}







void addProduct(sqlite3* db) {
    string name, catagory, location;
    int qty;
    double price;

    string code = generateNextSKU(db);
    cout << "Generated SKU: " << code << endl;

    cout << "Enter product name: ";
    getline(cin >> ws, name);
    cout << "Enter category: ";
    getline(cin >> ws, catagory);
    cout << "Enter location (CHCH, ALK, WLG): ";
    getline(cin >> ws, location);
    cout << "Enter quantity: ";
    cin >> qty;
    cout << "Enter price: ";
    cin >> price;

    string sql = "INSERT INTO product (Product_Code, Product_Name, Catagory, " + location + "_Qty, Product_Price) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, catagory.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 4, qty);
        sqlite3_bind_double(stmt, 5, price);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "\u2705 Product added successfully!\n";
        }
        else {
            cerr << "\u274C Insertion failed: " << sqlite3_errmsg(db) << endl;
        }
        sqlite3_finalize(stmt);
    }
    else {
        cerr << "Failed to prepare insert: " << sqlite3_errmsg(db) << endl;
    }
}

void updateProduct(sqlite3* db) {
    string code;
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
        cout << "6. Cancel / Exit\n";
        cout << "Select an option (1-6): ";
        cin >> choice;
        cin.ignore();

        sqlite3_stmt* stmt = NULL;
        string sql;

        int result;

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

                continue;
            }

            string fromLoc, toLoc;
            int transferQty;

            if (isStoreManager()) {
                // Store Manager's from location is their own store
                fromLoc = currentUser.branch;
                cout << "Transferring FROM your store: " << fromLoc << endl;

                cout << "Enter quantity to transfer: ";
                cin >> transferQty;

                // Store Manager can transfer within their own store (depending on your policy).
                // So normally TO should be the SAME store.
                toLoc = currentUser.branch;

            }
            else if (isAdmin()) {
                cout << "Enter location to transfer FROM (CHCH, ALK, WLG): ";
                cin >> fromLoc;

                cout << "Enter location to transfer TO (CHCH, ALK, WLG): ";
                cin >> toLoc;

                if (fromLoc == toLoc) {
                    cout << "Source and destination must be different.\n";
                    continue;
                }

                cout << "Enter quantity to transfer: ";
                cin >> transferQty;
            }

            // Determine column names
            string fromCol, toCol;

            if (fromLoc == "Christchurch") fromCol = "CHCH_Qty";
            else if (fromLoc == "AucklandALK") fromCol = "ALK_Qty";
            else if (fromLoc == "Wellington") fromCol = "WLG_Qty";

            if (toLoc == "Christchurch") toCol = "CHCH_Qty";
            else if (toLoc == "Auckland") toCol = "ALK_Qty";
            else if (toLoc == "Wellington") toCol = "WLG_Qty";

            // Check available first
            string fetchSQL = "SELECT " + fromCol + ", " + toCol + " FROM product WHERE Product_Code = ?;";
            sqlite3_prepare_v2(db, fetchSQL.c_str(), -1, &stmt, NULL);
            sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_TRANSIENT);

            int fromStock = 0, toStock = 0;

            if (sqlite3_step(stmt) == SQLITE_ROW) {
                fromStock = sqlite3_column_int(stmt, 0);
                toStock = sqlite3_column_int(stmt, 1);
            }
            else {
                cout << "Product not found.\n";
                sqlite3_finalize(stmt);
                continue;
            }
            sqlite3_finalize(stmt);

            if (transferQty > fromStock) {
                cout << "Not enough stock in " << fromLoc << ". Available: " << fromStock << endl;
                continue;
            }

            int newFromStock = fromStock - transferQty;
            int newToStock = toStock + transferQty;

            string updateSQL = "UPDATE product SET " + fromCol + " = ?, " + toCol + " = ? WHERE Product_Code = ?;";
            sqlite3_prepare_v2(db, updateSQL.c_str(), -1, &stmt, NULL);
            sqlite3_bind_int(stmt, 1, newFromStock);
            sqlite3_bind_int(stmt, 2, newToStock);
            sqlite3_bind_text(stmt, 3, code.c_str(), -1, SQLITE_TRANSIENT);

            if (sqlite3_step(stmt) == SQLITE_DONE) {
                cout << "Transfer successful.\n";
            }
            else {
                cout << "Transfer failed: " << sqlite3_errmsg(db) << endl;
            }

            sqlite3_finalize(stmt);
            break;
        }

        case '6':
            return;

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

void deleteProduct(sqlite3* db) {
    string code;
    cout << "Enter product code to delete: ";
    cin >> code;

    string sql = "DELETE FROM product WHERE Product_Code = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "\u2705 Product deleted.\n";
        }
        else {
            cerr << "\u274C Delete failed: " << sqlite3_errmsg(db) << endl;
        }
        sqlite3_finalize(stmt);
    }
    else {
        cerr << "Failed to prepare delete: " << sqlite3_errmsg(db) << endl;
    }
}
