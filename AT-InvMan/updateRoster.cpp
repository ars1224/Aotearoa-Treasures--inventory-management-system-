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
    int scheduleId;

    cout << "Enter Schedule ID to update: "; cin >> scheduleId;

    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    // (load the schedule first)
    cout << "Enter new shift date: ";
    string newDate;
    cin >> newDate;

    cout << "Enter new start time: ";
    string newStart;
    cin >> newStart;

    cout << "Enter new end time: ";
    string newEnd;
    cin >> newEnd;

    cout << "Enter new location: ";
    string newLocation;
    cin >> newLocation;

    cout << "Enter new note: ";
    cin.ignore();
    string newNote;
    getline(cin, newNote);

    // update
    const char* update = "UPDATE schedules SET shift_date = ?, start_time = ?, end_time = ?, location = ?, note = ? WHERE schedule_id = ?";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, update, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, newDate.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, newStart.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, newEnd.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, newLocation.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, newNote.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 6, scheduleId);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Schedule updated successfully!" << endl;
        }
        else {
            cout << "Failed to update schedule!" << endl;
        }
        sqlite3_finalize(stmt);
    }
    else {
        cout << "Failed to prepare update!" << endl;
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
