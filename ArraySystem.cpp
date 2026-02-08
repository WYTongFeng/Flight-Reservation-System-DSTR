#include "FlightCommon.hpp"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

// ==========================================
// CLASS: ArraySystem
// Description: Implements the Flight System using Dynamic Arrays.
// Key Features:
// 1. 2D Array for visual Seat Map (Fast Access O(1)).
// 2. 1D Array for Passenger Manifest (Linear Search).
// 3. Singly Linked List for Waitlist (Requirement).
// ==========================================
class ArraySystem : public FlightSystem {
private:
    string** seatMap;            // 2D Dynamic Array for Seating Grid [Rows][Cols]
    int maxRows;                 // Current maximum rows (Expandable)
    Passenger** passengerList;   // 1D Dynamic Array for storing Passenger Objects
    int passengerCapacity;       // Current capacity of the list
    int currentCount;            // Current number of passengers

    // --- Waitlist Variables (Singly Linked List) ---
    // Stores passengers who are waiting for a seat when the flight is full.
    WaitlistNode* waitlistHead;
    WaitlistNode* waitlistTail;

    // ==========================================
    // HELPER: Dynamic Array Expansion
    // ==========================================
    
    // Function: Expand Seat Map (2D Array)
    // Logic: Creates a larger array, copies old data, and deletes the old array.
    void expandSeatMap(int requiredRow) {
        if (requiredRow <= maxRows) return;
        
        // Double the size or add buffer
        int newMax = (requiredRow > maxRows * 2) ? requiredRow + 10 : maxRows * 2;

        cout << ">> [System] Expanding Seat Map from " << maxRows << " to " << newMax << " rows..." << endl;

        // 1. Allocate new 2D Array
        string** newMap = new string*[newMax];
        for (int i = 0; i < newMax; i++) {
            newMap[i] = new string[FlightGlobal::COLS]; 
            for (int j = 0; j < FlightGlobal::COLS; j++) newMap[i][j] = "EMPTY"; 
        }

        // 2. Copy data from old map
        for (int i = 0; i < maxRows; i++) {
            for (int j = 0; j < FlightGlobal::COLS; j++) {
                newMap[i][j] = seatMap[i][j];
            }
            delete[] seatMap[i]; // Delete old row memory
        }
        delete[] seatMap; // Delete old pointer array

        // 3. Point to new map
        seatMap = newMap;
        maxRows = newMax;
    }

    // Function: Expand Passenger List (1D Array)
    // Logic: Standard vector-like resizing (Doubling capacity).
    void expandPassengerList() {
        int newCap = passengerCapacity * 2;
        Passenger** newList = new Passenger*[newCap];
        
        // Copy pointers
        for (int i = 0; i < currentCount; i++) newList[i] = passengerList[i];
        // Initialize rest to null
        for (int i = currentCount; i < newCap; i++) newList[i] = nullptr;
        
        delete[] passengerList;
        passengerList = newList;
        passengerCapacity = newCap;
    }

    // ==========================================
    // HELPER: Merge Sort Implementation
    // ==========================================
    
    // Sub-function: Merges two sorted subarrays into one.
    void merge(Passenger** arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;

        // Create temporary arrays
        Passenger** L = new Passenger*[n1];
        Passenger** R = new Passenger*[n2];

        // Copy data to temp arrays
        for (int i = 0; i < n1; i++) L[i] = arr[left + i];
        for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

        // Merge back into main array
        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            // Compare by ID (String comparison)
            if (L[i]->passengerID <= R[j]->passengerID) {
                arr[k] = L[i];
                i++;
            } else {
                arr[k] = R[j];
                j++;
            }
            k++;
        }

