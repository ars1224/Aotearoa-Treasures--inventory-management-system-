//Jhon Aries

#include <iostream>
#include <sqlite3.h>
#include "db-conn.h"
#include "employee.h"
using namespace std;

void updateEmployee() {
    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    string empID;
    int choice;
    string newValue;

    cout << endl;
    cout << "Enter Employee ID to update: ";
    cin >> empID;
    cin.ignore();

    cout << "\nSelect field to update:\n";
    cout << "1. Name\n";
    cout << "2. Position\n";
    cout << "3. Status\n";
    cout << "4. Password\n";
    cout << endl;
    cout << "Enter your choice ";
    cin >> choice;
    cin.ignore();

    cout << endl;

    string column;
    switch (choice) {
    case 1:
        column = "Employee_Name";
        break;
    case 2:
        column = "Employee_Possition";
        break;
    case 3:
        column = "Employee_Status";
        break;
    case 4:
        column = "password_hash";
        break;
    default:
        cout << "Invalid choice." << endl;
        closeDatabase(db);
        return;
    }

    cout << "Enter new value for " << column << ": ";
    getline(cin, newValue);

    cout << endl;

    string sql = "UPDATE employee SET " + column + " = ? WHERE Employee_ID = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        closeDatabase(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, newValue.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, empID.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "Update failed: " << sqlite3_errmsg(db) << endl;
    }
    else {
        cout << "Employee field updated successfully!" << endl;
		employeeList(); // Refresh employee list
    }

    sqlite3_finalize(stmt);
    closeDatabase(db);
}