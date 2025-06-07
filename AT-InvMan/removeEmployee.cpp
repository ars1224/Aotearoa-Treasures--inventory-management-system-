//Jhon Aries

#include <iostream>
#include <sqlite3.h>
#include "db-conn.h"
#include "employee.h"
using namespace std;

void deleteEmployee() {
    sqlite3* db = connectToDatabase();
    if (db == nullptr) {
        return;
    }

    string empID;
    cout << "Enter Employee ID to delete: ";
    cin >> empID;

    const char* sql = "DELETE FROM employee WHERE Employee_ID = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare DELETE statement: " << sqlite3_errmsg(db) << endl;
        closeDatabase(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, empID.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "Failed to delete employee: " << sqlite3_errmsg(db) << endl;
    }
    else {
        cout << "Employee with ID " << empID << " deleted successfully!" << endl;
    }

    sqlite3_finalize(stmt);
    closeDatabase(db);
}