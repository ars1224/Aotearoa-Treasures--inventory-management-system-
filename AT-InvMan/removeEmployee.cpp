#include <iostream>
#include <limits>
#include <sqlite3.h>
#include "db-conn.h"
#include "employee.h"
using namespace std;

void deleteEmployee() {
    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    char choice;

    do {
        string empID;
        cout << "\nEnter Employee ID to delete: ";
        cin >> empID;

        const char* sql = "DELETE FROM employee WHERE Employee_ID = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare DELETE statement: " << sqlite3_errmsg(db) << endl;
            break;
        }

        sqlite3_bind_text(stmt, 1, empID.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            cerr << "Failed to delete employee: " << sqlite3_errmsg(db) << endl;
        }
        else {
            cout << "Employee with ID " << empID << " deleted successfully!\n";
        }

        sqlite3_finalize(stmt);

        // Input validation loop
        while (true) {
            cout << "\nDo you want to delete another employee? (Y/N): ";
            cin >> choice;

            // Clear input buffer
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (choice == 'Y' || choice == 'y' || choice == 'N' || choice == 'n') {
                break; // valid input
            }
            else {
                cout << " Invalid input. Please enter 'Y' or 'N' only.\n";
            }
        }

    } while (choice == 'Y' || choice == 'y');

    closeDatabase(db);

    // Show employee list at the end
    employeeList();
}
