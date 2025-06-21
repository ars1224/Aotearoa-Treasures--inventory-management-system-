#include "salesLogger.h"
#include <sqlite3.h>
#include <iostream>
#include <ctime>

void logSaleToBranchTable(const std::string& branch, const std::vector<CartItem>& cartItems) {
    sqlite3* db;
    int rc = sqlite3_open("../db/at-invman.db", &db);
    if (rc) {
        std::cerr << "Error opening DB: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // Get current date
    time_t t = time(nullptr);
    tm now;
    localtime_s(&now, &t);  // Safe version
    char date[11];
    strftime(date, sizeof(date), "%d/%m/%Y", &now);

    std::string tableName;
    if (branch == "Auckland") tableName = "AKL_sales";
    else if (branch == "Christchurch") tableName = "CHCH_sales";
    else if (branch == "Wellington") tableName = "WLG_sales";
    else {
        std::cerr << "Invalid branch for sales logging.\n";
        sqlite3_close(db);
        return;
    }

    for (const auto& item : cartItems) {
        std::string sql = "INSERT INTO " + tableName + " (SKU, `Product Name`, `Quantity Sold`, `Sales Date`) VALUES (?, ?, ?, ?);";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, item.code.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, item.name.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 3, item.quantity);
            sqlite3_bind_text(stmt, 4, date, -1, SQLITE_STATIC);

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "Failed to insert sale.\n";
            }

            sqlite3_finalize(stmt);
        }
    }

    sqlite3_close(db);
}
