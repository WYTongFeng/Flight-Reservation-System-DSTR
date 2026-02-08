#include "FlightCommon.hpp"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

class ArraySystem : public FlightSystem {
private:
    string** seatMap; 
    int maxRows; 
    Passenger** passengerList; 
    int passengerCapacity; 
    int currentCount;      

    // --- NEW: Waitlist Variables (Singly Linked List) ---
    WaitlistNode* waitlistHead;
    WaitlistNode* waitlistTail;

    // Helper: Expand Seat Map (2D Array)
    void expandSeatMap(int requiredRow) {
        if (requiredRow <= maxRows) return;
        
        int newMax = (requiredRow > maxRows * 2) ? requiredRow + 10 : maxRows * 2;

        cout << ">> [System] Expanding Seat Map from " << maxRows << " to " << newMax << " rows..." << endl;

        string** newMap = new string*[newMax];
        for (int i = 0; i < newMax; i++) {
            newMap[i] = new string[FlightGlobal::COLS]; 
            for (int j = 0; j < FlightGlobal::COLS; j++) newMap[i][j] = "EMPTY"; 
        }

        // Copy old data
        for (int i = 0; i < maxRows; i++) {
            for (int j = 0; j < FlightGlobal::COLS; j++) {
                newMap[i][j] = seatMap[i][j];
            }
            delete[] seatMap[i]; // Delete old row
        }
        delete[] seatMap; // Delete old pointer array

        seatMap = newMap;
        maxRows = newMax;
    }

    // Helper: Expand Passenger List (1D Array)
    void expandPassengerList() {
        int newCap = passengerCapacity * 2;
        Passenger** newList = new Passenger*[newCap];
        
        for (int i = 0; i < currentCount; i++) newList[i] = passengerList[i];
        for (int i = currentCount; i < newCap; i++) newList[i] = nullptr;
        
        delete[] passengerList;
        passengerList = newList;
        passengerCapacity = newCap;
    }

public:
    ArraySystem() {
        currentCount = 0;
        maxRows = 50; 
        passengerCapacity = 200; 

        seatMap = new string*[maxRows];
        for (int i = 0; i < maxRows; i++) {
            seatMap[i] = new string[FlightGlobal::COLS]; 
            for (int j = 0; j < FlightGlobal::COLS; j++) seatMap[i][j] = "EMPTY"; 
        }

        passengerList = new Passenger*[passengerCapacity];
        for(int i=0; i<passengerCapacity; i++) passengerList[i] = nullptr;

        // Init Waitlist
        waitlistHead = nullptr;
        waitlistTail = nullptr;

        cout << ">> Array System Initialized." << endl;
    }

    ~ArraySystem() {
        if (seatMap) {
            for (int i = 0; i < maxRows; i++) delete[] seatMap[i];
            delete[] seatMap;
        }
        if (passengerList) {
            for (int i = 0; i < currentCount; i++) delete passengerList[i];
            delete[] passengerList;
        }

        // Delete Waitlist (Singly Linked List)
        WaitlistNode* temp = waitlistHead;
        while (temp != nullptr) {
            WaitlistNode* next = temp->next;
            delete temp;
            temp = next;
        }
    }

