// weekly_sales_report.cpp
#include <iostream>
#include <iomanip>
#include <string>
#include <sqlite3.h>
using namespace std;

void generateWeeklyReport(sqlite3* db, const string& location) {
    string dateField = "sales_date";
    string timeGroup = "strftime('%Y-%W', date(substr(" + dateField + ", 7, 4) || '-' || substr(" + dateField + ", 4, 2) || '-' || substr(" + dateField + ", 1, 2)))";
    string displayGroup = "strftime('%d-%m-%Y', date(substr(" + dateField + ", 7, 4) || '-' || substr(" + dateField + ", 4, 2) || '-' || substr(" + dateField + ", 1, 2), 'weekday 1', '-6 days'))";

    string sql;
    if (location == "ALL") {
        sql =
            "SELECT " + displayGroup + " AS period, 'WLG' AS location, SUM(quantity) FROM WLG_Sales GROUP BY " + timeGroup + " UNION ALL "
            "SELECT " + displayGroup + ", 'CHCH', SUM(quantity) FROM CHCH_Sales GROUP BY " + timeGroup + " UNION ALL "
            "SELECT " + displayGroup + ", 'AKL', SUM(quantity) FROM AKL_Sales GROUP BY " + timeGroup + " ORDER BY period DESC, location;";
    }
    else {
        string table = location + "_Sales";
        sql = "SELECT " + displayGroup + " AS period, '" + location + "' AS location, SUM(quantity) FROM " + table + " GROUP BY " + timeGroup + " ORDER BY period DESC;";
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
