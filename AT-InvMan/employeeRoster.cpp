#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
#include "auth.h"
using namespace std;

void roster() {
    sqlite3* db = connectToDatabase();
    if (db == nullptr) return;

    const char* sql = R"(
        SELECT * FROM schedules
        WHERE DATE(shift_date) BETWEEN DATE('now') AND DATE('now', '+28 days')
        ORDER BY shift_date ASC;
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        cout << endl;
        cout << "\n=======================================================================\n";
        cout << "                    Roster Schedule (Next 4 Weeks):\n";
        cout << "========================================================================\n";
        cout << endl;
        cout << "\nRoster Schedule (Next 4 Weeks):\n";
        cout << left << setw(5) << "ID"
            << setw(12) << "Emp #"
            << setw(20) << "Name"
            << setw(15) << "Position"
            << setw(12) << "Date"
            << setw(10) << "Start"
            << setw(10) << "End"
            << setw(15) << "Location"
            << "Note" << endl;
        cout << string(115, '-') << endl;

        bool hasData = false;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            hasData = true;
            int id = sqlite3_column_int(stmt, 0);
            int empID = sqlite3_column_int(stmt, 1);
            const unsigned char* name = sqlite3_column_text(stmt, 2);
            const unsigned char* poss = sqlite3_column_text(stmt, 3);
            const unsigned char* date = sqlite3_column_text(stmt, 4);
            const unsigned char* start = sqlite3_column_text(stmt, 5);
            const unsigned char* end = sqlite3_column_text(stmt, 6);
            const unsigned char* location = sqlite3_column_text(stmt, 7);
            const unsigned char* note = sqlite3_column_text(stmt, 8);
            if (!note) note = (const unsigned char*)"";

            printf("%-5d %-12d %-20s %-15s %-12s %-10s %-10s %-15s %s\n",
                id, empID, name, poss, date, start, end, location, note);
        }

        if (!hasData) {
            cout << "No schedules found for the next 4 weeks.\n";
        }
    }
    else {
        cerr << "Failed to view roster: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    closeDatabase(db);

    int option;
    cout << "\nRelated options on Employee\n";
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
        exit(0);
    default:
        cout << "Invalid option. Please try again." << endl;
    }
}
