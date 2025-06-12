#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
#include "auth.h"
using namespace std;

void mainMenu() {
    char choice;
    cout << "\n=======================================================================\n";
    cout << "     Welcome to Aotearoa Treasures’ Inventory Management System\n";
    cout << "=======================================================================\n";
    cout << "                             Main Menu\n";
    cout << "=======================================================================\n\n";

    do {
        if (isAdmin() || isStoreManager()) {
            cout << "1. Inventory Management" << endl;
        }

        if (!isSalesAssociate()) {
            cout << "2. Roster Management" << endl;
        }

        cout << "3. Employee List" << endl;

        if (isAdmin() || isStoreManager()) {
            cout << "4. Sales Report" << endl;
        }

        cout << "5. Exit" << endl;
        cout << "\nPlease select an option from the menu above: ";
        cin >> choice;

        switch (choice) {
        case '1':
            if (isAdmin() || isStoreManager()) {
                // inventoryManagement();
            }
            else {
                cout << "❌ Access denied.\n";
            }
            break;
        case '2':
            if (!isSalesAssociate()) {
                roster();
            }
            else {
                cout << "❌ Access denied.\n";
            }
            break;
        case '3':
            employeeList();
            break;
        case '4':
            if (isAdmin() || isStoreManager()) {
                // salesReport();
            }
            else {
                cout << "❌ Access denied.\n";
            }
            break;
        case '5':
            cout << "Exiting the system. Goodbye!\n";
            exit(0);
        default:
            cout << "Invalid choice. Please try again.\n";
            break;
        }
    } while (true);
}
