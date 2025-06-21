#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
#include "auth.h" // updated to use correct RBAC header
#include "session.h"     // for access to currentUser
using namespace std;

void employeeMasterlist() {
    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    sqlite3_stmt* stmt;
    string sql;

    if (isAdmin()) {
        sql = "SELECT * FROM employee;";
    }
    else if (isStoreManager()) {
        sql = "SELECT * FROM employee WHERE branch = ?;";
    }

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        closeDatabase(db);
        return;
    }

    if (isStoreManager()) {
        sqlite3_bind_text(stmt, 1, currentUser.branch.c_str(), -1, SQLITE_STATIC);
    }
    else if (!isAdmin()) {
        sqlite3_bind_int(stmt, 1, currentUser.id);
    }

    cout << string(140, '-') << endl;
    cout << right << setw(60) << " Employee List:\n";
    cout << string(140, '-') << endl;
    cout << right << setw(5) << "ID" << right << setw(20) << "Name" << right << setw(30) << "Position" << right << setw(30) << "Status" << right << setw(30) << "Branch" << endl;
    cout << string(140, '-') << endl;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* name = sqlite3_column_text(stmt, 1);
        const unsigned char* pos = sqlite3_column_text(stmt, 2);
        const unsigned char* status = sqlite3_column_text(stmt, 3);
        const unsigned char* branch = sqlite3_column_text(stmt, 5);

        cout << right << setw(5) << id << right << setw(20) << name << right << setw(30) << pos << right << setw(30) << status << right << setw(30) << branch << endl;
    }
    cout << string(140, '-') << endl;

    sqlite3_finalize(stmt);
    closeDatabase(db);
}