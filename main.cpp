#include <iostream>
#include <string>
#include <limits> 
#include <fstream>
#include <sstream>
#include <cctype>   

#include "FlightCommon.hpp"
#include "Timer.hpp" // Custom Timer Class for Performance Testing

// Include System Implementations
#include "ArraySystem.cpp"
#include "LinkedListSystem.cpp" 

using namespace std;

// ==========================================
// HELPER FUNCTIONS (Input Validation)
// ==========================================

// Function: Convert string to lowercase
string toLowerStr(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return (char)tolower(c); });
    return s;
}

// Function: Safe Integer Input for Row (1-100)
int readRow() {
int row;
    while (true) {
        cout << "Enter Row (1-30): "; // <--- Change prompt text
        
        // <--- Change 100 to 30 below
        if (cin >> row && row >= 1 && row <= 30) return row; 
        
        cout << ">> [Error] Please enter a valid row number (1-30).\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// Function: Safe Column Input (A-F)
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

// Function: Safe Class Input (First, Business, Economy)
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

// ==========================================
// FILE I/O: CSV Loader
// ==========================================
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
        
        // Parse CSV Line
        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, rowStr, ',');
        getline(ss, col, ',');
        getline(ss, fclass, ',');

        // Clean cleanup for Windows/Mac line endings
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

// ==========================================
// MENU UI
// ==========================================
void showSubMenu(string systemName) {
    cout << "\n--- " << systemName << " Operations ---" << endl;
    cout << "1. Add Passenger (Measures O(1) vs O(N))" << endl;
    cout << "2. Remove Passenger (Measures Shift vs Unlink)" << endl;
    cout << "3. Search Passenger (Linear Search)" << endl;
    cout << "4. Display Seat Map (Visual Rendering Time)" << endl;
    cout << "5. Display Manifest (Bubble Sort by Name)" << endl;
    cout << "6. Display Manifest (Merge Sort by ID)" << endl;
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
            // --- OPERATION 1: ADD PASSENGER ---
            case 1: { 
                // 1. Collect Input FIRST (Do not time user typing speed)
                cout << "Enter ID (e.g., P9999): ";
                cin >> id;
                cout << "Enter Name: ";
                cin.ignore();
                getline(cin, pname);
                row = readRow();
                seatCol = readCol_AtoF();
                fclass = readClass_FBE();

                // 2. Start Timer
                Timer t;
                t.start();
                
                // 3. Run Algorithm
                bool success = sys->addPassenger(id, pname, row, seatCol, fclass);
                
                // 4. Stop Timer & Report
                t.stop();
                cout << ">> [Performance] Insert Time: " << t.getDurationInMicroseconds() << " microseconds" << endl;

                // 5. Handle Waitlist (Singly Linked List) if Full
                if (!success) {
                    char choice;
                    cout << ">> Seat/ID invalid or taken. Add to Waitlist? (y/n): ";
                    cin >> choice;
                    if (choice == 'y' || choice == 'Y') {
                        sys->addToWaitlist(id, pname, fclass);
                    }
                }
                break;
            }

            // --- OPERATION 2: REMOVE PASSENGER ---
            case 2:{
                cout << "Enter Passenger ID to remove: ";
                cin >> id;

                Timer t;
                t.start();
                bool success = sys->removePassenger(id);
                t.stop();

                if (success) cout << ">> Removed successfully.\n";
                else cout << ">> Passenger NOT found.\n";

                cout << ">> [Performance] Delete Time: " << t.getDurationInMicroseconds() << " microseconds" << endl;
                break;
            }

            // --- OPERATION 3: SEARCH PASSENGER ---
            case 3: 
                cout << "Enter Passenger ID to search: ";
                cin >> id;
                {
                    Timer t;
                    t.start();
                    Passenger* p = sys->searchPassenger(id);
                    t.stop();
                    
                    if (p) cout << ">> Found: " << p->name << endl;
                    else cout << ">> Not found.\n";

                    cout << ">> [Performance] Search Time: " << t.getDurationInMicroseconds() << " microseconds" << endl;
                }
                break;

            // --- OPERATION 4: DISPLAY SEAT MAP ---
            case 4: 
                {
                    // Note: This measures Rendering Time + Data Access Time
                    // Array will be significantly faster than Linked List here.
                    Timer t;
                    t.start();
                    sys->displaySeatingMap();
                    t.stop();

                    cout << ">> [Performance] Map Rendering Time: " << t.getDurationInMicroseconds() << " microseconds" << endl;
                    
                    cout << "(Press Enter to continue)";
                    cin.ignore(); cin.get(); 
                }
                break;

            // --- OPERATION 5: BUBBLE SORT (Name) ---
            case 5: 
                {
                    Timer t;
                    t.start();
                    sys->sortAlphabetically();
                    t.stop();
                    cout << ">> [Performance] Bubble Sort Time: " << t.getDurationInMilliseconds() << " ms." << endl;
                }
                break;

            // --- OPERATION 6: MERGE SORT (ID) ---
            case 6: 
                {
                    Timer t;
                    t.start();
                    sys->sortByID();
                    t.stop();
                    cout << ">> [Performance] Merge Sort Time: " << t.getDurationInMilliseconds() << " ms." << endl;
                }
                break;

            case 0:
                break;
            default:
                cout << "Invalid option!" << endl;
        }
    } while (choice != 0);
}

// ==========================================
// MAIN FUNCTION
// ==========================================
int main() {
    // 1. Create System Instances (Polymorphism)
    FlightSystem* arraySys = new ArraySystem();
    FlightSystem* listSys = new LinkedListSystem();

    // 2. Data Loading
    // NOTE: Filename is currently set to double extension (.csv.csv) based on user environment
    string filename = "flight_passenger_data.csv.csv";
    
    cout << ">> Initializing Array System..." << endl;
    loadData(arraySys, filename);

    cout << ">> Initializing Linked List System..." << endl;
    loadData(listSys, filename);

    // 3. Main Loop
    int mainChoice;
    while (true) {
        cout << "\n=== FLIGHT RESERVATION SYSTEM ===" << endl;
        cout << "1. ARRAY Based System (O(1) Access)" << endl;
        cout << "2. LINKED LIST Based System (Dynamic Memory)" << endl;
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

    // 4. Cleanup
    delete arraySys;
    delete listSys;
    return 0;
}