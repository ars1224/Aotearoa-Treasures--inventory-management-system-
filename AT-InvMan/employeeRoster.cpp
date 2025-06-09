// add-roster.cpp

#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "employee.h"
using namespace std;

void addRoster() {
    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    string repeat;
    do {
        rosterEntry entry;
        string empName, empBranch, empPosition;

        cout << "\nAdd New Schedule\n";
        cout << "Employee ID: ";
        cin >> entry.employeeID;

        // Look up employee name, branch, and position
        const char* lookupSQL = "SELECT Employee_Name, branch, Employee_Possition FROM employee WHERE Employee_ID = ?;";
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

        cout << "Shift Date (YYYY-MM-DD): ";
        cin >> entry.shiftDate;
        cout << "Start Time (HH:MM): ";
        cin >> entry.startTime;
        cout << "End Time (HH:MM): ";
        cin >> entry.endTime;
        cin.ignore();
        cout << "Note (optional, press Enter to skip): ";
        getline(cin, entry.note);

        const char* sql = R"(
            INSERT INTO schedules (
                Employee_ID, Employee_Name, Employee_Possition, shift_date, start_time, end_time, location, note
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?);
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

        if (entry.note.empty()) {
            sqlite3_bind_null(stmt, 8);
        }
        else {
            sqlite3_bind_text(stmt, 8, entry.note.c_str(), -1, SQLITE_STATIC);
        }

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "\n✅ Schedule added successfully.\n";
        }
        else {
            cerr << "❌ Failed to add schedule: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);

        cout << "\nDo you want to make another schedule? (y/n): ";
        cin >> repeat;
        cin.ignore(); // remove leftover newline

    } while (repeat == "y" || repeat == "Y");

    closeDatabase(db);

    int option;
    cout << "\nRelated options on Employee\n";
    cout << "1. View Roster\n";
    cout << "2. Update schedule\n";
    cout << "3. Add Employee\n";
    cout << "4. Update Employee\n";
    cout << "5. Delete Employee\n";
    cout << "6. Back to Main Menu\n";
    cout << "7. Exit Program\n";
    cout << "Please select an option: ";
    cin >> option;

    switch (option) {
    case 1:
        roster();
        break;
    case 2:
        updateRoster();
        break;
    case 3:
        addEmployee();
        break;
    case 4:
        updateEmployee();
        break;
    case 5:
        deleteEmployee();
        break;
    case 6:
        // mainMenu();
        break;
    case 7:
        return;
    default:
        cout << "Invalid option. Please try again." << endl;
    }
}






void roster() {
    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    const char* sql = R"(
        SELECT * FROM schedules
        WHERE DATE(shift_date) BETWEEN DATE('now') AND DATE('now', '+28 days')
        ORDER BY shift_date ASC;
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        cout << "\nRoster Schedule (Next 4 Weeks):\n";
        cout << left << setw(5) << "ID"
            << setw(12) << "Emp #"
            << setw(20) << "Name"
            << setw(15) << "Position"
            << setw(12) << "Date"
            << setw(10) << "Start"
            << setw(10) << "End"
            << setw(15) << "Location"
            << "Note" << endl;
        cout << string(115, '-') << endl;

        bool hasData = false;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            hasData = true;
            int id = sqlite3_column_int(stmt, 0);
            int empID = sqlite3_column_int(stmt, 1);
            const unsigned char* name = sqlite3_column_text(stmt, 2);
            const unsigned char* poss = sqlite3_column_text(stmt, 3);
            const unsigned char* date = sqlite3_column_text(stmt, 4);
            const unsigned char* start = sqlite3_column_text(stmt, 5);
            const unsigned char* end = sqlite3_column_text(stmt, 6);
            const unsigned char* location = sqlite3_column_text(stmt, 7);
            const unsigned char* note = sqlite3_column_text(stmt, 8);
            if (!note) note = (const unsigned char*)"";

            printf("%-5d %-12d %-20s %-15s %-12s %-10s %-10s %-15s %s\n",
                id, empID, name, poss, date, start, end, location, note);
        }

        if (!hasData) {
            cout << "No schedules found for the next 4 weeks.\n";
        }
    }
    else {
        cerr << "Failed to view roster: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    closeDatabase(db);

    int option;
    cout << "\nRelated options on Employee\n";
    cout << "1. Make a new schedule\n";
    cout << "2. Update schedule\n";
    cout << "3. Add Employee\n";
    cout << "4. Update Employee\n";
    cout << "5. Delete Employee\n";
    cout << "6. Back to Main Menu\n";
    cout << "7. Exit Program\n";
    cout << "Please select an option: ";
    cin >> option;

    switch (option) {
    case 1:
        cout << endl;
        employeeMasterList();
        addRoster();
        break;
    case 2:
        updateRoster();
        break;
    case 3:
        addEmployee();
        break;
    case 4:
        updateEmployee();
        break;
    case 5:
        deleteEmployee();
        break;
    case 6:
        // mainMenu();
        break;
    case 7:
        return;
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
    string date = (const char*)sqlite3_column_text(stmt, 4);
    string start = (const char*)sqlite3_column_text(stmt, 5);
    string end= (const char*)sqlite3_column_text(stmt, 6);
    string location = (const char*)sqlite3_column_text(stmt, 7);
    string note = (const char*)sqlite3_column_text(stmt, 8);
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

    int option;
    cout << "\nRelated options on Employee\n";
    cout << "1. Make a new schedule\n";
    cout << "2. Add Employee\n";
    cout << "3. Update Employee\n";
    cout << "4. Delete Employee\n";
    cout << "5. Back to Main Menu\n";
    cout << "6. Exit Program\n";
    cout << "Please select an option: ";
    cin >> option;

    switch (option) {
    case 1:
        cout << endl;
        employeeMasterList();
        addRoster();
        break;
    case 2:
        addEmployee();
        break;
    case 3:
        updateEmployee();
        break;
    case 4:
        deleteEmployee();
        break;
    case 5:
        // mainMenu();
        break;
    case 6:
        return;
    default:
        cout << "Invalid option. Please try again." << endl;
    }
}
