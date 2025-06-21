#ifndef PURCHASE_ORDER_MANAGER_H
#define PURCHASE_ORDER_MANAGER_H

#include <string>
#include <vector>

struct PurchaseItem {
    std::string productCode;
    int quantity;
};

int createPurchaseOrder(const std::string& date, const std::string& branch, const std::string& supplier);
void addItemToPurchaseOrder(int poNumber, const std::vector<PurchaseItem>& items);
void listPurchaseOrders();

#endif
