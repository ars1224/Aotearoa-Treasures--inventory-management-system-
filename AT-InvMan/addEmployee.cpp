#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
#include "auth.h"
using namespace std;

// Generates a unique 5-digit Employee_ID
string generateEmployeeId(sqlite3* db) {
    int id;
    bool exists = true;

    do {
        id = rand() % 90000 + 10000; // Generates number between 10000 and 99999

        // Check if it already exists in the database
        sqlite3_stmt* stmt;
        string check = "SELECT Employee_ID FROM employee WHERE Employee_ID = ?";
        if (sqlite3_prepare_v2(db, check.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, to_string(id).c_str(), -1, SQLITE_TRANSIENT);

            int res = sqlite3_step(stmt);
            if (res == SQLITE_ROW) {
                // Already exists
                exists = true;
            }
            else {
                // Not found
                exists = false;
            }
            sqlite3_finalize(stmt);
        }
        else {
            cout << "Failed to prepare for ID check!" << endl;
            exists = true;
        }
    } while (exists);

    return to_string(id);
}

void addEmployee()
{
    if (!isAdmin() && !isStoreManager()) {
        cout << "\u274c You do not have permission to add employees.\n";
        return;
    }

    char choice;
    do {
        sqlite3* db = connectToDatabase();
        if (db == nullptr) return;

        // Prepare insert
        const char* sql = "INSERT INTO employee (Employee_ID, Employee_Name, Employee_Possition, Employee_Status, password_hash, branch) VALUES (?, ?, ?, ?, ?, ?);";

        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            closeDatabase(db);
            return;
        }

        employee emp;

        cout << "\nEnter Employee Details:\n";
        cout << "--------------------------------\n";

        // Branch Selection
        if (isStoreManager()) {
            emp.branch = currentUser.branch;
            cout << "Branch (auto-filled): " << emp.branch << endl;
        }
        else if (isAdmin()) {
            int branchChoice = 0;
            while (true) {
                cout << "Select Branch:\n";
                cout << "1. Christchurch\n";
                cout << "2. Auckland\n";
                cout << "3. Wellington\n";
                cout << "Enter choice (1-3): "; cin >> branchChoice; cin.ignore();

                if (branchChoice == 1) {
                    emp.branch = "Christchurch"; break;
                }
                else if (branchChoice == 2) {
                    emp.branch = "Auckland"; break;
                }
                else if (branchChoice == 3) {
                    emp.branch = "Wellington"; break;
                }
                else {
                    cout << "\u274c Invalid branch selection. Please try again.\n\n";
                }
            }
            cout << "Branch selected: " << emp.branch << endl;
        }

        // Auto-generating Employee_ID
        emp.id = generateEmployeeId(db);
        cout << "Generated Employee_ID: " << emp.id << endl;

        // Name
        cout << "Name: ";
        getline(cin, emp.name);

        // Position
        int posChoice = 0;
        while (true) {
            cout << "Select Position:\n";
            cout << "1. Sales Associate\n";
            cout << "2. Cashier\n";
            cout << "3. Stock Clerk\n";
            if (isAdmin()) cout << "4. Store Manager\n";

            cout << "Enter choice (1-" << (isAdmin() ? "4" : "3") << "): ";
            cin >> posChoice;
            cin.ignore();

            if (posChoice == 1) { emp.position = "Sales Associate"; break; }
            else if (posChoice == 2) { emp.position = "Cashier"; break; }
            else if (posChoice == 3) { emp.position = "Stock Clerk"; break; }
            else if (posChoice == 4 && isAdmin()) { emp.position = "Store Manager"; break; }
            else { cout << "Invalid position selection. Please try again.\n\n"; }
        }

        cout << "Position selected: " << emp.position << endl;

        // Password
        cout << "Password: ";
        getline(cin, emp.password);
        if (emp.password.empty()) {
            cout << "\u26A0 No password entered. Using default 'changeme'.\n";
            emp.password = "changeme";
        }

        // Default status
        emp.status = "Active";

        // Bind parameters
        sqlite3_bind_text(stmt, 1, emp.id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, emp.name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, emp.position.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, emp.status.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, emp.password.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, emp.branch.c_str(), -1, SQLITE_TRANSIENT);

        // Execute
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "\n\u2705 Employee added successfully!\n";
        }
        else {
            cout << "Insert failed: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);
        closeDatabase(db);

        cout << "\nDo you want to add another employee? (y/n): ";
        cin >> choice;
        cin.ignore();

    } while (choice == 'y' || choice == 'Y');

    cout << endl;
    employeeList();
}

