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

// Safely handle possible NULL values
const char* safeText(const unsigned char* text) {
    return text ? reinterpret_cast<const char*>(text) : "";
}

void rosterlist() {
    sqlite3* db = connectToDatabase();
    if (db == NULL) return;

    string baseSQL = R"(
        SELECT schedules.Employee_ID, schedules.Employee_Name, 
               schedules.Employee_Possition, schedules.branch, 
               schedules.Monday, schedules.Tuesday, schedules.Wednesday, 
               schedules.Thursday, schedules.Friday, schedules.Saturday, schedules.Sunday
        FROM schedules
    )";

    sqlite3_stmt* stmt;
    string whereClause;
    bool hasCondition = false;

    // Role-based filtering
    if (isStoreManager()) {
        hasCondition = true;
        whereClause = "WHERE schedules.branch = ?";
    }
    else if (isEmployee()) {
        hasCondition = true;
        whereClause = "WHERE schedules.Employee_ID = ?";
    }

    string finalSQL = baseSQL + (hasCondition ? (" " + whereClause) : "") + ";";

    // Prepare statement
    if (sqlite3_prepare_v2(db, finalSQL.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "❌ Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        closeDatabase(db);
        return;
    }

    // Header
    cout << string(140, '-') << endl;
    cout << right << setw(85) << "Employee Roster Schedule" << endl;
    cout << string(140, '-') << endl;
    cout << right << setw(87) << "Opening Shift: 7am to 3:30pm\n";
    cout << right << setw(88) << "Closing Shift: 10:30am to 7pm\n\n";
    cout << string(140, '-') << endl;

    // Column titles
    cout << left
        << setw(8) << "EmpID"
        << setw(12) << "Name"
        << setw(20) << "Position"
        << setw(15) << "Branch"
        << setw(12) << "Monday"
        << setw(12) << "Tuesday"
        << setw(12) << "Wednesday"
        << setw(12) << "Thursday"
        << setw(12) << "Friday"
        << setw(12) << "Saturday"
        << setw(12) << "Sunday"
        << endl;

    cout << string(140, '-') << endl;

    bool hasData = false;

    // Fetch and print rows
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        hasData = true;

        int empId = sqlite3_column_int(stmt, 0);
        const char* name = safeText(sqlite3_column_text(stmt, 1));
        const char* position = safeText(sqlite3_column_text(stmt, 2));
        const char* branch = safeText(sqlite3_column_text(stmt, 3));
        const char* monday = safeText(sqlite3_column_text(stmt, 4));
        const char* tuesday = safeText(sqlite3_column_text(stmt, 5));
        const char* wednesday = safeText(sqlite3_column_text(stmt, 6));
        const char* thursday = safeText(sqlite3_column_text(stmt, 7));
        const char* friday = safeText(sqlite3_column_text(stmt, 8));
        const char* saturday = safeText(sqlite3_column_text(stmt, 9));
        const char* sunday = safeText(sqlite3_column_text(stmt, 10));

        cout << left
            << setw(8) << empId
            << setw(12) << name
            << setw(20) << position
            << setw(15) << branch
            << setw(12) << monday
            << setw(12) << tuesday
            << setw(12) << wednesday
            << setw(12) << thursday
            << setw(12) << friday
            << setw(12) << saturday
            << setw(12) << sunday
            << endl;
    }

    if (!hasData) {
        cout << "\n No schedules found for your role.\n";
    }

    sqlite3_finalize(stmt);
    cout << string(140, '-') << endl;
    closeDatabase(db);
}