    // [Function 1] Reservation
    void addPassenger(string id, string name, int row, string col, string fclass) override {
    // 1. Check Row Validity & Expand if needed
        if (row > maxRows) expandSeatMap(row);
        
        int rIndex = row - 1;
        int cIndex = FlightGlobal::getColIndex(col);

        if (rIndex < 0 || cIndex == -1) {
            cout << ">> [Error] Invalid Seat Position: " << row << col << endl;
            return;
        }

        // 2. SEAT COLLISION CHECK (O(1))
        if (seatMap[rIndex][cIndex] != "EMPTY") {
            cout << ">> [Failed] Seat " << row << col << " is already occupied by " << seatMap[rIndex][cIndex] << "." << endl;
            return;
        }

        // 3. ID UNIQUENESS CHECK (New Feature! O(N))
        // We must loop through the existing list to ensure this ID doesn't exist.
        for (int i = 0; i < currentCount; i++) {
            if (passengerList[i]->passengerID == id) {
                cout << ">> [Failed] Passenger ID " << id << " already exists (Holder: " << passengerList[i]->name << ")." << endl;
                return;
            }
        }

        // 4. Check List Capacity
        if (currentCount >= passengerCapacity) expandPassengerList();

        // 5. Create and Add Passenger
        Passenger* newP = new Passenger;
        newP->passengerID = id;
        newP->name = name;
        newP->seatRow = row;
        newP->seatCol = col;
        newP->flightClass = fclass;
        
        passengerList[currentCount++] = newP; // Add to 1D List
        seatMap[rIndex][cIndex] = name;       // Add to 2D Map

        cout << ">> [Success] Passenger " << name << " (" << id << ") assigned to " << row << col << "." << endl;
    }

    // [Function 2] Remove Passenger
    bool removePassenger(string id) override {
        int index = -1;
        
        // 1. Find Passenger
        for (int i = 0; i < currentCount; i++) {
            if (passengerList[i] != nullptr && passengerList[i]->passengerID == id) {
                index = i;
                break;
            }
        }

        if (index == -1) {
            // cout << "[Array] Remove failed: ID " << id << " not found." << endl; 
            return false;
        }

        // 2. Clear from Seat Map
        Passenger* p = passengerList[index];
        int rIndex = p->seatRow - 1;
        int cIndex = FlightGlobal::getColIndex(p->seatCol);
        
        if (rIndex >= 0 && rIndex < maxRows && cIndex != -1) {
            seatMap[rIndex][cIndex] = "EMPTY"; 
        }

        // 3. Delete Object
        delete passengerList[index]; 

        // 4. Shift Array (to fill the gap)
        for (int i = index; i < currentCount - 1; i++) {
            passengerList[i] = passengerList[i + 1]; 
        }
        passengerList[currentCount - 1] = nullptr; 
        currentCount--;

        return true;
    }

    // [Function 3] Search
    Passenger* searchPassenger(const string& id) override {
        for (int i = 0; i < currentCount; i++) {
            if (passengerList[i] != nullptr && passengerList[i]->passengerID == id) {
                return passengerList[i]; 
            }
        }
        return nullptr;
    }

    // [Function 4] Seat Map Display
    void displaySeatingMap() override {
        // Calculate the last used row to avoid printing 100 empty rows
        int lastActiveRow = 20; // Default minimum
        for(int i = maxRows - 1; i >= 0; i--) {
            bool rowEmpty = true;
            for(int j = 0; j < FlightGlobal::COLS; j++) {
                if(seatMap[i][j] != "EMPTY") { rowEmpty = false; break; }
            }
            if(!rowEmpty) { 
                lastActiveRow = i + 1; 
                break; 
            }
        }

        int totalPages = (lastActiveRow + FlightGlobal::ROWS_PER_PAGE - 1) / FlightGlobal::ROWS_PER_PAGE;
        if (totalPages < 1) totalPages = 1;
        int currentPage = 1;

        while (true) {
            // Use standard newlines to clear screen (portable)
            cout << string(50, '\n'); 

            cout << "==========================================================================" << endl;
            cout << "                       FLIGHT SEATING MAP (ARRAY)                         " << endl;
            cout << "==========================================================================" << endl;
            cout << "      " 
                 << left << setw(FlightGlobal::COL_WIDTH) << "[A]" 
                 << left << setw(FlightGlobal::COL_WIDTH) << "[B]" 
                 << left << setw(FlightGlobal::COL_WIDTH) << "[C]" 
                 << "    " 
                 << left << setw(FlightGlobal::COL_WIDTH) << "[D]" 
                 << left << setw(FlightGlobal::COL_WIDTH) << "[E]" 
                 << left << setw(FlightGlobal::COL_WIDTH) << "[F]" << endl;
            cout << "--------------------------------------------------------------------------" << endl;

            int startRow = (currentPage - 1) * FlightGlobal::ROWS_PER_PAGE;
            int endRow = startRow + FlightGlobal::ROWS_PER_PAGE; 
            if (endRow > lastActiveRow) endRow = lastActiveRow;

            for (int i = startRow; i < endRow; i++) {
                // Determine Class Label
                string rowClass = "Eco"; 
                if (i < 3) rowClass = "Fst";      
                else if (i < 10) rowClass = "Bus"; 
                
                cout << rowClass << setw(2) << setfill('0') << (i + 1) << setfill(' ') << " "; 

                for (int j = 0; j < FlightGlobal::COLS; j++) {
                    string display = FlightGlobal::formatName(seatMap[i][j]); 
                    // Truncate if too long
                    if (display.length() > 12) display = display.substr(0, 9) + "..";
                    
                    cout << "[" << left << setw(10) << display << "] ";
                    if (j == 2) cout << "    "; 
                }
                cout << endl;
            }
            cout << "--------------------------------------------------------------------------" << endl;
            cout << "PAGE " << currentPage << "/" << totalPages << " | [N]ext  [P]rev  [0] Exit: ";
            
            string input;
            cin >> input;

            if (input == "0") break;
            if ((input == "n" || input == "N") && currentPage < totalPages) currentPage++;
            if ((input == "p" || input == "P") && currentPage > 1) currentPage--;
        }
    }

