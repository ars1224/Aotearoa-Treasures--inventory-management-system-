#include <iostream>
#include <sqlite3.h>

int main() {
    sqlite3* DB;
    int exit;

    std::string dbPath = "../db/at-invman.db";

    exit = sqlite3_open(dbPath.c_str(), &DB);

    if (exit) {
        std::cerr << "? Error opening DB: " << sqlite3_errmsg(DB) << std::endl;
        return -1;
    }
    else {
        std::cout << "? Opened database successfully!" << std::endl;
    }

    sqlite3_close(DB);
    return 0;
}