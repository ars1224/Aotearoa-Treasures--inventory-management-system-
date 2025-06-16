#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
#include "auth.h"
using namespace std;

void rosterlist(){
    sqlite3* db = connectToDatabase();
    if (db == NULL) return;

    // We omit schedules.schedule_id in the select
    const char* sql = R"(
        SELECT schedules.Employee_ID, schedules.Employee_Name, 
               schedules.Employee_Possition, schedules.branch, schedules.Monday, schedules.Tuesday, 
               schedules.Wednesday, schedules.Thursday, schedules.Friday,
               schedules.Saturday, schedules.Sunday
        FROM schedules;
    )";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        cout << string(150, '=') << endl;
        cout << right << setw(85) << "Employee Roster Schedule" << endl;
        cout << string(150, '=') << endl;
        cout << endl;
        cout << setw(90) << "Opening Shift: 7am to 3:30pm " << endl;
        cout << setw(90) << "Closing Shift: 10:30am to 7pm " << endl;
        cout << endl;
        cout << string(150, '=') << endl;

        cout << setw(5) << "EmpID"
            << setw(10) << "Name"
            << setw(20) << "Position"
            << setw(20) << "Branch"
            << setw(13) << "Monday"
            << setw(13) << "Tuesday"
            << setw(13) << "Wednesday"
            << setw(13) << "Thursday"
            << setw(13) << "Friday"
            << setw(13) << "Saturday"
            << setw(13) << "Sunday"
            << endl;

        cout << string(150, '=') << endl;

        bool hasData = false;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            hasData = true;

            int empId = sqlite3_column_int(stmt, 0);
            const unsigned char* name = sqlite3_column_text(stmt, 1);
            const unsigned char* position = sqlite3_column_text(stmt, 2);
            const unsigned char* branch = sqlite3_column_text(stmt, 3);
            const unsigned char* monday = sqlite3_column_text(stmt, 4);
            const unsigned char* tuesday = sqlite3_column_text(stmt, 5);
            const unsigned char* wednesday = sqlite3_column_text(stmt, 6);
            const unsigned char* thursday = sqlite3_column_text(stmt, 7);
            const unsigned char* friday = sqlite3_column_text(stmt, 8);
            const unsigned char* saturday = sqlite3_column_text(stmt, 9);
            const unsigned char* sunday = sqlite3_column_text(stmt, 10);

            cout << setw(5) << empId
                << setw(10) << (const char*)name
                << setw(20) << (const char*)position
                << setw(20) << (const char*)branch
                << setw(13) << (const char*)monday
                << setw(13) << (const char*)tuesday
                << setw(13) << (const char*)wednesday
                << setw(13) << (const char*)thursday
                << setw(13) << (const char*)friday
                << setw(13) << (const char*)saturday
                << setw(13) << (const char*)sunday << endl;
        }

        if (!hasData) {
            cout << "No schedules found for the roster." << endl;
        }

    }
    else {
        cout << "Failed to view roster: " << sqlite3_errmsg(db) << endl;
    }

    cout << string(150, '=') << endl;

    sqlite3_finalize(stmt);
    closeDatabase(db);
}