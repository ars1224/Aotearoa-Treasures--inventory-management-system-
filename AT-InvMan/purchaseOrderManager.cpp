#include "purchaseOrderManager.h"
#include <sqlite3.h>
#include <iostream>

using namespace std;

// Create a new PO and return its ID
int createPurchaseOrder(const std::string& date, const std::string& branch, const std::string& supplier) {
    sqlite3* db;
    sqlite3_open("../db/at-invman.db", &db);

    const char* sql = "INSERT INTO purchase_orders (Order_Date, Branch, Supplier_Name) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;

    int poNumber = -1;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, date.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, branch.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, supplier.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            poNumber = (int)sqlite3_last_insert_rowid(db);
            cout << "Purchase Order Created. PO Number: " << poNumber << endl;
        }
        else {
            cerr << "Failed to create PO.\n";
        }
        sqlite3_finalize(stmt);
    }

    sqlite3_close(db);
    return poNumber;
}

// Add items to a specific PO
void addItemToPurchaseOrder(int poNumber, const std::vector<PurchaseItem>& items) {
    sqlite3* db;
    sqlite3_open("../db/at-invman.db", &db);

    const char* sql = "INSERT INTO purchase_order_items (PO_Number, Product_Code, Quantity_Ordered) VALUES (?, ?, ?);";

    for (const auto& item : items) {
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, poNumber);
            sqlite3_bind_text(stmt, 2, item.productCode.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 3, item.quantity);

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                cerr << "❌ Failed to add item: " << item.productCode << endl;
            }
            sqlite3_finalize(stmt);
        }
    }

    cout << "Items added to PO #" << poNumber << endl;
    sqlite3_close(db);
}

// Show all POs with items
void listPurchaseOrders() {
    sqlite3* db;
    sqlite3_open("../db/at-invman.db", &db);

    const char* sql = R"(
        SELECT po.PO_Number, po.Order_Date, po.Branch, po.Supplier_Name, po.Status,
               i.Product_Code, i.Quantity_Ordered
        FROM purchase_orders po
        LEFT JOIN purchase_order_items i ON po.PO_Number = i.PO_Number
        ORDER BY po.PO_Number;
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        cout << "\n PURCHASE ORDERS:\n";
        cout << "--------------------------------------------------------------\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int po = sqlite3_column_int(stmt, 0);
            string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            string branch = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            string supplier = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            string status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            const char* product = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            int qty = sqlite3_column_int(stmt, 6);

            cout << "PO#" << po << " | " << date << " | " << branch
                << " | " << supplier << " | Status: " << status
                << " | Product: " << (product ? product : "-")
                << " x" << (product ? qty : 0) << "\n";
        }
        sqlite3_finalize(stmt);
    }

    sqlite3_close(db);
}
