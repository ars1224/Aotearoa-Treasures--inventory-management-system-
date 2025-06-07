#ifndef EMPLOYEE_H
#define EMPLOYEE_H

void userLogin();
void employeeList();
void addEmployee();
void updateEmployee();
void deleteEmployee();


//void roster();


#include <string>
using namespace std;

struct employee {
    string id;
    string name;
    string position;
    string status;
    string password;
};

#endif



