#ifndef INVENTORYMANAGER_H
#define INVENTORYMANAGER_H

#include <string>

using namespace std;

struct Product {
    string Product_Code;
    string Product_Name;
    string Category;
    int ALK_Qty;   
    int CHCH_Qty;
    int WLG_Qty;
    double price;
};

// Function declarations
void productList();
void addProduct();
void updateProduct(sqlite3* db);
void deleteProduct(sqlite3* db);
void inventory();
void productList();
string generateNextSKU();

#endif