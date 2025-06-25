// Dhona created this snippet to implement the POS system
// This function logs each completed transaction into a branch-specific sales table
// Used to maintain a history of sales for future reporting and analysis

#include "salesLogger.h"
#include "pos.h"
#include <sqlite3.h>
#include <ctime>
#include <iostream>
using namespace std;

void logSaleToBranchTable(const std::string& branch, const std::vector<CartItem>& cartItems) {
    sqlite3* db;

    // Open connection to the SQLite database
    sqlite3_open("../db/at-invman.db", &db);

    // Determine which sales table to use based on the branch
    std::string tableName;
    if (branch == "Auckland") tableName = "AKLSales";
    else if (branch == "Christchurch") tableName = "CHCHSales";
    else if (branch == "Wellington") tableName = "WLGSales";
    else return;

    // Get the current date in the format DD/MM/YYYY
    time_t t = time(nullptr);
    tm now;
    localtime_s(&now, &t);
    char date[11];
    strftime(date, sizeof(date), "%d/%m/%Y", &now);

    // Loop through each item in the cart to log individual sales entries
    for (const auto& item : cartItems) {
        // SQL insert statement for logging the sale
        std::string sql = "INSERT INTO " + tableName + " (SKU, 'Product Name', 'Quantity Sold', 'Sales Date') VALUES (?, ?, ?, ?);";
        sqlite3_stmt* stmt;

        // Prepare and bind values safely
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, item.code.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, item.name.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 3, item.quantity);
            sqlite3_bind_text(stmt, 4, date, -1, SQLITE_STATIC);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
    }
    // Close the database connection
    sqlite3_close(db);
}
