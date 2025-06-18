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

void inventory()
{
    char choice;

    do {
        cout << "\n========== Inventory Management ==========\n";
        cout << "1. View Product List\n";
        cout << "2. Add New Product\n";
        cout << "3. Update Existing Product\n";
        cout << "4. Delete Product\n";
        cout << "5. Return to Main Menu\n";
        cout << "Select an option: ";
        cin >> choice;
        cout << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
        case '1': productList(); break;
        case '2': {
            sqlite3* db = connectToDatabase();
            if (db) {
                addProduct();
                closeDatabase(db);
            }
            break;
        }
        case '3': {
            sqlite3* db = connectToDatabase();
            if (db) {
                updateProduct(db);
                closeDatabase(db);
            }
            break;
        }
        case '4': {
            sqlite3* db = connectToDatabase();
            if (db) {
                deleteProduct(db);
                closeDatabase(db);
            }
            break;
        }
        case '5': return;

        default:
            cout << "Invalid selection. Please try again.\n";
        }
    } while (true);
}