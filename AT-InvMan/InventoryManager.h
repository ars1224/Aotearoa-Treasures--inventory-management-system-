#ifndef INVENTORYMANAGER_H
#define INVENTORYMANAGER_H

#include <string>

using namespace std;

struct Product {
    string Product_Code;
    string Product_Name;
    string Category;
    int CHCH_Qty;
    int AKL_Qty;
    int WLG_Qty;
    double price;
};

// Function declarations
void inventory();
void inventory();
void productList(sqlite3* db);
void addProduct(sqlite3* db);
void updateProduct(sqlite3* db);
void deleteProduct(sqlite3* db);

// Optional DB helper functions
void initializeInventoryDB(const string& dbFile);
bool connectInventoryDB();
void closeInventoryDB();

#endif