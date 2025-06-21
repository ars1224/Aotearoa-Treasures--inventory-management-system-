// Dhona created this for POS system
// ===================================
// Header file for logging sales data to a branch-specific sales table in the database.
// This supports post-transaction recordkeeping and reporting by branch.
// ===================================

#ifndef SALES_LOGGER_H
#define SALES_LOGGER_H

#include <string>
#include <vector>
#include "pos.h" //For CartItem definition

// Logs each item in the cart to a branch-specific sales table (e.g., AKL_sales, CHCH_sales).
// Called after a successful transaction to record the SKU, product name, quantity, and date.
// Parameters:
// - branch: the currently logged-in branch (e.g., "Auckland", "Christchurch", "Wellington")
// - cartItems: vector of CartItem structs representing the final purchased items

void logSaleToBranchTable(const std::string& branch, const std::vector<CartItem>& cartItems);

#endif // SALES_LOGGER_H


