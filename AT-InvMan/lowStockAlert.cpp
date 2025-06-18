// Dhona Obina
// This is the alert system for inventory management.
// Admin or Store Owner ONLY Access
// Shows Invertory Levels Across All Branches
// Shows stock that is below five items

#include <iostream>
#include <sqlite3.h>
#include "main-functions.h"
#include <iomanip> // For table formatting

void alert() {
    sqlite3* db;
    sqlite3_stmt* stmt;
    const char* query = "SELECT Product_Name, ALK_Qty, CHCH_Qty, WLG_Qty FROM product WHERE ALK_Qty < 5 OR CHCH_Qty < 5 OR WLG_Qty < 5";

    if (sqlite3_open("../db/at-invman.db", &db) != SQLITE_OK) {
        cerr << "Error opening database: " << sqlite3_errmsg(db) << endl;
        return;
    }

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Error executing query: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    bool hasAlerts = false;

    // **Title Section**
    cout << "*************************************************************************************************************\n";
    cout << "********************                  LOW STOCK ALERT SYSTEM                             ********************\n";
    cout << "*************************************************************************************************************\n\n";

    // Table Headers
    cout << "=============================================================================================================\n";
    cout << "| LOCATION      | ITEM DESCRIPTION                 | QUANTITY |\n";
    cout << "=============================================================================================================\n";

    // **Auckland**
    bool hasAucklandAlerts = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string product_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int alk_qty = sqlite3_column_int(stmt, 1);

        if (alk_qty < 5) {
            hasAlerts = true;
            hasAucklandAlerts = true;
            cout << "| Auckland      | " << setw(30) << left << product_name << " | " << setw(8) << alk_qty << " |\n";
        }
    }
    if (!hasAucklandAlerts) cout << "| ✅ Auckland   | All items have sufficient stock.                 |\n";

    // **Reset & Check Christchurch**
    sqlite3_reset(stmt);
    bool hasChristchurchAlerts = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string product_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int chch_qty = sqlite3_column_int(stmt, 2);

        if (chch_qty < 5) {
            hasAlerts = true;
            hasChristchurchAlerts = true;
            cout << "| Christchurch  | " << setw(30) << left << product_name << " | " << setw(8) << chch_qty << " |\n";
        }
    }
    if (!hasChristchurchAlerts) cout << "| ✅ Christchurch | All items have sufficient stock.              |\n";

    // **Reset & Check Wellington**
    sqlite3_reset(stmt);
    bool hasWellingtonAlerts = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string product_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int wlg_qty = sqlite3_column_int(stmt, 3);

        if (wlg_qty < 5) {
            hasAlerts = true;
            hasWellingtonAlerts = true;
            cout << "| Wellington    | " << setw(30) << left << product_name << " | " << setw(8) << wlg_qty << " |\n";
        }
    }
    if (!hasWellingtonAlerts) cout << "| ✅ Wellington  | All items have sufficient stock.              |\n";

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    cout << "============================================================================================================\n";
    cout << "============================================================================================================\n\n\n";
}



