#pragma once
#ifndef MAIN_FUNCTIONS_H
#define MAIN_FUNCTIONS_H

#include <string>
using namespace std;

// FUNCTIONS FOR EMPLOYEES
void employeeList();
void addEmployee();
void updateEmployee();
void deleteEmployee();
void employeeMenu(char skipOption = '\0');

// FUNCTIONS FOR ROSTERS
void rosterlist(); 
void RosterMenu(char skipOption = '\0');
void addRoster();
void updateRoster();
void deleteRoster();
void rosterlist();
void employeeWithoutSchedule();

// STRUCT FOR EMPLOYEES
struct employee {
    string id;
    string name;
    string position;
    string status;
    string password;
    string branch;
};

// STRUCT FOR ROSTERS
struct rosterEntry {
    int scheduleId;
    int employeeId;
    string employeeName;
    string employeePossition;
	string employeeBranch;
    string monday;
    string tuesday;
    string wednesday;
    string thursday;
    string friday;
    string saturday;
    string sunday;
};

// FUNCTIONS FOR REPORTS
//void salesReportMenu();
//void runSalesReport(sqlite3* db);

// FUNCTIONS FOR LOGIN
bool login(int empID, const string& password);

// FUNCTIONS FOR MENUS
void mainMenu();

//FUNCTIONS FOR ALERTS
void alert();

// FUNCTIONS FOR ROSTERS
const char* safeText(const unsigned char* text);
#endif
