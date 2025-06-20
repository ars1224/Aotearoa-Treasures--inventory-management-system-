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

void employees()
{
    char choice;

    do {
        if (isAdmin() || isStoreManager()) {
            cout << "\n========== Employee Management ==========\n";
            cout << "1. Employee List\n";
            cout << "2. Add New Employee\n";
            cout << "3. Update Employee\n";
            cout << "4. Remove Employee\n";
            cout << "5. Roster Management\n";
            cout << "6. Return to Main Menu\n";
            cout << "Select an option: ";
            cin >> choice;
            cout << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
            case '1': employeeList(); break;
            case '2': addEmployee(); break;
            case '3': updateEmployee(); break;
            case '4': deleteEmployee(); break;
            case '5': /*rosterMenu();*/ break;
            case '6': mainMenu(); break;
            default:
                cout << "Invalid selection. Please try again.\n";
            }
        }
        else if (isEmployee()) {
            cout << "\n========== Inventory Management ==========\n";
            cout << "1. Roster Management\n";
            cout << "2. Return to Main Menu\n";
            cout << "Select an option: ";
            cin >> choice;
            cout << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
            case '1': employeeList(); break;
            case '2': mainMenu(); break;
            default:
                cout << "Invalid selection. Please try again.\n";
            }
        }
    } while (true);
}
