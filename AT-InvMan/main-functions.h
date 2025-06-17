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
void addRoster();
void roster();
void updateRoster();
void deleteRoster();
void rosterlist();
void employeeMasterlist();

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


// FUNCTIONS FOR LOGIN
bool login(int empID, const string& password);

// FUNCTIONS FOR MENUS
void mainMenu();

#endif
