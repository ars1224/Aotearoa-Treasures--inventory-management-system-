#pragma once
#ifndef DB_CON_H
#define DB_CON_H

#include <sqlite3.h>
#include <string>

sqlite3* connectToDatabase();
void closeDatabase(sqlite3* db);


#endif
