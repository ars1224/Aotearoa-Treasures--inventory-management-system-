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
