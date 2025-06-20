#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
#include "auth.h"
#include <regex>
using namespace std;

void deleteRoster()
{
    int employeeId;

    cout << "Enter Employee ID to delete roster: ";
    cin >> employeeId;

    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    // Check if the roster exists first
    const char* select = "SELECT Employee_ID FROM schedules WHERE Employee_ID = ?";
    sqlite3_stmt* selectStmt;

    if (sqlite3_prepare_v2(db, select, -1, &selectStmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(selectStmt, 1, employeeId);

        if (sqlite3_step(selectStmt) == SQLITE_ROW) {
            // Roster exists, proceed with delete
            sqlite3_finalize(selectStmt);

            const char* deleteSQL = "DELETE FROM schedules WHERE Employee_ID = ?";
            sqlite3_stmt* deleteStmt;

            if (sqlite3_prepare_v2(db, deleteSQL, -1, &deleteStmt, NULL) == SQLITE_OK) {
                sqlite3_bind_int(deleteStmt, 1, employeeId);

                if (sqlite3_step(deleteStmt) == SQLITE_DONE) {
                    cout << "✅ Schedule deleted successfully!" << endl;
                    rosterlist();
                }
                else {
                    cout << "❌ Failed to delete schedule!" << endl;
                    rosterlist();
                }
                sqlite3_finalize(deleteStmt);
            }
            else {
                cout << "❌ Failed to prepare delete!" << endl;
            }
        }
        else {
            cout << "❌ Schedule not found!" << endl;
            sqlite3_finalize(selectStmt);

        }
    }
    else {
        cout << "❌ Failed to prepare select!" << endl;
    }

    closeDatabase(db);
    RosterMenu();
}
