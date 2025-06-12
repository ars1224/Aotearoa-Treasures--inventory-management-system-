#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
#include "auth.h"
using namespace std;

void deleteEmployee() {
    if (!isAdmin() && !isStoreManager()) {
        cout << "\u274c You do not have permission to delete employees.\n";
        return;
    }

    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    char choice;
    do {
        string empID;
        cout << "Enter Employee ID to delete: ";
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
            cout << "Employee with ID " << empID << " deleted successfully!";
        }

        sqlite3_finalize(stmt);

        while (true) {
            cout << " Do you want to delete another employee ? (Y / N) : ";
                cin >> choice;

            cin.ignore(numeric_limits<streamsize>::max());

                if (choice == 'Y' || choice == 'y' || choice == 'N' || choice == 'n') {
                    break;
                }
                else {
                    cout << " Invalid input. Please enter 'Y' or 'N' only.";
                }
        }
    } while (choice == 'Y' || choice == 'y');

    closeDatabase(db);
}
