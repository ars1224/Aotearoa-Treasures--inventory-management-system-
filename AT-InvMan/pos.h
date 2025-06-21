#ifndef POS_H
#define POS_H
#include <vector>
#include <string>
#include <sqlite3.h>
#pragma once

// Struct for cart items
struct CartItem {
    std::string code;
    std::string name;
    double price;
    int quantity;
};

// Class for POS logic
class POS {
private:
    std::vector<CartItem> cart;
    sqlite3* db;
    std::string userBranch;

    void connectDB();
    void closeDB();
    void updateInventory(const std::string& code, int qty);

public:
    POS();                  // Constructor
    POS(const std::string& branch); // Constructor with branch
    ~POS();                 // Destructor

    void scanItem();        // Add item to cart
    void printReceipt();    // Show the receipt

    std::vector<CartItem> getCart() const; 
};

#endif
