// add-roster.cpp

#include <iostream>
#include <sqlite3.h>
#include "db-conn.h"
#include "employee.h"
using namespace std;

void addRoster() {
    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    rosterEntry entry;
    cout << "\n Add New Schedule\n";
    cout << "Employee ID: ";
    cin >> entry.employeeID;
    cout << "Shift Date (YYYY-MM-DD): ";
    cin >> entry.shiftDate;
    cout << "Start Time (HH:MM): ";
    cin >> entry.startTime;
    cout << "End Time (HH:MM): ";
    cin >> entry.endTime;
    cout << "Location: ";
    cin.ignore(); // clear newline from previous cin
    getline(cin, entry.location);
    cout << "Note (optional): ";
    getline(cin, entry.note);

    const char* sql = R"(
        INSERT INTO schedules (Employee_ID, shift_date, start_time, end_time, location, note)
        VALUES (?, ?, ?, ?, ?, ?);
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        closeDatabase(db);
        return;
    }

    sqlite3_bind_int(stmt, 1, entry.employeeID);
    sqlite3_bind_text(stmt, 2, entry.shiftDate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, entry.startTime.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, entry.endTime.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, entry.location.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, entry.note.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        cout << "\nSchedule added successfully.\n";
    }
    else {
        cerr << "\nFailed to add schedule: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    closeDatabase(db);
}

void roster() {
    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    const char* sql = "SELECT * FROM schedules;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        cout << "\nRoster Schedule:\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            int empID = sqlite3_column_int(stmt, 1);
            const unsigned char* date = sqlite3_column_text(stmt, 2);
            const unsigned char* start = sqlite3_column_text(stmt, 3);
            const unsigned char* end = sqlite3_column_text(stmt, 4);
            const unsigned char* location = sqlite3_column_text(stmt, 5);
            const unsigned char* note = sqlite3_column_text(stmt, 6);

            cout << "      " << "ID " <<"      " << "  Employee # " << "      " << "  Time " << "      " << "  " << "      " << " Location " << "      " << "  Note " << endl;
			cout << "---------------------------------------------------------------------------------------------------" << endl;
            cout << endl;
            cout << " " << "      " << id << "      " << "      " << empID << "      " << "      " << start << "   -   " << "- " << end << "      " << "      " << location << "      " << "      " << note << "      " << "      " << endl;
            cout << "---------------------------------------------------------------------------------------------------" << endl;
            cout << endl;
        }
    }
    else {
        cerr << "Failed to view roster: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    closeDatabase(db);

    int option;

    cout << endl;
    cout << "Related options on Employee" << endl;
    cout << endl;
    cout << "1. Make a new schedule " << endl;
    cout << "2. Update schedule " << endl;
    cout << "3. Add Employee" << endl;
    cout << "4. Update Employee" << endl;
    cout << "5.  Delete Employee" << endl;
    cout << "6. Back to Main Menu" << endl;
    cout << "7. Exit Program" << endl;
    cout << endl;
    cout << "Please select an option: ";


    cin >> option;
    switch (option) {
    case 1:
        cout << endl;
        employeeMasterList();
        addRoster(); // Function to add an employee
        
        break;
    case 2:
        updateRoster(); // Function to update an employee
        break;
    case 3:
        addEmployee(); // Function to add an employee
        break;
    case 4:
        updateEmployee(); // Recursive call to see the list again
        break;
    case 5:
        deleteEmployee();
        break;
    case 6:
        //mainMenu();
        break;
    case 7:
        return; // Back to main menu
    default:

        cout << "Invalid option. Please try again." << endl;
    }
}

void updateRoster() {
    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    int scheduleID;
    cout << "\nUpdate Schedule\n";
    cout << "Enter Schedule ID to update: ";
    cin >> scheduleID;
    cin.ignore(); // flush newline

    // Retrieve existing schedule
    const char* selectSQL = "SELECT * FROM schedules WHERE schedule_id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare SELECT: " << sqlite3_errmsg(db) << endl;
        closeDatabase(db);
        return;
    }

    sqlite3_bind_int(stmt, 1, scheduleID);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        cout << "Schedule ID not found.\n";
        sqlite3_finalize(stmt);
        closeDatabase(db);
        return;
    }

    // Get current values
    string date = (const char*)sqlite3_column_text(stmt, 2);
    string start = (const char*)sqlite3_column_text(stmt, 3);
    string end = (const char*)sqlite3_column_text(stmt, 4);
    string location = (const char*)sqlite3_column_text(stmt, 5);
    string note = (const char*)sqlite3_column_text(stmt, 6);
    sqlite3_finalize(stmt);

    int choice;
    string input;

    do {
        cout << "\nCurrent Schedule (ID: " << scheduleID << ")\n";
        cout << "1. Date      : " << date << "\n";
        cout << "2. Start Time: " << start << "\n";
        cout << "3. End Time  : " << end << "\n";
        cout << "4. Location  : " << location << "\n";
        cout << "5. Note      : " << note << "\n";
        cout << "6. Save and Exit\n";
        cout << "Select a field to update (1-6): ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            cout << "New Date (YYYY-MM-DD): ";
            getline(cin, date);
            break;
        case 2:
            cout << "New Start Time (HH:MM): ";
            getline(cin, start);
            break;
        case 3:
            cout << "New End Time (HH:MM): ";
            getline(cin, end);
            break;
        case 4:
            cout << "New Location: ";
            getline(cin, location);
            break;
        case 5:
            cout << "New Note: ";
            getline(cin, note);
            break;
        case 6:
            break;
        default:
            cout << "Invalid option.\n";
        }

    } while (choice != 6);

    // Update database
    const char* updateSQL = R"(
        UPDATE schedules
        SET shift_date = ?, start_time = ?, end_time = ?, location = ?, note = ?
        WHERE schedule_id = ?;
    )";

    sqlite3_stmt* updateStmt;
    if (sqlite3_prepare_v2(db, updateSQL, -1, &updateStmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare UPDATE: " << sqlite3_errmsg(db) << endl;
        closeDatabase(db);
        return;
    }

    sqlite3_bind_text(updateStmt, 1, date.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(updateStmt, 2, start.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(updateStmt, 3, end.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(updateStmt, 4, location.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(updateStmt, 5, note.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(updateStmt, 6, scheduleID);

    if (sqlite3_step(updateStmt) == SQLITE_DONE) {
        cout << "\n? Schedule updated successfully.\n";
    }
    else {
        cerr << "\n? Failed to update schedule: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(updateStmt);
    closeDatabase(db);
}
