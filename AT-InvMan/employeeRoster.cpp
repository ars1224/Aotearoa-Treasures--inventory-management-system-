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

    const char* sql = R"(
        SELECT schedules.schedule_id, schedules.Employee_ID, schedules.Employee_Name,
               schedules.Monday, schedules.Tuesday, schedules.Wednesday,
               schedules.Thursday, schedules.Friday
        FROM schedules;
    )";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        cout << endl;
        cout << "Opening Hours: 8 am to " << endl;
        cout << string(122, '=') << endl;
        cout << "                                                 Employee Roster Schedule" << endl;
        cout << string(122, '=') << endl;

        cout<< left << setw(10) << "  ID"
            << left << setw(15) << "Name"
            << left << setw(15) << "Monday"
            << left << setw(15) << "Tuesday"
            << left << setw(15) << "Wednesday"
            << left << setw(15) << "Thursday"
            << left << setw(15) << "Friday"
            << left << setw(15) << "Saturday"
            << left << setw(15) << "Sunday"
            << endl;

        cout << string(122, '=') << endl;

        bool hasData = false;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            hasData = true;

            int id = sqlite3_column_int(stmt, 0);
            int empId = sqlite3_column_int(stmt, 1);
            const unsigned char* name = sqlite3_column_text(stmt, 2);
            const unsigned char* monday = sqlite3_column_text(stmt, 3);
            const unsigned char* tuesday = sqlite3_column_text(stmt, 4);
            const unsigned char* wednesday = sqlite3_column_text(stmt, 5);
            const unsigned char* thursday = sqlite3_column_text(stmt, 6);
            const unsigned char* friday = sqlite3_column_text(stmt, 7);
            const unsigned char* saturday = sqlite3_column_text(stmt, 7);
            const unsigned char* sunday = sqlite3_column_text(stmt, 7);

            cout << left << setw(5) << id
                << setw(20) << (const char*)name
                << setw(10) << (const char*)monday
                << setw(10) << (const char*)tuesday
                << setw(10) << (const char*)wednesday
                << setw(10) << (const char*)thursday
                << setw(10) << (const char*)friday
                << setw(10) << (const char*)saturday 
                << setw(10) << (const char*)sunday << endl;
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

    cout << "Please select an option: "; cin >> option;

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
        exit(0);
    default:
        cout << "Invalid option. Please try again." << endl;
    }
}

