#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
#include "auth.h"
using namespace std;

bool login(int empID, const string& password) {
    sqlite3* db = connectToDatabase();
    if (db == nullptr) {
        cerr << "Could not open database.\n";
        return false;
    }

    sqlite3_stmt* stmt;
    bool success = false;

    string sql = "SELECT Employee_Name, Employee_Possition, branch FROM employee WHERE Employee_ID = ? AND password_hash = ?";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, empID);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            currentUser.id = empID;
            currentUser.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            currentUser.position = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            currentUser.branch = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

            cout << string(120, '-') << endl;
            cout<< right << setw(70) << "Log In Successful" << endl;
			cout << right << setw(50) << "Welcome, " << currentUser.name << " (" << currentUser.position << " @" << currentUser.branch << ")" << endl;
            cout << string(120, '-') << endl;
            cout << endl;
            cout << endl;
            
            success = true;
        }
        else {
            cout << string(120, '-') << endl;
            cout << right << setw(70) << "Login failed. \n" << endl;
            cout << right << setw(73)<< "Invalid ID or password.\n";
            cout << string(120, '-') << endl;
            cout << endl;
        }
        sqlite3_finalize(stmt);
    }
    else {
        cout << endl;
    }

    sqlite3_close(db);

    if (success) {
        mainMenu(); // Proceed to main menu
    }

    return success;
}
