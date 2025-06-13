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

void addRoster()
{
    if (!isAdmin() && !isStoreManager()) {
        cout << "❌ You do not have permission to add schedules.\n";
        return;
    }

    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    string repeat;
    do {
        rosterEntry entry;
        string empName;

        cout << "\n======================================================================\n";
        cout << "                         Add New Schedule\n";
        cout << "======================================================================\n";

        cout << "Employee ID: ";
        cin >> entry.employeeId;

        // Look up employee details first
        const char* lookupSQL = "SELECT Employee_Name FROM employee WHERE Employee_ID = ?";
        sqlite3_stmt* lookupStmt;

        if (sqlite3_prepare_v2(db, lookupSQL, -1, &lookupStmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(lookupStmt, 1, entry.employeeId);
            if (sqlite3_step(lookupStmt) == SQLITE_ROW) {
                empName = reinterpret_cast<const char*>(sqlite3_column_text(lookupStmt, 0));
                cout << "Employee Name: " << empName << endl;
            }
            else {
                cout << "❌ Employee not found.\n";
                sqlite3_finalize(lookupStmt);
                break;
            }
            sqlite3_finalize(lookupStmt);
        }
        else {
            cout << "❌ Failed to prepare statement.\n";
            break;
        }

        // Gather schedules for each day
        cout << "Monday (shift info or leave blank): "; cin.ignore(); getline(cin, entry.monday);
        cout << "Tuesday (shift info or leave blank): "; getline(cin, entry.tuesday);
        cout << "Wednesday (shift info or leave blank): "; getline(cin, entry.wednesday);
        cout << "Thursday (shift info or leave blank): "; getline(cin, entry.thursday);
        cout << "Friday (shift info or leave blank): "; getline(cin, entry.friday);
        cout << "Saturday (shift info or leave blank): "; getline(cin, entry.saturday);
        cout << "Sunday (shift info or leave blank): "; getline(cin, entry.sunday);

        // Insert into schedules
        const char* insertSQL = R"(
            INSERT INTO schedules (Employee_ID, Employee_Name, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
        )";

        sqlite3_stmt* insertStmt;

        if (sqlite3_prepare_v2(db, insertSQL, -1, &insertStmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(insertStmt, 1, entry.employeeId);
            sqlite3_bind_text(insertStmt, 2, empName.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(insertStmt, 3, entry.monday.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(insertStmt, 4, entry.tuesday.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(insertStmt, 5, entry.wednesday.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(insertStmt, 6, entry.thursday.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(insertStmt, 7, entry.friday.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(insertStmt, 8, entry.saturday.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(insertStmt, 9, entry.sunday.c_str(), -1, SQLITE_STATIC);

            if (sqlite3_step(insertStmt) == SQLITE_DONE) {
                cout << "✅ Schedule added successfully.\n";
            }
            else {
                cout << "❌ Failed to add schedule: " << sqlite3_errmsg(db) << endl;
            }

            sqlite3_finalize(insertStmt);
        }
        else {
            cout << "❌ Failed to prepare insert: " << sqlite3_errmsg(db) << endl;
        }

        cout << "Do you want to add another schedule? (y/n): ";
        cin >> repeat;

    } while (repeat == "y" || repeat == "Y");

    closeDatabase(db);
}