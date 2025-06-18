#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "db-conn.h"
#include "main-functions.h"
#include "InventoryManager.h"
#include "auth.h"


using namespace std;

void generateSalesReport(sqlite3* db, const string& period, const string& location) {
    string timeGroup, displayGroup, dateField = "sales_date";

    if (period == "WEEKLY") {
        // Group by week, format to Monday date of that week
        timeGroup = "strftime('%Y-%W', date(substr(" + dateField + ", 7, 4) || '-' || substr(" + dateField + ", 4, 2) || '-' || substr(" + dateField + ", 1, 2)))";
        displayGroup = "strftime('%d-%m-%Y', date(substr(" + dateField + ", 7, 4) || '-' || substr(" + dateField + ", 4, 2) || '-' || substr(" + dateField + ", 1, 2), 'weekday 1', '-6 days'))";
    }
    else if (period == "MONTHLY") {
        // Group by month, show 01-MM-YYYY
        timeGroup = "strftime('%Y-%m', date(substr(" + dateField + ", 7, 4) || '-' || substr(" + dateField + ", 4, 2) || '-' || substr(" + dateField + ", 1, 2)))";
        displayGroup = "strftime('%d-%m-%Y', date(substr(" + dateField + ", 7, 4) || '-' || substr(" + dateField + ", 4, 2) || '-01'))";
    }
    else if (period == "YEARLY") {
        // Group by year, show 01-01-YYYY
        timeGroup = "strftime('%Y', date(substr(" + dateField + ", 7, 4) || '-' || substr(" + dateField + ", 4, 2) || '-' || substr(" + dateField + ", 1, 2)))";
        displayGroup = "strftime('%d-%m-%Y', date(substr(" + dateField + ", 7, 4) || '-01-01'))";
    }
    else {
        cerr << "Invalid period. Use WEEKLY, MONTHLY, or YEARLY." << endl;
        return;
    }

    string sql;

    if (location == "ALL") {
        sql =
            "SELECT " + displayGroup + " AS period, 'WLG' AS location, SUM(quantity) "
            "FROM WLG_Sales GROUP BY " + timeGroup + " "
            "UNION ALL "
            "SELECT " + displayGroup + ", 'CHCH', SUM(quantity) "
            "FROM CHCH_Sales GROUP BY " + timeGroup + " "
            "UNION ALL "
            "SELECT " + displayGroup + ", 'AKL', SUM(quantity) "
            "FROM AKL_Sales GROUP BY " + timeGroup + " "
            "ORDER BY period DESC, location;";
    }
    else {
        string table = location + "_Sales";
        sql = "SELECT " + displayGroup + " AS period, '" + location + "' AS location, SUM(quantity) "
            "FROM " + table + " GROUP BY " + timeGroup + " ORDER BY period DESC;";
    }

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    cout << left << setw(15) << "Period" << setw(10) << "Location" << setw(12) << "Total Sold" << endl;
    cout << string(37, '-') << endl;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string periodStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string loc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int totalSold = sqlite3_column_int(stmt, 2);

        cout << left << setw(15) << periodStr << setw(10) << loc << setw(12) << totalSold << endl;
    }

    sqlite3_finalize(stmt);
}
