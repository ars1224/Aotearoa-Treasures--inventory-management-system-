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

void roster() {
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

    int option;
    cout << endl;
    cout << "Related options on Employee\n";
    cout << endl;
    cout << "1. Make a new schedule\n";
    cout << "2. Delete Schedule\n";
    cout << "3. Update schedule\n";
    cout << "4. Add Employee\n";
    cout << "5. Update Employee\n";
    cout << "6. Delete Employee\n";
    cout << "7. Back to Main Menu\n";
    cout << "8. Exit Program\n";
    cout << endl;
    cout << "Please select an option: ";
    cin >> option;

    switch (option) {
    case 1:
        cout << endl;
        employeeMasterlist();
        addRoster();
        break;
    case 2:
        cout << endl;
        deleteRoster();
        break;
    case 3:
        cout << endl;
        employeeMasterlist();
		updateRoster();
        break;
    case 4:
        cout << endl;
        addEmployee();
        break;
    case 5:
        cout << endl;
        updateEmployee();
        break;
    case 6:
        cout << endl;
        deleteEmployee();
        break;
    case 7:
        mainMenu();
        break;
    case 8:
		exit(0);
    default:
        cout << "Invalid option. Please try again." << endl;
    }
}







