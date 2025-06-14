#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
#include "auth.h"
#include <regex>
using namespace std;

bool hasExistingShiftForDay(sqlite3* db, int empID, const string& dayColumn) {
    string checkSQL = "SELECT " + dayColumn + " FROM schedules WHERE Employee_ID = ?";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, checkSQL.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, empID);

        bool exists = false;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* day = sqlite3_column_text(stmt, 0);
            if (day && strcmp(reinterpret_cast<const char*>(day), "Rest") &&
                strcmp(reinterpret_cast<const char*>(day), "Leave") &&
                strcmp(reinterpret_cast<const char*>(day), "") &&
                strcmp(reinterpret_cast<const char*>(day), " "))
            {
                exists = true;
            }
        }
        sqlite3_finalize(stmt);
        return exists;
    }
    else {
        cout << "❌ Failed to prepare statement to check shift for day: " << sqlite3_errmsg(db) << endl;
        return false;
    }
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

        cout << "\n======================================================================\n";
        cout << "                         Add New Schedule\n";
        cout << "======================================================================\n";

        cout << "Employee ID: "; cin >> entry.employeeId;

        // Look up employee details first
        const char* lookupSQL = "SELECT Employee_Name, Employee_Possition FROM employee WHERE Employee_ID = ?";
        sqlite3_stmt* lookupStmt;

        if (sqlite3_prepare_v2(db, lookupSQL, -1, &lookupStmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(lookupStmt, 1, entry.employeeId);
            if (sqlite3_step(lookupStmt) == SQLITE_ROW) {
                entry.employeeName = reinterpret_cast<const char*>(sqlite3_column_text(lookupStmt, 0)); // Name
                entry.employeePossition = reinterpret_cast<const char*>(sqlite3_column_text(lookupStmt, 1)); // Position
                entry.employeeBranch = reinterpret_cast<const char*>(sqlite3_column_text(lookupStmt, 1)); // Position

                cout << "Employee Name: " << entry.employeeName << endl;
                cout << "Employee Position: " << entry.employeePossition << endl;
                cout << "Employee Branch:  " << entry.employeeBranch << endl;

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

        // Provide predefined options for each day
        vector<string> options = { "Opening", "Closing", "Rest", "Leave" };
        string dayNames[7] = { "Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday" };
        string* dayValues[7] = {
            &entry.monday,
            &entry.tuesday,
            &entry.wednesday,
            &entry.thursday,
            &entry.friday,
            &entry.saturday,
            &entry.sunday
        };

        for (int i = 0; i < 7; ++i) {
            cout << dayNames[i] << ":\n";
            cout << "  1. Opening\n  2. Closing\n  3. Rest\n  4. Leave\nSelect (1-4): ";
            int choice;
            cin >> choice;

            if (choice < 1 || choice > 4) {
                cout << "Invalid choice. Setting to Rest by default.\n";
                choice = 3;
            }
            *dayValues[i] = options[choice - 1];
        }

        // Insert into schedules
        const char* insertSQL = R"(
            INSERT INTO schedules (Employee_ID, Employee_Name, Employee_Possition, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)

        )";

        sqlite3_stmt* insertStmt;

        if (sqlite3_prepare_v2(db, insertSQL, -1, &insertStmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(insertStmt, 1, entry.employeeId);
            sqlite3_bind_text(insertStmt, 2, entry.employeeName.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insertStmt, 3, entry.employeePossition.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insertStmt, 4, entry.employeeBranch.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insertStmt, 5, entry.monday.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insertStmt, 6, entry.tuesday.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insertStmt, 7, entry.wednesday.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insertStmt, 8, entry.thursday.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insertStmt, 9, entry.friday.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insertStmt, 10, entry.saturday.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insertStmt, 11, entry.sunday.c_str(), -1, SQLITE_TRANSIENT);


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