    // [Manifest]
    void displayManifest() override {
        if (currentCount == 0) {
            cout << ">> [Manifest] No passengers found." << endl;
            return;
        }

        // Simple full list print for stability (Pagination can be added back if needed)
        cout << "\n==============================================================" << endl;
        cout << "             PASSENGER MANIFEST (" << currentCount << " Passengers)" << endl;
        cout << "==============================================================" << endl;
        cout << left << setw(10) << "ID" 
             << left << setw(20) << "Name" 
             << left << setw(10) << "Seat" 
             << left << setw(15) << "Class" << endl;
        cout << "--------------------------------------------------------------" << endl;

        for (int i = 0; i < currentCount; i++) {
            if (passengerList[i]) {
                string fullSeat = to_string(passengerList[i]->seatRow) + passengerList[i]->seatCol;
                cout << left << setw(10) << passengerList[i]->passengerID 
                     << left << setw(20) << passengerList[i]->name 
                     << left << setw(10) << fullSeat
                     << left << setw(15) << passengerList[i]->flightClass << endl;
            }
        }
        cout << "==============================================================" << endl;
        cout << "Press Any Key + Enter to continue...";
        string dummy; cin >> dummy;
    }

    // [Sorting] Bubble Sort
    void sortAlphabetically() override {
        if (currentCount < 2) {
            cout << ">> Not enough passengers to sort." << endl;
            return;
        }

        for (int i = 0; i < currentCount - 1; i++) {
            for (int j = 0; j < currentCount - i - 1; j++) {
                if (passengerList[j]->name > passengerList[j + 1]->name) {
                    Passenger* temp = passengerList[j];
                    passengerList[j] = passengerList[j + 1];
                    passengerList[j + 1] = temp;
                }
            }
        }
        cout << ">> Sorted " << currentCount << " passengers by Name." << endl;
        displayManifest();
    }

    // --- NEW: Waitlist Implementation ---
    void addToWaitlist(string id, string name, string fclass) override {
        WaitlistNode* newNode = new WaitlistNode;
        newNode->id = id;
        newNode->name = name;
        newNode->flightClass = fclass;
        newNode->next = nullptr;

        // Add to Tail (Singly Linked List)
        if (waitlistHead == nullptr) {
            waitlistHead = newNode;
            waitlistTail = newNode;
        } else {
            waitlistTail->next = newNode;
            waitlistTail = newNode;
        }
        cout << ">> [Waitlist] " << name << " added to priority queue (Singly Linked List)." << endl;
    }
};