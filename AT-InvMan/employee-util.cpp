// Jhon Aries Tayao

#include <iostream>
#include <sqlite3.h>
#include "db-conn.h"
#include "employee.h"
using namespace std;

void logEmployeeAction(sqlite3* db, const string& empID, const string& action, const string& details) {
    const char* sql = "INSERT INTO employee_log (Employee_ID, Action, Details) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, empID.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, action.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, details.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            cerr << " Failed to log action: " << sqlite3_errmsg(db) << endl;
        }
        sqlite3_finalize(stmt);
    }
    else {
        cerr << "Failed to prepare log statement: " << sqlite3_errmsg(db) << endl;
    }
}

void viewEmployeeLogs() {
    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    const char* sql = "SELECT * FROM employee_log ORDER BY Timestamp DESC;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare log query: " << sqlite3_errmsg(db) << endl;
        closeDatabase(db);
        return;
    }

    cout << "\n--- Employee Activity Logs ---\n";
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int logID = sqlite3_column_int(stmt, 0);
        const char* empID = (const char*)sqlite3_column_text(stmt, 1);
        const char* action = (const char*)sqlite3_column_text(stmt, 2);
        const char* details = (const char*)sqlite3_column_text(stmt, 3);
        const char* timestamp = (const char*)sqlite3_column_text(stmt, 4);

        cout  << "    " " timestamp " << "    " << "    " << "    " << "Employee ID: " << "    " << "    " << "    " << "  Action: " << "    " << "    " << "    " << "  Details: " << "    " << "    " << "    " << endl;
        cout << endl;
        cout << "----------------------------------------------------------------------------------------------" << endl;
        cout << timestamp << "    " << "    " << "    " << empID << "    " << "    " << "    " << action << "    " << "    " << "    " << (details ? details : "N/A");
        cout << endl;
        cout << "----------------------------------------------------------------------------------------------" << endl;
        
        int option;

        cout << endl;
        cout << "Related options on Employee" << endl;
        cout << endl;
        cout << "1. View Employee List" << endl;
        cout << "2. Add Employee" << endl;
        cout << "3. Update Employee" << endl;
        cout << "4. Delete Employee" << endl;
        cout << "5 See Roster of Employees" << endl;
        cout << "6. Back to Main Menu" << endl;
        cout << "7. Exit Program" << endl;
        cout << endl;
        cout << "Please select an option: ";


        cin >> option;
        switch (option) {
        case 1:
            cout << endl;
            employeeList(); // Function to add an employee
            break;
        case 2:
            cout << endl;
            addEmployee(); // Function to add an employee
            break;
        case 3:
            updateEmployee(); // Function to update an employee
            break;
        case 4:
            deleteEmployee(); // Function to delete an employee
            break;
        case 5:
            //roster(); // Recursive call to see the list again
            break;
        case 6:
            //MainMenu();
            break;
        case 7:
            return; // Back to main menu
        default:

            cout << "Invalid option. Please try again." << endl;
        }

        sqlite3_finalize(stmt);
        closeDatabase(db);
    }
}