        // Copy remaining elements
        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];

        // Clean up memory
        delete[] L;
        delete[] R;
    }

    // Recursive sorting function
    void mergeSortRecursive(Passenger** arr, int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSortRecursive(arr, left, mid);
        mergeSortRecursive(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }

public:
    // ==========================================
    // CONSTRUCTOR & DESTRUCTOR
    // ==========================================
    ArraySystem() {
        currentCount = 0;
        maxRows = 50; 
        passengerCapacity = 200; 

        // Initialize 2D Seat Map
        seatMap = new string*[maxRows];
        for (int i = 0; i < maxRows; i++) {
            seatMap[i] = new string[FlightGlobal::COLS]; 
            for (int j = 0; j < FlightGlobal::COLS; j++) seatMap[i][j] = "EMPTY"; 
        }

        // Initialize 1D Passenger List
        passengerList = new Passenger*[passengerCapacity];
        for(int i=0; i<passengerCapacity; i++) passengerList[i] = nullptr;

        // Initialize Waitlist (Singly Linked List)
        waitlistHead = nullptr;
        waitlistTail = nullptr;

        cout << ">> Array System Initialized." << endl;
    }

    ~ArraySystem() {
        // Free 2D Array Memory
        if (seatMap) {
            for (int i = 0; i < maxRows; i++) delete[] seatMap[i];
            delete[] seatMap;
        }
        // Free 1D Array Memory
        if (passengerList) {
            for (int i = 0; i < currentCount; i++) delete passengerList[i];
            delete[] passengerList;
        }

        // Free Waitlist Memory (Traverse Singly Linked List)
        WaitlistNode* temp = waitlistHead;
        while (temp != nullptr) {
            WaitlistNode* next = temp->next;
            delete temp;
            temp = next;
        }
    }

    // ==========================================
    // FUNCTION 1: Add Passenger (Reservation)
    // ==========================================
    bool addPassenger(string id, string name, int row, string col, string fclass) override {
        // 1. Expand Array if Row exceeds current limit
        if (row > maxRows) expandSeatMap(row);
        
        int rIndex = row - 1;
        int cIndex = FlightGlobal::getColIndex(col);

        if (rIndex < 0 || cIndex == -1) {
            cout << ">> [Error] Invalid Seat Position: " << row << col << endl;
            return false;
        }

        // 2. SEAT COLLISION CHECK
        // Advantage of Array: Access is O(1) (Instant)
        if (seatMap[rIndex][cIndex] != "EMPTY") {
            cout << ">> [Failed] Seat " << row << col << " is already occupied by " << seatMap[rIndex][cIndex] << "." << endl;
            return false;
        }

        // 3. ID UNIQUENESS CHECK
        // Disadvantage of Array: Must loop through entire list O(N)
        for (int i = 0; i < currentCount; i++) {
            if (passengerList[i]->passengerID == id) {
                cout << ">> [Failed] Passenger ID " << id << " already exists (Holder: " << passengerList[i]->name << ")." << endl;
                return false;
            }
        }

        // 4. Resize List if Full
        if (currentCount >= passengerCapacity) expandPassengerList();

        // 5. Create Object and Add to Arrays
        Passenger* newP = new Passenger;
        newP->passengerID = id;
        newP->name = name;
        newP->seatRow = row;
        newP->seatCol = col;
        newP->flightClass = fclass;
        
        passengerList[currentCount++] = newP; // Store in 1D List
        seatMap[rIndex][cIndex] = name;       // Store in 2D Map (Visuals)

        cout << ">> [Success] Passenger " << name << " (" << id << ") assigned to " << row << col << "." << endl;
        return true;
    }

    // ==========================================
    // FUNCTION 2: Remove Passenger
    // ==========================================
    bool removePassenger(string id) override {
        int index = -1;
        
        // 1. Linear Search to find passenger Index (O(N))
        for (int i = 0; i < currentCount; i++) {
            if (passengerList[i] != nullptr && passengerList[i]->passengerID == id) {
                index = i;
                break;
            }
        }

        if (index == -1) {
            return false; // Not found
        }

        // 2. Clear from Seat Map (O(1))
        Passenger* p = passengerList[index];
        int rIndex = p->seatRow - 1;
        int cIndex = FlightGlobal::getColIndex(p->seatCol);
        
        if (rIndex >= 0 && rIndex < maxRows && cIndex != -1) {
            seatMap[rIndex][cIndex] = "EMPTY"; 
        }

        // 3. Delete Object from Memory
        delete passengerList[index]; 

        // 4. Shift Array Elements Left (O(N))
        // This is necessary to close the "gap" in the array.
        for (int i = index; i < currentCount - 1; i++) {
            passengerList[i] = passengerList[i + 1]; 
        }
        passengerList[currentCount - 1] = nullptr; 
        currentCount--;

        return true;
    }

    // ==========================================
    // FUNCTION 3: Search Passenger
    // ==========================================
    Passenger* searchPassenger(const string& id) override {
        // Standard Linear Search O(N)
        for (int i = 0; i < currentCount; i++) {
            if (passengerList[i] != nullptr && passengerList[i]->passengerID == id) {
                return passengerList[i]; 
            }
        }
        return nullptr;
    }

    // ==========================================
    // FUNCTION 4: Display Seat Map
    // ==========================================
    void displaySeatingMap() override {
        // Optimization: Calculate last used row to avoid printing 100+ empty rows
        int lastActiveRow = 20; // Minimum default
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

        // Pagination Logic
        int totalPages = (lastActiveRow + FlightGlobal::ROWS_PER_PAGE - 1) / FlightGlobal::ROWS_PER_PAGE;
        if (totalPages < 1) totalPages = 1;
        int currentPage = 1;

        while (true) {
            // Clear Screen
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

            // Render Rows
            for (int i = startRow; i < endRow; i++) {
                string rowClass = "Eco"; 
                if (i < 3) rowClass = "Fst";      
                else if (i < 10) rowClass = "Bus"; 
                
                cout << rowClass << setw(2) << setfill('0') << (i + 1) << setfill(' ') << " "; 

                for (int j = 0; j < FlightGlobal::COLS; j++) {
                    string display = FlightGlobal::formatName(seatMap[i][j]); 
                    // Truncate name if too long for the grid
                    if (display.length() > 12) display = display.substr(0, 9) + "..";
                    
                    cout << "[" << left << setw(10) << display << "] ";
                    if (j == 2) cout << "    "; // Aisle gap
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

// ==========================================
    // FUNCTION 5: Display Manifest (Fixed to show Waitlist)
    // ==========================================
    void displayManifest() override {
        // 1. Display Main Passenger List (Array)
        if (currentCount == 0) {
            cout << ">> [Manifest] Main list is empty." << endl;
        } else {
            cout << "\n==============================================================" << endl;
            cout << "             PASSENGER MANIFEST (ARRAY)" << endl;
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
        }

        // 2. Display Waitlist (Singly Linked List) - THIS WAS MISSING
        cout << "\n------------------ WAITLIST (Singly Linked List) ------------------" << endl;
        if (waitlistHead == nullptr) {
            cout << "(Empty)" << endl;
        } else {
            WaitlistNode* temp = waitlistHead;
            int count = 1;
            while (temp != nullptr) {
                cout << count++ << ". " << temp->name << " (" << temp->id << ") - " << temp->flightClass << endl;
                temp = temp->next;
            }
        }
        
        cout << "==============================================================" << endl;
        cout << "Press Any Key + Enter to continue...";
        string dummy; cin >> dummy;
    }

    // ==========================================
    // ALGORITHM 1: Bubble Sort (By Name)
    // Complexity: O(N^2)
    // ==========================================
    void sortAlphabetically() override {
        if (currentCount < 2) {
            cout << ">> Not enough passengers to sort." << endl;
            return;
        }

        // Standard Bubble Sort: Swaps adjacent elements if out of order
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

    // ==========================================
    // WAITLIST (Singly Linked List Implementation)
    // Requirement: Must demonstrate Singly Linked List
    // ==========================================
    void addToWaitlist(string id, string name, string fclass) override {
        WaitlistNode* newNode = new WaitlistNode;
        newNode->id = id;
        newNode->name = name;
        newNode->flightClass = fclass;
        newNode->next = nullptr;

        // Add to Tail of Singly Linked List
        if (waitlistHead == nullptr) {
            waitlistHead = newNode;
            waitlistTail = newNode;
        } else {
            waitlistTail->next = newNode;
            waitlistTail = newNode;
        }
        cout << ">> [Waitlist] " << name << " added to priority queue (Singly Linked List)." << endl;
    }

    // ==========================================
    // ALGORITHM 2: Merge Sort (By ID)
    // Complexity: O(N log N) - Efficient Sorting
    // ==========================================
    void sortByID() override {
        if (currentCount < 2) {
            cout << ">> Not enough passengers to sort." << endl;
            return;
        }
        cout << ">> [Array] Sorting by ID using MERGE SORT..." << endl;
        
        mergeSortRecursive(passengerList, 0, currentCount - 1);
        
        cout << ">> Sort Complete (Merge Sort)." << endl;
        displayManifest();
    }
};