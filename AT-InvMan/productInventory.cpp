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


void productList()
{
    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    sqlite3_stmt* stmt = NULL;

    // Filter based on role
    std::string sql;

    int choice = 0;

    cout << endl;
    cout << string(120, '-') << endl;
    cout <<right<<setw(70)<< "Inventory Management\n";
    cout << string(120, '-') << endl;
    cout << endl;
    cout << endl;

    if (isAdmin()) {
        // Administrator — view everything
        sql = "SELECT Product_Code, Product_Name, Catagory, CHCH_Qty, ALK_Qty, WLG_Qty, Product_Price FROM product;";
    }
    else if (isStoreManager()) {
        cout << "View (1) this store or (2) all stores? ";
        cin >> choice;

        if (choice == 1) {
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
        else if (choice == 2) {
            // View everything
            sql = "SELECT Product_Code, Product_Name, Catagory, CHCH_Qty, ALK_Qty, WLG_Qty, Product_Price FROM product;";
        }
        else {
            cout << "Invalid choice.\n";
            closeDatabase(db);
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
            if (choice == 1) {
                if (currentUser.branch == "Christchurch") cout << setw(10) << "CHCH Qty";
                if (currentUser.branch == "Auckland") cout << setw(10) << "ALK Qty";
                if (currentUser.branch == "Wellington") cout << setw(10) << "WLG Qty";
            }
            else if (choice == 2) {
                cout << setw(10) << "CHCH Qty" << setw(10) << "ALK Qty" << setw(10) << "WLG Qty";
            }
        }
        else if (isEmployee()) {
            cout << setw(10) << "Qty";
        }

        cout << setw(30) << "Price" << endl;

        cout << string(120, '-') << endl;

        // Loop through the results
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string code = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string catagory = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

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

    // Product menu afterwards
    int productMenuChoice;
    do {
        cout << "\n========== Inventory Management ==========\n";
        cout << "1. Add New Product\n";
        cout << "2. Update Existing Product\n";
        cout << "3. Delete Product\n";
        cout << "4. Return to Main Menu\n";
        cout << "Select an option: ";
        cin >> productMenuChoice;
        cout << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (productMenuChoice) {
        case 1: addProduct(); break;
        case 2: updateProduct(db); break;
        case 3: deleteProduct(db); break;
        case 4:
            closeDatabase(db);
            return;
        default: cout << "Invalid selection. Please try again.\n";
        }
    } while (true);
}
