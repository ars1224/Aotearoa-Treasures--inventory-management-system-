#ifndef DB_CONN_H
#define DB_CONN_H

#include <sqlite3.h>

sqlite3* connectToDatabase();
void closeDatabase(sqlite3* db);

#endif
