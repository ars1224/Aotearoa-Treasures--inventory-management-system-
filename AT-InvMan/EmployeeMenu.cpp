#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
#include "auth.h"
#include <limits>
#include <sstream>
#include "InventoryManager.h"

using namespace std;

void employeeMenu(char skipOption) {
    char choice;

    do {
        if (isAdmin() || isStoreManager()) {
            cout << "\n========== Employee Management ==========\n";
            if (skipOption != '1') cout << "1. Employee List\n";
            if (skipOption != '2') cout << "2. Add New Employee\n";
            if (skipOption != '3') cout << "3. Update Employee\n";
            if (skipOption != '4') cout << "4. Remove Employee\n";
            if (skipOption != '5') cout << "5. Roster Management\n";
            if (skipOption != '6') cout << "6. Return to Main Menu\n";

            cout << "Select an option: ";
            cin >> choice;
            cout << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
            case '1': employeeList(); employeeMenu('1'); break;
            case '2': addEmployee();  employeeMenu('2'); break;
            case '3': updateEmployee();  employeeMenu('3'); break;
            case '4': deleteEmployee();  employeeMenu('4'); break;
            case '5': /*rosterMenu();*/  employeeMenu('5'); break;
            case '6': mainMenu(); break;
            default:
                cout << "Invalid selection. Please try again.\n";
            }
        }
        else if (isEmployee()) {
            cout << "\n========== Inventory Management ==========\n";
            if (skipOption != '1') cout << "1. Roster Management\n";
            if (skipOption != '2') cout << "2. Return to Main Menu\n";

            cout << "Select an option: ";
            cin >> choice;
            cout << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
            case '1': employeeList(); employeeMenu('1'); break;
            case '2': mainMenu(); break;
            default:
                cout << "Invalid selection. Please try again.\n";
            }
        }
    } while (true);
}

