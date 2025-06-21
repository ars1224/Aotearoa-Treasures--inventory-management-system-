// Aries and Dhona worked on this to implement the POS system
// This utility header provides role-checking functions and 
// access to the user's assigned branch for use across the system.

// It supports user-specific logic such as stock access, permissions, and branch filtering
// Used heavily in POS, inventory, and other role-sensitive operations.
#ifndef AUTH_UTILS_H
#define AUTH_UTILS_H

#include "session.h" // Provides access to the global currentUser object from login session

// Role checkers: Determine the current user's role based on their position
inline bool isAdmin() { return currentUser.position == "Administrator"; }
inline bool isStoreManager() { return currentUser.position == "Store Manager"; }
inline bool isCashier() { return currentUser.position == "Cashier"; }
inline bool isStockClerk() { return currentUser.position == "Stock Clerk"; }
inline bool isSalesAssociate() { return currentUser.position == "Sales Associate"; }

// General employee check (non-managerial)
inline bool isEmployee() {
    return isCashier() || isStockClerk() || isSalesAssociate();
}

// Dhona added this to support branch-specific functionality (used in POS, inventory, etc.)
// Returns the branch of the currently logged-in user
std::string getCurrentUserBranch();  

#endif // AUTH_UTILS_H
