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
void updateProduct();
void deleteProduct();
void inventory(char skipOption = '\0');
void ListOnly();
void productList();
void salesReport();
string generateNextSKU();

void generateMonthlySalesReport();
#endif