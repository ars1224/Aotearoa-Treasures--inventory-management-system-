#include <iostream>
#include <sqlite3.h>
#include <conio.h> 
#include <string>
#include "db-conn.h"
#include "main-functions.h"
using namespace std;

string getHiddenPassword() {
    string password;
    char ch;

    while ((ch = _getch()) != '\r') { // Enter key
        if (ch == '\b') { // Backspace
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        }
        else {
            password += ch;
            cout << '*';
        }
    }

    cout << endl;
    return password;
}

string getNumericID() {
    string id = "";
    char ch;
    cout << "Employee ID #: ";
    while ((ch = _getch()) != '\r') { // Enter key to finish
        if (ch >= '0' && ch <= '9') {
            id.push_back(ch);
            cout << ch;  // echo digit
        }
        else if (ch == '\b' && !id.empty()) { // backspace
            id.pop_back();
            cout << "\b \b"; // erase char on screen
        }
        // ignore other chars silently
    }
    cout << endl;
    return id;
}

int main() {
 int id;
    string pass;
    bool loggedIn = false;

    cout << R"(
 __      __       .__                                            
/  \    /  \ ____ |  |   ____  ____   _____   ____               
\   \/\/   // __ \|  | _/ ___\/  _ \ /     \_/ __ \              
 \        /\  ___/|  |_\  \__(  <_> )  Y Y  \  ___/              
  \__/\  /  \___  >____/\___  >____/|__|_|  /\___  >             
       \/       \/          \/            \/     \/              
   ________________      .___              _____                 
  /  _  \__    ___/      |   | _______  __/     \ _____    ____  
 /  /_\  \|    |  ______ |   |/    \  \/ /  \ /  \\__  \  /    \ 
/    |    \    | /_____/ |   |   |  \   /    Y    \/ __ \|   |  \
\____|__  /____|         |___|___|  /\_/\____|__  (____  /___|  /
        \/                        \/            \/     \/     \/ 
)" << endl;

    cout << "\n=======================================================================\n";
    cout << "                            AT-InvMan Login\n";
    cout << "=======================================================================\n\n";

    do {
        string idStr = getNumericID();

        // Convert to int safely
        try {
            id = stoi(idStr);
        }
        catch (...) {
            cout << "Invalid ID entered. Please enter numbers only.\n";
            continue; // Restart the loop if conversion fails
        }

        cout << "Password      : ";
        pass = getHiddenPassword();

        loggedIn = login(id, pass);

        if (!loggedIn) {
            cout << "Login failed! Please try again.\n";
        }

    } while (!loggedIn);

    // Proceed to MainMenu or next steps here... 

    
    return 0;
}
