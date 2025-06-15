#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
#include "auth.h"
#include <regex>
using namespace std;

void updateRoster()
{
    int employeeId;

    cout << "Enter Employee ID to update: ";
    cin >> employeeId;

    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    // Check if the schedule exists first
    const char* check = "SELECT Employee_ID FROM schedules WHERE Employee_ID = ?";
    sqlite3_stmt* chkStmt;

    if (sqlite3_prepare_v2(db, check, -1, &chkStmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(chkStmt, 1, employeeId);
        if (sqlite3_step(chkStmt) == SQLITE_ROW) {
            cout << "Schedule found for Employee_ID " << employeeId << ".\n";

            sqlite3_finalize(chkStmt);
        }
        else {
            cout << "❌ Schedule not found for this Employee_ID.\n";
            sqlite3_finalize(chkStmt);
            closeDatabase(db);
            return;
        }
    }
    else {
        cout << "❌ Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        closeDatabase(db);
        return;
    }

    // Provide predefined options for each day
    vector<string> options = { "Opening", "Closing", "Rest", "Leave" };
    string dayNames[7] = { "Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday" };
    string newValues[7];
    for (int i = 0; i < 7; ++i) {
        cout << dayNames[i] << ":\n";
        cout << "  1. Opening\n  2. Closing\n  3. Rest\n  4. Leave\nSelect (1-4): ";
        int choice;
        cin >> choice;

        if (choice < 1 || choice > 4) {
            cout << "Invalid choice. Setting to Rest by default.\n";
            choice = 3;
        }
        newValues[i] = options[choice - 1];
    }

    // Perform the update
    const char* update = R"(
        UPDATE schedules
        SET Monday = ?, Tuesday = ?, Wednesday = ?, Thursday = ?, Friday = ?, Saturday = ?, Sunday = ?
        WHERE Employee_ID = ?
    )";

    sqlite3_stmt* updateStmt = NULL;

    if (sqlite3_prepare_v2(db, update, -1, &updateStmt, NULL) == SQLITE_OK) {
        for (int i = 0; i < 7; ++i) {
            sqlite3_bind_text(updateStmt, i + 1, newValues[i].c_str(), -1, SQLITE_TRANSIENT);
        }
        sqlite3_bind_int(updateStmt, 8, employeeId);

        if (sqlite3_step(updateStmt) == SQLITE_DONE) {
            cout << "✅ Schedule updated successfully!" << endl;
        }
        else {
            cout << "❌ Failed to update schedule!" << endl;
        }
        sqlite3_finalize(updateStmt);
    }
    else {
        cout << "❌ Failed to prepare update!" << endl;
    }

    closeDatabase(db);

    int option;
    cout << "\nRelated options on Employee\n";
    cout << endl;
    cout << "1. Make a new schedule\n";
    cout << "2. Add Employee\n";
    cout << "3. Update Employee\n";
    cout << "4. Delete Employee\n";
    cout << "5. Back to Main Menu\n";
    cout << "6. Exit Program\n";
    cout << endl;
    cout << "Please select an option: ";
    cin >> option;

    switch (option) {
    case 1:
        cout << endl;
        employeeMasterList();
        addRoster();
        break;
    case 2:
        cout << endl;
        addEmployee();
        break;
    case 3:
        cout << endl;
        updateEmployee();
        break;
    case 4:
        cout << endl;
        deleteEmployee();
        break;
    case 5:
        cout << endl;
        mainMenu();
        break;
    case 6:
        return;
        exit(0);
    default:
        cout << "Invalid option. Please try again." << endl;
    }
}

