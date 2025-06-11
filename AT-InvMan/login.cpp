// Jhon Aries
#include <iostream>
#include <sqlite3.h>
#include <string>
#include "db-conn.h"
#include "mainmenu.h"

using namespace std;

bool login(int empID, const string& password) {
    sqlite3* db = connectToDatabase();  // ✅ Only call once
    if (db == nullptr) {
        cerr << "❌ Could not open database.\n";
        return false;
    }

    sqlite3_stmt* stmt;
    bool success = false;

    string sql = "SELECT Employee_Name FROM employee WHERE Employee_ID = ? AND password_hash = ?";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, empID);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            cout << "\n=======================================================================\n";
            cout << "                           Log In Succesful\n";
            cout << "=======================================================================\n";
            cout << "\n=======================================================================\n";
            cout << "                           Welcome, " << name << "!\n";
            cout << "=======================================================================\n";
            cout << endl;
            cout << endl;
            mainMenu();
            success = true;
        }
        else {
            cout << "\n=======================================================================\n";
            cout << "                          Login failed. \n";
            cout << "=======================================================================";
            cout << "\n=======================================================================\n";
            cout << "                      Invalid ID or password.!\n";
            cout << "=======================================================================";
            cout << endl;       
            cout << endl;
        }
        sqlite3_finalize(stmt);
    }
    else {
        cerr << " Failed to prepare login query.\n";
    }

    sqlite3_close(db);
    return success;
}
    