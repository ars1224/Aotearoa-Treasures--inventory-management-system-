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
