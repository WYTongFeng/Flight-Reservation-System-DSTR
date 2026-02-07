#include <iostream>
#include <string>
#include <limits> 
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm> 
#include <cctype>   

#include "FlightCommon.hpp"
#include "Timer.hpp"

// Include your system implementations
#include "ArraySystem.cpp"
#include "LinkedListSystem.cpp" 

using namespace std;

// --- Helpers ---
string toLowerStr(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return (char)tolower(c); });
    return s;
}

// Fixed: Allow rows up to 100 to support array expansion
int readRow() {
    int row;
    while (true) {
        cout << "Enter Row (1-100): ";
        if (cin >> row && row >= 1 && row <= 100) return row;
        cout << ">> [Error] Please enter a valid row number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string readCol_AtoF() {
    string col;
    while (true) {
        cout << "Enter Col (A-F): ";
        cin >> col;
        if (!col.empty()) {
            char c = (char)toupper((unsigned char)col[0]);
            if (c >= 'A' && c <= 'F') return string(1, c);
        }
        cout << ">> [Error] Column must be A, B, C, D, E, or F.\n";
    }
}

string readClass_FBE() {
    string fclass;
    while (true) {
        cout << "Enter Class (First/Business/Economy): ";
        cin >> fclass;
        string x = toLowerStr(fclass);
        if (x == "first") return "First";
        if (x == "business" || x == "busin") return "Business";
        if (x.find("eco") == 0) return "Economy";
        cout << ">> [Error] Class must be First, Business, or Economy.\n";
    }
}

// --- CSV Loader ---
void loadData(FlightSystem* sys, string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return;
    }
    string line;
    getline(file, line); // Skip Header

    int count = 0;
    while (getline(file, line)) {
        stringstream ss(line);
        string id, name, rowStr, col, fclass;
        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, rowStr, ',');
        getline(ss, col, ',');
        getline(ss, fclass, ',');

        if (!col.empty() && col.back() == '\r') col.pop_back();
        if (!fclass.empty() && fclass.back() == '\r') fclass.pop_back();

        if (!id.empty() && !rowStr.empty()) {
            try {
                sys->addPassenger(id, name, stoi(rowStr), col, fclass);
                count++;
            } catch (...) {}
        }
    }
    cout << ">> Loaded " << count << " passengers into " << filename << endl;
    file.close();
}

void showSubMenu(string systemName) {
    cout << "\n--- " << systemName << " Operations ---" << endl;
    cout << "1. Add Passenger" << endl;
    cout << "2. Remove Passenger" << endl;
    cout << "3. Search Passenger" << endl;
    cout << "4. Display Seat Map" << endl;
    cout << "5. Display Manifest (Sorted)" << endl;
    cout << "0. Back to Main Menu" << endl;
    cout << "Select Operation: ";
}

void runSystem(FlightSystem* sys, string name) {
    int choice;
    string id, pname, seatCol, fclass;
    int row;

    do {
        showSubMenu(name);
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: { 
                // FIXED: Now asks for ID so you can remove them later
                cout << "Enter ID (e.g., P9999): ";
                cin >> id;
                
                cout << "Enter Name: ";
                cin.ignore(); 
                getline(cin, pname);

                row = readRow();
                seatCol = readCol_AtoF();
                fclass = readClass_FBE();

                sys->addPassenger(id, pname, row, seatCol, fclass);
                break;
            }
            case 2:
                cout << "Enter Passenger ID to remove: ";
                cin >> id;
                if (sys->removePassenger(id)) cout << ">> Removed successfully.\n";
                else cout << ">> Passenger NOT found.\n";
                break;
            case 3:
                cout << "Enter Passenger ID to search: ";
                cin >> id;
                {
                    Passenger* p = sys->searchPassenger(id);
                    if (p) cout << ">> Found: " << p->name << " at " << p->seatRow << p->seatCol << endl;
                    else cout << ">> Not found.\n";
                }
                break;
            case 4:
                sys->displaySeatingMap();
                break;
            case 5:
                sys->sortAlphabetically();
                break;
            case 0:
                break;
            default:
                cout << "Invalid option!" << endl;
        }
    } while (choice != 0);
}

int main() {
    FlightSystem* arraySys = new ArraySystem();
    FlightSystem* listSys = new LinkedListSystem();

    string filename = "flight_passenger_data.csv.csv";
    
    // Load Data
    cout << ">> Initializing Array System..." << endl;
    loadData(arraySys, filename);

    cout << ">> Initializing Linked List System..." << endl;
    loadData(listSys, filename);

    int mainChoice;
    while (true) {
        cout << "\n=== FLIGHT RESERVATION SYSTEM ===" << endl;
        cout << "1. ARRAY Based System" << endl;
        cout << "2. LINKED LIST Based System" << endl;
        cout << "3. Exit" << endl;
        cout << "Select: ";

        if (!(cin >> mainChoice)) {
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (mainChoice == 1) runSystem(arraySys, "ARRAY SYSTEM");
        else if (mainChoice == 2) runSystem(listSys, "LINKED LIST SYSTEM");
        else if (mainChoice == 3) break;
    }

    delete arraySys;
    delete listSys;
    return 0;
}