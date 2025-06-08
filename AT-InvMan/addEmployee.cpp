//Jhon Aries

#include <iostream>
#include <sqlite3.h>
#include "db-conn.h"
#include "employee.h"
using namespace std;

void addEmployee() {
    sqlite3* db = connectToDatabase();
    if (db == nullptr) {
        return;
    }

    const char* sql = "INSERT INTO employee (Employee_ID, Employee_Name, Employee_Possition, Employee_Status, password_hash, branch) VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        closeDatabase(db);
        return;
    }

    // Input
    employee emp;
    cout << "Enter employee details:" << endl;
    cout << "ID: ";
    cin >> emp.id;
    cin.ignore(); // Clear newline left by cin

    cout << "Name: ";
    getline(cin, emp.name);

    cout << "Position: ";
    getline(cin, emp.position);

    cout << "Status: ";
    getline(cin, emp.status);

    cout << "Password: ";
    getline(cin, emp.password);

    cout << "Store Branch: ";
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
        cout << "Employee added successfully!" << endl;
        cout << endl;
        cout << endl;
    }

    cout << "Do you want to add another employee? (y/n): ";
    char choice;
    cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        addEmployee(); // Recursive call to add another employee
    }
    else {
        cout << "Exiting employee addition." << endl;
        cout << endl;
        cout << endl;
        int option;

        cout << "Related options on Employee" << endl;
        cout << endl;
        cout << "1. Update Employee" << endl;
        cout << "2. Delete Employee" << endl;
        cout << "3. See Roster of Employees" << endl;
        cout << "4. Back to Main Menu" << endl;
        cout << endl;
        cout << "Please select an option: ";


        cin >> option;
        switch (option) {
        case 1:
            cout << endl;
            updateEmployee(); // Function to add an employee
            break;
        case 2:
            cout << endl;
            deleteEmployee(); // Function to update an employee
            break;
        case 3:
            cout << endl;
            //roster();; // Function to delete an employee
            break;
        case 4:
            return; // Back to main menu
        default:

            cout << "Invalid option. Please try again." << endl;
        }


        // Cleanup
        sqlite3_finalize(stmt);
        closeDatabase(db);
    }
}
