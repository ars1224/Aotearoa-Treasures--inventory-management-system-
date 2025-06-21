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

void inventory(char skipOption)
{
    char choice;

    do {
        if (isAdmin() || isStoreManager()) {
            cout << "\n========== Inventory Management ==========\n";
            if (skipOption != '1') cout << "1. Product List\n";
            if (skipOption != '2') cout << "2. Add New Product\n";
            if (skipOption != '3') cout << "3. Update Existing Product\n";
            if (skipOption != '4') cout << "4. Delete Product\n";
            if (skipOption != '5') cout << "5. Point of Sale System\n";
            if (skipOption != '6') cout << "6. Return to Main Menu\n";
            cout << "Select an option: ";
            cin >> choice;
            cout << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
            case '1': productList(); inventory('1'); break;
            case '2': addProduct(); inventory('2'); break;
            case '3': updateProduct(); inventory('3'); break;
            case '4': deleteProduct(); inventory('4'); break;
            case '5': /*pos(); inventory('5');*/ break;
            case '6': mainMenu(); inventory('6'); break;
            default:
                cout << "Invalid selection. Please try again.\n";
            }
        }
        else if (isCashier()) {
            cout << "\n========== Inventory Management ==========\n";
            if (skipOption != '1') cout << "1. Product List\n";
            if (skipOption != '2') cout << "2. Point of Sale System\n";
            if (skipOption != '3') cout << "3. Return to Main Menu\n";
            cout << "Select an option: ";
            cin >> choice;
            cout << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
            case '1': productList(); inventory('1'); break;
            case '2': /*pos(); inventory('2');*/ break;
            case '3': mainMenu(); inventory('3'); break;
            default:
                cout << "Invalid selection. Please try again.\n";
            }
        } else if(isStockClerk()){
            cout << "\n========== Inventory Management ==========\n";
            if (skipOption != '1') cout << "1. Product List\n";
            if (skipOption != '2') cout << "2. Add New Product\n";
            if (skipOption != '3') cout << "3. Update Existing Product\n";
            if (skipOption != '4') cout << "4. Return to Main Menu\n";
            cout << "Select an option: ";
            cin >> choice;
            cout << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
            case '1': productList(); inventory('1'); break;
            case '2': addProduct(); inventory('2'); break;
            case '3': updateProduct(); inventory('3'); break;
            case '4': mainMenu(); inventory('4'); break;
            default:
                cout << "Invalid selection. Please try again.\n";
            }
        }
        else if (isSalesAssociate()) {
            cout << "\n========== Inventory Management ==========\n";
            if (skipOption != '1') cout << "1. Product List\n";
            if (skipOption != '2') cout << "2. Return to Main Menu\n";
            cout << "Select an option: ";
            cin >> choice;
            cout << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
            case '1': productList(); inventory('1'); break;
            case '2': mainMenu(); inventory('2'); break;
            default:
                cout << "Invalid selection. Please try again.\n";
            }
        }
    } while (true);
}
