#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
#include "auth.h"
#include <regex>
using namespace std;

bool isValidDate(const string& date) {
    return regex_match(date, regex(R"(\d{4}-\d{2}-\d{2})")); // YYYY-MM-DD
}

bool isValidTime(const string& time) {
    return regex_match(time, regex(R"(^([01]\d|2[0-3]):([0-5]\d)$)")); // HH:MM 24-hour
}

bool hasExistingShift(sqlite3* db, int empID, const string& date) {
    const char* checkSQL = "SELECT COUNT(*) FROM schedules WHERE Employee_ID = ? AND shift_date = ?";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, checkSQL, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, empID);
    sqlite3_bind_text(stmt, 2, date.c_str(), -1, SQLITE_STATIC);

    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_int(stmt, 0) > 0) {
        exists = true;
    }
    sqlite3_finalize(stmt);
    return exists;
}

void addRoster() {
    if (!isAdmin() && !isStoreManager()) {
        cout << "\u274c You do not have permission to add schedules.\n";
        return;
    }

    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    string repeat;
    do {
        rosterEntry entry;
        string empName, empBranch, empPosition;
        cout << "\n======================================================================\n";
        cout << "                          Add New Schedule\n";
        cout << "======================================================================\n";

        cout << "Employee ID: ";
        cin >> entry.employeeID;

        const char* lookupSQL = "SELECT Employee_Name, branch, Employee_Possition FROM employee WHERE Employee_ID = ?";
        sqlite3_stmt* lookupStmt;
        if (sqlite3_prepare_v2(db, lookupSQL, -1, &lookupStmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(lookupStmt, 1, entry.employeeID);
            if (sqlite3_step(lookupStmt) == SQLITE_ROW) {
                empName = reinterpret_cast<const char*>(sqlite3_column_text(lookupStmt, 0));
                empBranch = reinterpret_cast<const char*>(sqlite3_column_text(lookupStmt, 1));
                empPosition = reinterpret_cast<const char*>(sqlite3_column_text(lookupStmt, 2));
                cout << "Employee Name    : " << empName << endl;
                cout << "Employee Branch  : " << empBranch << endl;
                cout << "Employee Position: " << empPosition << endl;
            }
            else {
                cerr << "❌ Employee ID not found. Cancelling.\n";
                sqlite3_finalize(lookupStmt);
                break;
            }
            sqlite3_finalize(lookupStmt);
        }
        else {
            cerr << "❌ Failed to prepare employee lookup: " << sqlite3_errmsg(db) << endl;
            break;
        }

        do {
            cout << "Shift Date (YYYY-MM-DD): ";
            cin >> entry.shiftDate;
            if (!isValidDate(entry.shiftDate)) cout << "❌ Invalid date format. Try again.\n";
        } while (!isValidDate(entry.shiftDate));

        if (hasExistingShift(db, entry.employeeID, entry.shiftDate)) {
            cout << "❌ This employee already has a shift scheduled on " << entry.shiftDate << ". Try another date.\n";
            continue; // skip the rest and repeat
        }

        do {
            cout << "Start Time (HH:MM): ";
            cin >> entry.startTime;
            if (!isValidTime(entry.startTime)) cout << "❌ Invalid time format. Try again.\n";
        } while (!isValidTime(entry.startTime));

        do {
            cout << "End Time (HH:MM): ";
            cin >> entry.endTime;
            if (!isValidTime(entry.endTime)) cout << "❌ Invalid time format. Try again.\n";
        } while (!isValidTime(entry.endTime));

        cin.ignore();
        cout << "Note (optional, press Enter to skip): ";
        getline(cin, entry.note);

        const char* sql = R"(
            INSERT INTO schedules (
                Employee_ID, Employee_Name, Employee_Possition, shift_date, start_time, end_time, location, note
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        )";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "❌ Failed to prepare insert: " << sqlite3_errmsg(db) << endl;
            break;
        }

        sqlite3_bind_int(stmt, 1, entry.employeeID);
        sqlite3_bind_text(stmt, 2, empName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, empPosition.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, entry.shiftDate.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, entry.startTime.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 6, entry.endTime.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 7, empBranch.c_str(), -1, SQLITE_STATIC);
        entry.note.empty() ? sqlite3_bind_null(stmt, 8) :
            sqlite3_bind_text(stmt, 8, entry.note.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "✅ Schedule added successfully.\n";
        }
        else {
            cerr << "❌ Failed to add schedule: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);

        cout << "Do you want to make another schedule? (y / n): ";
        cin >> repeat;
        cin.ignore();
    } while (repeat == "y" || repeat == "Y");

    closeDatabase(db);
}