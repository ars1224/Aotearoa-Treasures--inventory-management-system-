#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
#include "auth.h"
#include <regex>
using namespace std;

void employeeWithoutSchedule() {
    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    string userBranch;

    // Determine if user is allowed and optionally get their branch
    if (isAdmin()) {
        // Admins can view all branches
    }
    else if (isStoreManager()) {
        userBranch = currentUser.branch;  // You need to implement this function if not done yet
    }
    else {
        cout << "❌ You do not have permission to view this list.\n";
        return;
    }

    // Construct SQL with or without branch filter
    string sql = R"(
        SELECT Employee_ID, Employee_Name, Employee_Possition, branch
        FROM employee
        WHERE Employee_ID NOT IN (SELECT Employee_ID FROM schedules)
    )";

    if (isStoreManager()) {
        sql += " AND branch = ?";
    }

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "❌ Failed to prepare query: " << sqlite3_errmsg(db) << endl;
        return;
    }

    if (isStoreManager()) {
        sqlite3_bind_text(stmt, 1, userBranch.c_str(), -1, SQLITE_TRANSIENT);
    }

    cout << "\n========== Employees Without Schedules ==========\n";
    cout << left << setw(12) << "ID"
        << setw(25) << "Name"
        << setw(20) << "Position"
        << setw(15) << "Branch" << endl;
    cout << string(75, '-') << endl;

    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* position = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* branch = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        cout << left << setw(12) << id
            << setw(25) << name
            << setw(20) << position
            << setw(15) << branch << endl;

        found = true;
    }

    if (!found) {
        cout << "✅ All employees in your scope already have schedules.\n";
    }

    sqlite3_finalize(stmt);
    closeDatabase(db);
}