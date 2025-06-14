#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
#include "auth.h"
using namespace std;

void roster()
{
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
        cout << string(122, '=') << endl;
        cout << "                                                 Employee Roster Schedule" << endl;
        cout << string(122, '=') << endl;
        cout << endl;
        cout << setw(75) << "Opening Shift: 7am to 3:30pm " << endl;
        cout << setw(75) << "Closing Shift: 10:30am to 7pm " << endl;
        cout << endl;
        cout << string(140, '=') << endl;

        cout<< setw(20) <<"Employee ID"
            << setw(15) << "Name"
            << setw(20) << "Position"
            << setw(15) << "Branch"
            << setw(15) << "Monday"
            << setw(15) << "Tuesday"
            << setw(15) << "Wednesday"
            << setw(15) << "Thursday"
            << setw(15) << "Friday"
            << setw(15) << "Saturday"
            << setw(15) << "Sunday"
            << endl;

        cout << string(140, '=') << endl;

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

            cout << setw(20) << empId
                << setw(15) << (const char*)name
                << setw(20) << (const char*)position
                << setw(15) << (const char*)branch
                << setw(15) << (const char*)monday
                << setw(15) << (const char*)tuesday
                << setw(15) << (const char*)wednesday
                << setw(15) << (const char*)thursday
                << setw(15) << (const char*)friday
                << setw(15) << (const char*)saturday
                << setw(15) << (const char*)sunday << endl;
        }

        if (!hasData) {
            cout << "No schedules found for the roster." << endl;
        }

    }
    else {
        cout << "Failed to view roster: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    closeDatabase(db);

    int option;
    cout << endl;
    cout << "Related options on Employee\n";
    cout << endl;
    cout << "1. Make a new schedule\n";
    cout << "2. Update schedule\n";
    cout << "3. Add Employee\n";
    cout << "4. Update Employee\n";
    cout << "5. Delete Employee\n";
    cout << "6. Back to Main Menu\n";
    cout << "7. Exit Program\n";

    cout << "Please select an option: ";
    cin >> option;

    switch (option) {
    case 1:
        cout << endl;
        employeeMasterList();
        addRoster();
        break;
    case 2:
        cout << endl;
        updateRoster();
        break;
    case 3:
        cout << endl;
        addEmployee();
        break;
    case 4:
        cout << endl;
        updateEmployee();
        break;
    case 5:
        cout << endl;
        deleteEmployee();
        break;
    case 6:
        mainMenu();
        break;
    case 7:
        return;
    default:
        cout << "Invalid option. Please try again." << endl;
    }
}







