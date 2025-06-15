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
    const char* sql = "SELECT Product_Code, Product_Name, Catagory, CHCH_Qty, ALK_Qty, WLG_Qty, Product_Price FROM product;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to fetch product list: " << sqlite3_errmsg(db) << endl;
        return;
    }

    cout << "\n=== Product List ===\n";
    cout << left << setw(12) << "SKU"
        << setw(35) << "Product"
        << setw(17) << "Catagory"
        << setw(10) << "CHCH_Qty"
        << setw(10) << "ALK_Qty"
        << setw(10) << "WLG_Qty"
        << setw(10) << "Price" << endl;
    cout << string(90, '-') << endl;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string code = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string catagory = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int chch = sqlite3_column_int(stmt, 3);
        int alk = sqlite3_column_int(stmt, 4);
        int wlg = sqlite3_column_int(stmt, 5);
        double price = sqlite3_column_double(stmt, 6);

        cout << left << setw(12) << code
            << setw(35) << name
            << setw(17) << catagory
            << setw(10) << chch
            << setw(10) << alk
            << setw(10) << wlg
            << "$" << fixed << setprecision(2) << price << endl;
    }
    sqlite3_finalize(stmt);
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

        sqlite3_stmt* stmt;
        string sql;
        int result;

        switch (choice) {
        case '1': {
            string newName;
            cout << "Enter new product name: ";
            getline(cin, newName);

            sql = "UPDATE product SET Product_Name = ? WHERE Product_Code = ?;";
            sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            sqlite3_bind_text(stmt, 1, newName.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, code.c_str(), -1, SQLITE_TRANSIENT);
            break;
        }
        case '2': {
            string newCategory;
            cout << "Enter new category: ";
            getline(cin, newCategory);

            sql = "UPDATE product SET Catagory = ? WHERE Product_Code = ?;";
            sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            sqlite3_bind_text(stmt, 1, newCategory.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, code.c_str(), -1, SQLITE_TRANSIENT);
            break;
        }
        case '3': {
            double newPrice;
            cout << "Enter new price: ";
            cin >> newPrice;

            sql = "UPDATE product SET Product_Price = ? WHERE Product_Code = ?;";
            sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            sqlite3_bind_double(stmt, 1, newPrice);
            sqlite3_bind_text(stmt, 2, code.c_str(), -1, SQLITE_TRANSIENT);
            break;
        }
        case '4': {
            string location;
            int newQty;
            cout << "Enter location to update (CHCH, AKL, WLG): ";
            cin >> location;

            string column;
            if (location == "CHCH") column = "CHCH_Qty";
            else if (location == "ALK") column = "ALK_Qty";
            else if (location == "WLG") column = "WLG_Qty";
            else {
                cout << "Invalid location.\n";
                continue;
            }

            cout << "Enter new quantity for " << location << ": ";
            cin >> newQty;

            sql = "UPDATE product SET " + column + " = ? WHERE Product_Code = ?;";
            sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            sqlite3_bind_int(stmt, 1, newQty);
            sqlite3_bind_text(stmt, 2, code.c_str(), -1, SQLITE_TRANSIENT);
            break;
        }
        case '5': {
            string fromLoc, toLoc;
            int transferQty;

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

            string fromCol, toCol;
            if (fromLoc == "CHCH") fromCol = "CHCH_Qty";
            else if (fromLoc == "ALK") fromCol = "ALK_Qty";
            else if (fromLoc == "WLG") fromCol = "WLG_Qty";
            else {
                cout << "Invalid source location.\n";
                continue;
            }

            if (toLoc == "CHCH") toCol = "CHCH_Qty";
            else if (toLoc == "ALK") toCol = "ALK_Qty";
            else if (toLoc == "WLG") toCol = "WLG_Qty";
            else {
                cout << "Invalid destination location.\n";
                continue;
            }

            // Fetch current stock for validation
            string fetchSQL = "SELECT " + fromCol + ", " + toCol + " FROM product WHERE Product_Code = ?;";
            sqlite3_prepare_v2(db, fetchSQL.c_str(), -1, &stmt, nullptr);
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
            sqlite3_prepare_v2(db, updateSQL.c_str(), -1, &stmt, nullptr);
            sqlite3_bind_int(stmt, 1, newFromStock);
            sqlite3_bind_int(stmt, 2, newToStock);
            sqlite3_bind_text(stmt, 3, code.c_str(), -1, SQLITE_TRANSIENT);

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
