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
    cout<<right <<setw(90) << "Welcome to Aotearoa Treasures’ Inventory Management System\n";
    cout << string(120, '-') << endl;

    while (true)
    {
        cout << string(120, '-') << endl;
        cout <<right<<setw(65)<< "Main Menu\n";
        cout << string(120, '-') << endl;
        cout << endl;

      /* if (isAdmin()) {
            alert();
        }*/

        if (isAdmin() || isStoreManager()) {
            cout << "1. Inventory Management" << endl;
            cout << "2. Roster Management" << endl;
            cout << "3. Employee List" << endl;
            cout << "4. Sales Report" << endl;
            cout << "5. Exit" << endl;

            cout << "\nPlease select an option from the menu above: ";
            cin >> choice;

            switch (choice) {
            case '1':
                productList();
                break;

            case '2':
                if (isAdmin() || isStoreManager()) {
                    roster();
                }
                else {
                    cout << "❌ Access denied.\n";
                }
                break;

            case '3':
                if (isAdmin() || isStoreManager()) {
                    employeeList();
                }
                else {
                    cout << "❌ Access denied.\n";
                }
                break;

            case '4':
                if (isAdmin() || isStoreManager()) {
                    salesReportMenu();
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
        }
        else { // non-admin/user
            cout << "1. Inventory Management" << endl;
            cout << "2. Roster Management" << endl;
            cout << "3. Exit" << endl;

            cout << "\nPlease select an option from the menu above: ";
            cin >> choice;

            switch (choice) {
            case '1':
                inventory();
                break;

            case '2':
                roster();
                break;

            case '3':
                cout << "Exiting the system. Goodbye!\n";
                exit(0);

            default:
                cout << "Invalid choice. Please try again.\n";
                break;
            }
        }
    }
}

