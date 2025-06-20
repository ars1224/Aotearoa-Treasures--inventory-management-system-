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

void ListOnly() {
    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    sqlite3_stmt* stmt = nullptr;
    std::string sql;
    bool viewAll = false; // To track if viewing all branches

    cout << endl;
    cout << string(120, '-') << endl;
    cout << right << setw(70) << "Inventory Management\n";
    cout << string(120, '-') << endl << endl;

    // Admin can view everything
    if (isAdmin()) {
        viewAll = true;
        sql = "SELECT Product_Code, Product_Name, Catagory, CHCH_Qty, ALK_Qty, WLG_Qty, Product_Price FROM product;";
    }

    // Store Manager: own branch, then ask if they want to see all branches
    else if (isStoreManager() || isEmployee()) {
        if (currentUser.branch == "Christchurch") {
            sql = "SELECT Product_Code, Product_Name, Catagory, CHCH_Qty, Product_Price FROM product;";
        }
        else if (currentUser.branch == "Auckland") {
            sql = "SELECT Product_Code, Product_Name, Catagory, ALK_Qty, Product_Price FROM product;";
        }
        else if (currentUser.branch == "Wellington") {
            sql = "SELECT Product_Code, Product_Name, Catagory, WLG_Qty, Product_Price FROM product;";
        }


        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
            cout << left << setw(12) << "SKU"
                << setw(35) << "Product"
                << setw(17) << "Catagory";

            if (viewAll) {
                cout << setw(10) << "CHCH Qty"
                    << setw(10) << "ALK Qty"
                    << setw(10) << "WLG Qty";
            }
            else {
                cout << setw(10) << "Qty";
            }

            cout << setw(10) << "Price" << endl;
            cout << string(120, '-') << endl;

            while (sqlite3_step(stmt) == SQLITE_ROW) {
                string code = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                string catagory = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

                cout << left << setw(12) << code
                    << setw(35) << name
                    << setw(17) << catagory;

                if (viewAll) {
                    cout << setw(10) << sqlite3_column_int(stmt, 3)
                        << setw(10) << sqlite3_column_int(stmt, 4)
                        << setw(10) << sqlite3_column_int(stmt, 5)
                        << "$" << fixed << setprecision(2) << sqlite3_column_double(stmt, 6) << endl;
                }
                else {
                    cout << setw(10) << sqlite3_column_int(stmt, 3)
                        << "$" << fixed << setprecision(2) << sqlite3_column_double(stmt, 4) << endl;
                }
            }

            sqlite3_finalize(stmt);
        }
        else {
            cerr << "❌ Failed to fetch product list: " << sqlite3_errmsg(db) << endl;
        }
    }
}
