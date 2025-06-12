#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
#include "auth.h" // updated to use correct RBAC header
#include "session.h"     // for access to currentUser
using namespace std;

void employeeList() {
    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    sqlite3_stmt* stmt;
    string sql;

    if (isAdmin()) {
        sql = "SELECT * FROM employee;";
    }
    else if (isStoreManager()) {
        sql = "SELECT * FROM employee WHERE branch = ?;";
    }
    else {
        sql = "SELECT * FROM employee WHERE Employee_ID = ?;";
    }

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        closeDatabase(db);
        return;
    }

    if (isStoreManager()) {
        sqlite3_bind_text(stmt, 1, currentUser.branch.c_str(), -1, SQLITE_STATIC);
    }
    else if (!isAdmin()) {
        sqlite3_bind_int(stmt, 1, currentUser.id);
    }

    cout << "\n=======================================================================\n";
    cout << "                            Employee List:\n";
    cout << "=======================================================================\n";
    cout << "\n    ID      Name        Position    Status     Branch\n\n";

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* name = sqlite3_column_text(stmt, 1);
        const unsigned char* pos = sqlite3_column_text(stmt, 2);
        const unsigned char* status = sqlite3_column_text(stmt, 3);
        const unsigned char* branch = sqlite3_column_text(stmt, 5);

        cout << "    " << id << "    " << name << "    " << pos << "    " << status << "    " << branch << endl;
        cout << "_________________________________________________________________________________" << endl << endl;
    }

    sqlite3_finalize(stmt);
    closeDatabase(db);

    // Menu options
    if (isAdmin() || isStoreManager()) {
        int option;
        cout << "\nRelated options on Employee:\n";
        cout << "1. Add Employee\n";
        cout << "2. Update Employee\n";
        cout << "3. Delete Employee\n";
        cout << "4. View Roster of Employees\n";
        cout << "5. Back to Main Menu\n";
        cout << "6. Exit Program\n\n";
        cout << "Please select an option: ";
        cin >> option;

        switch (option) {
        case 1:
            addEmployee(); break;
        case 2:
            updateEmployee(); break;
        case 3:
            deleteEmployee(); break;
        case 4:
            roster(); break;
        case 5:
            mainMenu(); break;
        case 6:
            exit(0); break;
        default:
            cout << "Invalid option. Please try again.\n";
        }
    }
    else {
        cout << "\nYou only have permission to view your own information. Returning to main menu.\n";
        mainMenu();
    }
}
