#include <iostream>
#include "db-conn.h"
#include "login.h"
using namespace std;

int main() {
	
	userLogin();
	//invetory();//
	//roster();//
	//employeeList();//
	//salesReport();//

	connectToDatabase();

    return 0;
}