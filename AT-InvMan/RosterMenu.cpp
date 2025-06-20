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

void RosterMenu()
{
    char choice;

    do {
        if (isAdmin() || isStoreManager()) {
            cout << "\n========== Employee Management ==========\n";
            cout << "1. View Roster\n";
            cout << "2. Add New Roster\n";
            cout << "3. Update Roster\n";
            cout << "4. Delete Roster\n";
            cout << "5. Return to Main Menu\n";
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
        else if (isCashier()) {
            cout << "\n========== Inventory Management ==========\n";
            cout << "1. Product List\n";
            cout << "2. Point of Sale System\n";
            cout << "3. Return to Main Menu\n";
            cout << "Select an option: ";
            cin >> choice;
            cout << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
            case '1': productList(); break;
            case '2': /*pos();*/ break;
            case '3': mainMenu(); break;
            default:
                cout << "Invalid selection. Please try again.\n";
            }
        }
        else if (isStockClerk()) {
            cout << "\n========== Inventory Management ==========\n";
            cout << "1. Product List\n";
            cout << "2. Add New Product\n";
            cout << "3. Update Existing Product\n";
            cout << "4. Return to Main Menu\n";
            cout << "Select an option: ";
            cin >> choice;
            cout << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
            case '1': productList(); break;
            case '2': addProduct(); break;
            case '3': updateProduct(); break;
            case '4': mainMenu(); break;
            default:
                cout << "Invalid selection. Please try again.\n";
            }
        }
        else if (isSalesAssociate()) {
            cout << "\n========== Inventory Management ==========\n";
            cout << "1. Product List\n";
            cout << "2. Return to Main Menu\n";
            cout << "Select an option: ";
            cin >> choice;
            cout << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
            case '1': productList(); break;
            case '2': mainMenu(); break;
            default:
                cout << "Invalid selection. Please try again.\n";
            }
        }
    } while (true);
}
