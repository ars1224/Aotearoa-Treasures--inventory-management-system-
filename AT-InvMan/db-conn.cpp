#include "db-conn.h"
#include <iostream>

sqlite3* connectToDatabase() {
    sqlite3* DB;
    std::string dbPath = "../db/at-invman.db";

    int exit = sqlite3_open(dbPath.c_str(), &DB);
    if (exit) {
        std::cerr << "Error opening DB: " << sqlite3_errmsg(DB) << std::endl;
        return nullptr;
    }

    // ⏱️ Allow SQLite to wait up to 5 seconds if the DB is locked
    sqlite3_busy_timeout(DB, 5000);

    return DB;
}


void closeDatabase(sqlite3* db) {
    if (db != nullptr) {
        sqlite3_close(db);
    }
}
