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

 

    return DB;
}


void closeDatabase(sqlite3* db) {
    if (db != nullptr) {
        sqlite3_close(db);
    }
}
