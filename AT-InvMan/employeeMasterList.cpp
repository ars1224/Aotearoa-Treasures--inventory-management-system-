// Jhon Aries Tayao

#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
#include "auth.h"
using namespace std;

void employeeMasterList() {
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
    cout << endl;
    cout << "    " << "ID  " << "    " << " Name   " << "    " << " Position  " << "    " << " Status   " << "    " << " Branch   " << endl;
    cout << endl;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* name = sqlite3_column_text(stmt, 1);
        const unsigned char* pos = sqlite3_column_text(stmt, 2);
        const unsigned char* status = sqlite3_column_text(stmt, 3);
        const unsigned char* branch = sqlite3_column_text(stmt, 5);


        cout << "    " << id << "    " << "    " << name << "    " << "    " << pos << "    " << "    " << status << "    " << "    " << branch << "    " << "    " << endl;
        cout << "_________________________________________________________________________________" << endl;
        cout << endl;

    }

    sqlite3_finalize(stmt);
    closeDatabase(db);
}