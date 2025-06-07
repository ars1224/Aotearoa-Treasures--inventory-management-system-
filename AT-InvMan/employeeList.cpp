// Jhon Aries Tayao

#include <iostream>
#include <sqlite3.h>
#include "db-conn.h"
using namespace std;

void employeeList() {
    sqlite3* db = connectToDatabase();
    if (db == nullptr) {
        return; // Failed to connect
    }

    const char* sql = "SELECT * FROM employee;";
    sqlite3_stmt* stmt;

    // Prepare the statement
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        closeDatabase(db);
        return;
    }

    // Execute the query and read the results
    cout << "Employee List:\n";
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* name = sqlite3_column_text(stmt, 1);
        const unsigned char* pos = sqlite3_column_text(stmt, 2);
        const unsigned char* status = sqlite3_column_text(stmt, 3);

        cout << "ID: " << id
            << ", Name: " << name
            << ", Position: " << pos
            << ", Status: " << status << endl;
    }

    sqlite3_finalize(stmt);
    closeDatabase(db);
}
