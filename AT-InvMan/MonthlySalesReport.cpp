#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <sqlite3.h>
#include <map>
#include <vector>

using namespace std;

struct Sale {
    string sku;
    string name;
    string date;
    int quantity;
};

// Converts date string DD-MM-YYYY to tm struct
bool parseDate(const string& dateStr, tm& dateTm) {
    istringstream ss(dateStr);
    string day, month, year;
    if (getline(ss, day, '-') && getline(ss, month, '-') && getline(ss, year)) {
        dateTm = {};
        dateTm.tm_mday = stoi(day);
        dateTm.tm_mon = stoi(month) - 1;
        dateTm.tm_year = stoi(year) - 1900;
        return true;
    }
    return false;
}

bool isSameMonth(const string& dateStr, int month, int year) {
    tm dt;
    if (!parseDate(dateStr, dt)) return false;
    return dt.tm_mon == month && dt.tm_year == (year - 1900);
}

vector<Sale> fetchSales(sqlite3* db, const string& table, int month, int year) {
    vector<Sale> sales;
    string sql = "SELECT SKU, Product_Name, Sale_Date, Quantity FROM " + table;

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            if (isSameMonth(date, month, year)) {
                Sale s;
                s.sku = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                s.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                s.date = date;
                s.quantity = sqlite3_column_int(stmt, 3);
                sales.push_back(s);
            }
        }
    }
    sqlite3_finalize(stmt);
    return sales;
}

void printMonthlyReport(sqlite3* db, const string& location, int month, int year) {
    vector<string> locations = { "CHCH_Sales", "WLG_Sales", "AKL_Sales" };
    if (location != "ALL") locations = { location + "_Sales" };

    map<string, int> totals;

    for (const auto& table : locations) {
        vector<Sale> sales = fetchSales(db, table, month, year);
        for (const auto& sale : sales) {
            string key = sale.sku + " | " + sale.name;
            totals[key] += sale.quantity;
        }
    }

    cout << "\n\n========= Monthly Sales Report =========\n";
    cout << "Month: " << setfill('0') << setw(2) << month << "-" << year << "\n";
    if (location != "ALL") cout << "Location: " << location << "\n";
    cout << left << setw(15) << "SKU" << setw(40) << "Product Name" << "Quantity Sold\n";
    cout << string(70, '-') << "\n";

    for (const auto& entry : totals) {
        istringstream ss(entry.first);
        string sku, name;
        getline(ss, sku, '|');
        getline(ss, name);
        cout << left << setw(15) << sku << setw(40) << name << entry.second << "\n";
    }
    cout << "========================================\n\n";
}

void generateMonthlySalesReport() {
    sqlite3* db;
    if (sqlite3_open("../db/at-invman.db", &db) != SQLITE_OK) {
        cerr << "Failed to open database.\n";
        return;
    }

    string location;
    int month, year;

    cout << "Enter location (CHCH, WLG, AKL or ALL): ";
    cin >> location;
    cout << "Enter month (1-12): ";
    cin >> month;
    cout << "Enter year (e.g. 2024): ";
    cin >> year;

    printMonthlyReport(db, location, month, year);
    sqlite3_close(db);
}
