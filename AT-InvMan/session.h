#ifndef SESSION_H
#define SESSION_H

#include <string>

struct SessionUser {
    int id;
    std::string name;
    std::string position;
    std::string branch;
};

extern SessionUser currentUser;

#endif // SESSION_H