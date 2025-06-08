#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <string>
using namespace std;

void userLogin();
void employeeList();
void addEmployee();
void updateEmployee();
void deleteEmployee();
void addRoster();
void roster();
void employeeMasterList();
void updateRoster();

struct employee {
    string id;
    string name;
    string position;
    string status;
    string password;
    string branch;
};

struct rosterEntry {
    int employeeID;
    string shiftDate;
    string startTime;
    string endTime;
    string location;
    string note;
};


#endif