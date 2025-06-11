// Jhon Aries Tayao

#include <iostream>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
using namespace std;

void employeeList() {
    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    const char* sql = "SELECT * FROM employee WHERE Employee_Possition != 'Owner'";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        closeDatabase(db);
        return;
    }

    // Print results
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
    closeDatabase(db); // ✅ Close BEFORE branching elsewhere

    // Menu options
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
