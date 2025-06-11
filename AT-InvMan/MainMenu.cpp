//Jhon Aries
#include <iostream>
#include "employee.h"
using namespace std;


void mainMenu() {
	char choice;
	cout << "\n=======================================================================\n";
	cout << "     Welcome to Aotearoa Treasures’ inventory management system !\n";
	cout << "=======================================================================\n";
	cout << "\n=======================================================================\n";
	cout << "                           Main Menu\n";
	cout << "=======================================================================\n";
	cout << endl;
	do {
		cout << "1. Inventory Management" << endl;
		cout << "2. Roster Management" << endl;
		cout << "3. Employee List" << endl;
		cout << "4. Sales Report" << endl;
		cout << "5. Exit" << endl;
		cout << endl;
		cout << endl;
		cout << "Please select an option from the menu above:";
		cin >> choice;

		switch (choice) {
		case '1':
			//inventoryManagement();
			break;
		case '2':
			roster();
			break;
		case '3':
			employeeList();
			break;
		case '4':
			//salesReport();
			break;
		case '5':
			cout << "Exiting the system. Goodbye!" << endl;
			exit(0);
		default:
			cout << "Invalid choice. Please try again." << endl;
			mainMenu(); // Call mainMenu again for a valid choice
			break;
		}
	} while (choice > 5 || choice < 1);
}