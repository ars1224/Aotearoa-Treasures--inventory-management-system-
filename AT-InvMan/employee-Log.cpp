#include <iostream>
#include <sqlite3.h>
#include "db-conn.h"
#include "employee.h"
using namespace std;

void logEmployeeAction(sqlite3* db, const string& empID, const string& action, const string& details = "") {
    const char* sql = "INSERT INTO employee_log (Employee_ID, Action, Details) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, empID.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, action.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, details.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            cerr << "Failed to log action: " << sqlite3_errmsg(db) << endl;
        }
        sqlite3_finalize(stmt);
    }
    else {
        cerr << "Failed to prepare log statement: " << sqlite3_errmsg(db) << endl;
    }
}
