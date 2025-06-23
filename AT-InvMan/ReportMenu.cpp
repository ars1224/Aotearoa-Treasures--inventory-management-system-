#include <iostream>
#include <sqlite3.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include "InventoryManager.h"

using namespace std;

// Helper: Get the Monday-to-Sunday date range for a week offset from "today" (23/06/2025)
pair<string, string> getWeekDateRange(int weekNumber, int& yearOut) {
    // Fixed "today" as 23/06/2025
    tm today = {};
    today.tm_year = 2025 - 1900;
    today.tm_mon = 5; // June (0-based)
    today.tm_mday = 23;
    today.tm_hour = 0;
    today.tm_min = 0;
    today.tm_sec = 0;
    mktime(&today);

    // Find Monday of "this week"
    tm weekStart = today;
    int daysFromMonday = (weekStart.tm_wday == 0) ? 6 : (weekStart.tm_wday - 1);
    weekStart.tm_mday -= daysFromMonday;
    mktime(&weekStart);

    // Move back by (52 - weekNumber) weeks
    weekStart.tm_mday -= (52 - weekNumber) * 7;
    mktime(&weekStart);

    tm weekEnd = weekStart;
    weekEnd.tm_mday += 6;
    mktime(&weekEnd);

    yearOut = weekStart.tm_year + 1900;

    stringstream ssStart, ssEnd;
    ssStart << put_time(&weekStart, "%d/%m/%Y");
    ssEnd << put_time(&weekEnd, "%d/%m/%Y");

    return { ssStart.str(), ssEnd.str() };
}

// Helper: Always return 52 as the "current week" for 23/06/2025
int getCurrentWeekNumber() {
    return 52;
}

// Helper: Get the first and last day of a given month (1-12) and year, as DD/MM/YYYY
pair<string, string> getMonthDateRange(int year, int month) {
    tm first = {};
    first.tm_year = year - 1900;
    first.tm_mon = month - 1;
    first.tm_mday = 1;
    mktime(&first);

    // Find last day of month
    tm last = first;
    last.tm_mon += 1;
    last.tm_mday = 0;
    mktime(&last);

    stringstream ssStart, ssEnd;
    ssStart << put_time(&first, "%d/%m/%Y");
    ssEnd << put_time(&last, "%d/%m/%Y");
    return { ssStart.str(), ssEnd.str() };
}

// Callback to print query results
int query_callback(void* data, int argc, char** argv, char** colName) {
    if (argc == 0) return 0;
    for (int i = 0; i < argc; i++) {
        cout << (argv[i] ? argv[i] : "NULL") << "\t";
    }
    cout << endl;
    return 0;
}

// Fetch and display a table of SKU, Product Name, and total Quantity Sold for a specific location within a week
void fetchSalesData(sqlite3* db, const string& location, const string& startDate, const string& endDate) {
    string query =
        "SELECT SKU, Product_Name, SUM(Quantity_sold) as TotalQty "
        "FROM " + location +
        " WHERE "
        "substr(Sales_Date, 7, 4) || '-' || substr(Sales_Date, 4, 2) || '-' || substr(Sales_Date, 1, 2) "
        "BETWEEN '" +
        (startDate.substr(6,4) + "-" + startDate.substr(3,2) + "-" + startDate.substr(0,2)) +
        "' AND '" +
        (endDate.substr(6,4) + "-" + endDate.substr(3,2) + "-" + endDate.substr(0,2)) +
        "' "
        "GROUP BY SKU, Product_Name "
        "ORDER BY Product_Name ASC;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return;
    }

    cout << "\n=== " << location << " Sales Report ===\n";
    cout << left << setw(12) << "SKU"
         << left << setw(30) << "Product Name"
         << left << setw(10) << "Qty" << endl;
    cout << string(52, '-') << endl;

    bool hasRows = false;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        hasRows = true;
        string sku = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int qty = sqlite3_column_int(stmt, 2);

        cout << left << setw(12) << sku
             << left << setw(30) << name
             << left << setw(10) << qty << endl;
    }
    if (!hasRows) {
        cout << "No sales for this period.\n";
    }

    sqlite3_finalize(stmt);
}

