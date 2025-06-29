#ifndef SALES_LOGGER_H
#define SALES_LOGGER_H

#include <string>
#include <vector>
#include "pos.h" //For CartItem definition

void logSaleToBranchTable(const std::string& branch, const std::vector<CartItem>& cartItems);

#endif


