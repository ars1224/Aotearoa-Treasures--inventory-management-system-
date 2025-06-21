// Dhona created this snippet to implement the function getCurrentUserBranch for pos.cpp
// this should be defined in auth.cpp which fixed the linker error (LNK2019, LNK2001)
// It uses currenUser.branch from session.h / session.cpp assuming currentUser is a global variable or external object.

#include "auth.h"
#include "session.h"

// Returns the current user's assigned branch
std::string getCurrentUserBranch() {
    return currentUser.branch;
}