// Fetch and display a table of SKU, Product Name, and total Quantity Sold for a specific location within a month
void fetchMonthlySalesData(sqlite3* db, const string& location, const string& startDate, const string& endDate) {
    string query =
        "SELECT SKU, Product_Name, SUM(Quantity_sold) as TotalQty "
        "FROM " + location +
        " WHERE "
        "substr(Sales_Date, 7, 4) || '-' || substr(Sales_Date, 4, 2) || '-' || substr(Sales_Date, 1, 2) "
        "BETWEEN '" +
        (startDate.substr(6,4) + "-" + startDate.substr(3,2) + "-" + startDate.substr(0,2)) +
        "' AND '" +
        (endDate.substr(6,4) + "-" + endDate.substr(3,2) + "-" + endDate.substr(0,2)) +
        "' "
        "GROUP BY SKU, Product_Name "
        "ORDER BY Product_Name ASC;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return;
    }

    cout << "\n=== " << location << " Monthly Sales Report ===\n";
    cout << left << setw(12) << "SKU"
         << left << setw(40) << "Product Name"
         << left << setw(10) << "Qty" << endl;
    cout << string(52, '-') << endl;

    bool hasRows = false;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        hasRows = true;
        string sku = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int qty = sqlite3_column_int(stmt, 2);

        cout << left << setw(12) << sku
             << left << setw(40) << name
             << left << setw(10) << qty << endl;
    }
    if (!hasRows) {
        cout << "No sales for this period.\n";
    }

    sqlite3_finalize(stmt);
}

// Fetch and display total sales quantity for a specific location within a week
void fetchSalesSummary(sqlite3* db, const string& location, const string& startDate, const string& endDate) {
    string query = "SELECT SUM(Quantity_sold) FROM " + location +
        " WHERE Sales_Date BETWEEN '" + startDate + "' AND '" + endDate + "';";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return;
    }

    rc = sqlite3_step(stmt);
    int totalQty = 0;
    if (rc == SQLITE_ROW) {
        totalQty = sqlite3_column_int(stmt, 0);
    }
    cout << location << "\t" << totalQty << endl;

    sqlite3_finalize(stmt);
}

// Main logic for generating the weekly sales summary report
void generateWeeklySalesReport() {
    int weekInput;
    cout << "Enter week number to display (1-52, or 52 = current week): ";
    cin >> weekInput;

    if (weekInput < 1 || weekInput > 52) {
        cout << "Invalid input. Please enter a week between 1 and 52.\n";
        return;
    }

    int targetWeek = weekInput;
    if (weekInput == 52) {
        targetWeek = getCurrentWeekNumber();
    }

    int year;
    pair<string, string> weekDateRange = getWeekDateRange(targetWeek, year);
    string startDate = weekDateRange.first;
    string endDate = weekDateRange.second;
    cout << "\nSales Report for Week " << targetWeek << " (" << startDate << " to " << endDate << "):\n";

    sqlite3* db;
    int rc = sqlite3_open("../db/at-invman.db", &db);
    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return;
    }

    fetchSalesData(db, "WLG_sale", startDate, endDate);
    fetchSalesData(db, "AKL_sales", startDate, endDate);
    fetchSalesData(db, "CHCH_sales", startDate, endDate);

    sqlite3_close(db);
}

// Main logic for generating the monthly sales report
void generateMonthlySalesReport() {
    int year = 2025, month = 6; // June 2025
    int minYear = 2024, minMonth = 6; // June 2024

    cout << "Select month and year for report (from June 2025 back to June 2024):\n";
    cout << "Enter year (2024 or 2025): ";
    cin >> year;
    cout << "Enter month (1-12): ";
    cin >> month;

    if (year < minYear || year > 2025 || (year == minYear && month < minMonth) || (year == 2025 && month > 6)) {
        cout << "Invalid input. Please select a month between June 2024 and June 2025.\n";
        return;
    }

    pair<string, string> monthRange = getMonthDateRange(year, month);
    string startDate = monthRange.first;
    string endDate = monthRange.second;
    cout << "\nSales Report for " << setw(2) << month << "/" << year
         << " (" << startDate << " to " << endDate << "):\n";

    sqlite3* db;
    int rc = sqlite3_open("../db/at-invman.db", &db);
    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return;
    }

    fetchMonthlySalesData(db, "WLG_sale", startDate, endDate);
    fetchMonthlySalesData(db, "AKL_sales", startDate, endDate);
    fetchMonthlySalesData(db, "CHCH_sales", startDate, endDate);

    sqlite3_close(db);
}
