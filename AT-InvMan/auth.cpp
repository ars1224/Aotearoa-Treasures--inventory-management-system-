// Dhona created this snippet to implement the function getCurrentUserBranch for pos.cpp
// This function should be defined in auth.cpp to resolve linker errors such as LNK2019 or LNK2001
// It accesses currentUser.branch from session.h / session.cpp,
// assuming currentUser is declared as a global or external variable representing the authenticated user.

#include "auth.h"       // Declaration of getCurrentUserBranch()
#include "session.h"    // Access to the currentUser object which holds session data

// Retrieves and returns the branch assigned to the currently logged-in user.
// This is typically used to determine branch-specific behavior in POS logic.
std::string getCurrentUserBranch() {
    return currentUser.branch;
}
