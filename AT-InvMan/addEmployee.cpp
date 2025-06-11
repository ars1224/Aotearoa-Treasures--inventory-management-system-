// Jhon Aries

#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
using namespace std;

void addEmployee() {
    char choice;

    do {
        sqlite3* db = connectToDatabase();
        if (db == nullptr) return;

        const char* sql = "INSERT INTO employee (Employee_ID, Employee_Name, Employee_Possition, Employee_Status, password_hash, branch) VALUES (?, ?, ?, ?, ?, ?);";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            closeDatabase(db);
            return;
        }

        // Input
        employee emp;
        cout << "\nEnter Employee Details:\n";
        cout << "ID: ";
        cin >> emp.id;
        cin.ignore();

        cout << "Name: ";
        getline(cin, emp.name);

        cout << "Position: ";
        getline(cin, emp.position);

        cout << "Status: ";
        getline(cin, emp.status);

        cout << "Password: ";
        getline(cin, emp.password);

        cout << "Branch: ";
        getline(cin, emp.branch);

        // Bind values
        sqlite3_bind_text(stmt, 1, emp.id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, emp.name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, emp.position.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, emp.status.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, emp.password.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, emp.branch.c_str(), -1, SQLITE_TRANSIENT);

        // Execute
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            cerr << "Insert failed: " << sqlite3_errmsg(db) << endl;
        }
        else {
            cout << "\n✅ Employee added successfully!\n";
        }

        sqlite3_finalize(stmt);
        closeDatabase(db);

        cout << "\nDo you want to add another employee? (y/n): ";
        cin >> choice;
        cin.ignore();

    } while (choice == 'y' || choice == 'Y');

    // Post-input menu
    int option;
    cout << "\n========== Related Employee Options ==========\n";
    cout << "1. Update Employee\n";
    cout << "2. Delete Employee\n";
    cout << "3. See Roster of Employees\n";
    cout << "4. Back to Main Menu\n";
    cout << "5. Exit Program\n";
    cout << "=============================================\n";
    cout << "Please select an option: ";
    cin >> option;

    switch (option) {
    case 1:
        updateEmployee();
        break;
    case 2:
        deleteEmployee();
        break;
    case 3:
        roster();
        break;
    case 4:
        mainMenu();
        break;
    case 5:
        exit(0);
    default:
        cout << "Invalid option. Please try again.\n";
        break;
    }
}