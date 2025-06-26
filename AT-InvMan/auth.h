#ifndef AUTH_UTILS_H
#define AUTH_UTILS_H

#include "session.h" 

inline bool isAdmin() { return currentUser.position == "Administrator"; }
inline bool isStoreManager() { return currentUser.position == "Store Manager"; }
inline bool isCashier() { return currentUser.position == "Cashier"; }
inline bool isStockClerk() { return currentUser.position == "Stock Clerk"; }
inline bool isSalesAssociate() { return currentUser.position == "Sales Associate"; }

inline bool isEmployee() {
    return isCashier() || isStockClerk() || isSalesAssociate();
}

std::string getCurrentUserBranch();

#endif

//Aries
//Dhona