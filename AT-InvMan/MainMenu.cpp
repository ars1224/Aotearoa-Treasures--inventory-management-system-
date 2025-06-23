#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
#include "InventoryManager.h"
#include "auth.h"

using namespace std;

void mainMenu()
{
    char choice;
    cout << endl;
    cout << string(120, '-') << endl;
    cout << right << setw(90) << "Welcome to Aotearoa Treasures’ Inventory Management System\n";
    cout << string(120, '-') << endl;

    while (true)
    {
        cout << string(120, '-') << endl;
        cout << right << setw(65) << "Main Menu\n";
        cout << string(120, '-') << endl;
        cout << endl;

        if (isAdmin() || isStoreManager()) {
            cout << "1. Inventory Management" << endl;
            cout << "2. Roster Management" << endl;
            cout << "3. Employee Management" << endl;
            cout << "4. Sales Report" << endl;
            cout << "5. Alert: Low Product Qty" << endl;
            cout << "6. Exit" << endl;
            cout << endl;
            cout << "\nPlease select an option from the menu above: "; cin >> choice;
            cout << endl;

            switch (choice) {
            case '1': inventory(); break;
            case '2': RosterMenu(); break;
            case '3': employeeMenu(); break;
            case '4': generateMonthlySalesReport(); break;
            case '5': alert(); break;
            case '6': cout << "Exiting the system. Goodbye!\n"; exit(0);
            default: cout << "Invalid choice. Please try again.\n"; break;
            }
        }

        else if (isEmployee()) {
            cout << "1. Inventory Management" << endl;
            cout << "2. Roster Management" << endl;
            cout << "3. Exit" << endl;
            cout << endl;
            cout << "\nPlease select an option from the menu above: "; cin >> choice;
            cout << endl;

            switch (choice) {
            case '1': inventory(); break;
            case '2': RosterMenu(); break;
            case '3': cout << "Exiting the system. Goodbye!\n"; exit(0);
            default: cout << "Invalid choice. Please try again.\n"; break;
            }
        }
    }
}

