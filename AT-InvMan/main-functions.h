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
void employeeMasterList();
void updateRoster();

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
    int employeeID;
    string shiftDate;
    string startTime;
    string endTime;
    string location;
    string note;
};

// FUNCTIONS FOR LOGIN
bool login(int empID, const string& password);

// FUNCTIONS FOR MENUS
void mainMenu();

//test functions will delete later
//void testInput();


#endif
