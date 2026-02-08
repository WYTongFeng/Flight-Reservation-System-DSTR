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

    void processWaitlist() override {
        cout << ">> [System] Processing Waitlist for empty seats..." << endl;
        
        if (waitlistHead == nullptr) {
            cout << ">> [System] Waitlist is empty. No action needed." << endl;
            return;
        }

        WaitlistNode* prev = nullptr;
        WaitlistNode* curr = waitlistHead;

        while (curr != nullptr) {
            bool assigned = false;
            int startRow, endRow;

            // 1. Determine Class Zone
            if (curr->flightClass == "First") { startRow = 1; endRow = 3; }
            else if (curr->flightClass == "Business") { startRow = 4; endRow = 10; }
            else { startRow = 11; endRow = maxRows; } // Economy

            // 2. Find an Empty Seat in that Zone
            for (int r = startRow; r <= endRow; r++) {
                for (int c = 0; c < FlightGlobal::COLS; c++) {
                    
                    // Check if seat is EMPTY in the 2D Map
                    if (seatMap[r-1][c] == "EMPTY") {
                        
                        string foundCol = FlightGlobal::getColName(c);
                        cout << ">> [Auto-Assign] Moved " << curr->name << " from Waitlist to Seat " << r << foundCol << "." << endl;

                        // Add to Main System
                        addPassenger(curr->id, curr->name, r, foundCol, curr->flightClass);
                        assigned = true;
                        goto FOUND_SEAT; // Break out of double loop
                    }
                }
            }

            FOUND_SEAT:
            
            // 3. Remove from Waitlist if assigned
            if (assigned) {
                WaitlistNode* toDelete = curr;
                
                if (prev == nullptr) {
                    waitlistHead = curr->next;
                    if (waitlistHead == nullptr) waitlistTail = nullptr;
                    curr = waitlistHead; // Move to new head
                } else {
                    prev->next = curr->next;
                    if (prev->next == nullptr) waitlistTail = prev;
                    curr = curr->next; // Move to next
                }
                delete toDelete;
            } else {
                // No seat found, move to next person
                prev = curr;
                curr = curr->next;
            }
        }
        cout << ">> [System] Waitlist processing complete." << endl;
    }

    // ==========================================
    // CONSTRUCTOR & DESTRUCTOR
    // ==========================================
    ArraySystem() {
        currentCount = 0;
        maxRows = 30; // FIXED SIZE: A real plane size
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
    
    // --- NEW VALIDATION ---
    if (!validateSeatClass(row, fclass)) {
        cout << ">> [Error] Class Mismatch! " << fclass << " passengers cannot sit in Row " << row << "." << endl;
        
        // Optional: Suggest the correct rows
        if (fclass == "First") cout << ">> [Hint] First Class is Rows 1-3." << endl;
        else if (fclass == "Business") cout << ">> [Hint] Business Class is Rows 4-10." << endl;
        else cout << ">> [Hint] Economy Class is Rows 11+." << endl;

        return false;
    }
    
    // 1. Expand Array if Row exceeds current limit
        
    // Instead, just double check validity (though main.cpp handles this)
        if (row > maxRows) {
            cout << ">> [Error] This plane only has " << maxRows << " rows." << endl;
            return false;
        }

        int rIndex = row - 1;
        int cIndex = FlightGlobal::getColIndex(col);

        if (rIndex < 0 || cIndex == -1) {
            cout << ">> [Error] Invalid Seat Position." << endl;
            return false;
        }

        // SEAT COLLISION -> WAITLIST
        if (seatMap[rIndex][cIndex] != "EMPTY") {
            cout << ">> [System] Seat " << row << col << " is occupied. Adding to Waitlist..." << endl;
            addToWaitlist(id, name, row, col, fclass); // AUTOMATICALLY GO TO WAITLIST
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
    // FUNCTION: Remove Passenger (ARRAY VERSION)
    // ==========================================
    bool removePassenger(string id) override {
        int targetIndex = -1;
        
        // 1. Find Passenger
        for (int i = 0; i < currentCount; i++) {
            if (passengerList[i] != nullptr && passengerList[i]->passengerID == id) {
                targetIndex = i;
                break;
            }
        }

        if (targetIndex == -1) return false;

        // 2. Capture Data (To know which seat opened up)
        Passenger* p = passengerList[targetIndex];
        int freedRow = p->seatRow;
        string freedCol = p->seatCol;

        // 3. Clear from Seat Map
        int rIndex = freedRow - 1;
        int cIndex = FlightGlobal::getColIndex(freedCol);
        if (rIndex >= 0 && cIndex != -1) {
            seatMap[rIndex][cIndex] = "EMPTY";
        }

        // 4. Remove from Array (Shift elements)
        delete passengerList[targetIndex]; 
        for (int i = targetIndex; i < currentCount - 1; i++) {
            passengerList[i] = passengerList[i + 1];
        }
        passengerList[currentCount - 1] = nullptr; 
        currentCount--;

        cout << ">> [System] Passenger " << id << " removed. Seat " << freedRow << freedCol << " is now free." << endl;

        // ==========================================
        // AUTO-FILL FROM WAITLIST (Corrected Names)
        // ==========================================
        WaitlistNode* prev = nullptr;
        WaitlistNode* curr = waitlistHead;

        while (curr != nullptr) {
            // ERROR FIX: Use 'row' and 'col' to match your FlightCommon.hpp
            if (curr->row == freedRow && curr->col == freedCol) {
                
                cout << ">> [Waitlist] Found match! Promoting " << curr->name << " to Seat " << freedRow << freedCol << "." << endl;

                // Add to main flight (using the row/col we just freed)
                addPassenger(curr->id, curr->name, freedRow, freedCol, curr->flightClass);

                // Remove from Waitlist
                if (prev == nullptr) {
                    waitlistHead = curr->next;
                    if (waitlistHead == nullptr) waitlistTail = nullptr;
                } else {
                    prev->next = curr->next;
                    if (prev->next == nullptr) waitlistTail = prev;
                }
                
                delete curr; // Free memory
                break; // Stop after promoting one person
            }
            
            prev = curr;
            curr = curr->next;
        }

        return true;
    }

    // ==========================================
    // FUNCTION 3: Search Passenger (Updated for Waitlist)
    // ==========================================
    Passenger* searchPassenger(const string& id) override {
        // 1. Search Main List (Array Linear Search)
        for (int i = 0; i < currentCount; i++) {
            if (passengerList[i] != nullptr && passengerList[i]->passengerID == id) {
                return passengerList[i]; // Found in the main plane!
            }
        }

        // 2. Search Waitlist (Singly Linked List)
        // If we didn't find them in the array, check the waitlist
        WaitlistNode* wTemp = waitlistHead;
        while (wTemp != nullptr) {
            if (wTemp->id == id) {
                // Found in Waitlist!
                // Use the same "Static Object Trick" to return a Passenger* pointer
                static Passenger tempResult; 
                
                tempResult.passengerID = wTemp->id;
                tempResult.name = wTemp->name + " [WAITLIST]";
                tempResult.seatRow = 0;
                tempResult.seatCol = "WL";
                tempResult.flightClass = wTemp->flightClass;
                
                return &tempResult;
            }
            wTemp = wTemp->next;
        }

        return nullptr; // Not found in Array OR Waitlist
    }

    // ==========================================
    // FUNCTION 4: Display Seat Map
    // ==========================================
void displaySeatingMap() override {
    // 1. Calculate Last Active Row
    int maxRowDisplayed = 60; // Default to showing 60 rows
    /* Optional: Loop to find actual last row if you want to optimize
    for(int i = maxRows; i >= 1; i--) { 
        // check if row is empty... 
    } */

    int totalPages = (maxRowDisplayed + FlightGlobal::ROWS_PER_PAGE - 1) / FlightGlobal::ROWS_PER_PAGE;
    int currentPage = 1;

    while (true) {
        cout << string(50, '\n'); 
        cout << "==========================================================================================" << endl;
        cout << "                          FLIGHT SEATING MAP (ARRAY SYSTEM)                               " << endl;
        cout << "==========================================================================================" << endl;
        
        // --- DYNAMIC HEADER ---
        cout << "      "; 
        for (int c = 0; c < FlightGlobal::COLS; c++) {
            cout << " [" << FlightGlobal::getColName(c) << "]            "; 
            if (c == 2) cout << "    "; 
        }
        cout << endl;
        cout << "------------------------------------------------------------------------------------------" << endl;

        int startRow = (currentPage - 1) * FlightGlobal::ROWS_PER_PAGE;
        int endRow = startRow + FlightGlobal::ROWS_PER_PAGE;
        if (endRow > maxRowDisplayed) endRow = maxRowDisplayed;

        // --- RENDER ROWS ---
        for (int r = startRow; r < endRow; r++) {
            int actualRow = r + 1;
            
            string rowClass = "Eco"; 
            if (actualRow <= 3) rowClass = "Fst";      
            else if (actualRow <= 10) rowClass = "Bus";
            
            // Print Row Number (Right Aligned)
            cout << rowClass << right << setw(2) << setfill('0') << actualRow << setfill(' ') << " ";

            // Print Columns
            for (int c = 0; c < FlightGlobal::COLS; c++) {
                
                // [FIX] Declare 'display' EXACTLY ONCE here
                string display = "EMPTY";
                
                // Array Logic: Direct Access
                if ((r < maxRows) && (seatMap[r][c] != "EMPTY")) {
                    display = seatMap[r][c];
                }

                // Formatting
                if (display == "EMPTY" || display == "") display = "---";
                if (display.length() > 12) display = display.substr(0, 10) + "..";

                cout << "[" << left << setw(12) << display << "] "; 
                if (c == 2) cout << "    "; 
            }
            cout << endl;
        }

        cout << "------------------------------------------------------------------------------------------" << endl;
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
    void addToWaitlist(string id, string name, int row, string col, string fclass) override {
    WaitlistNode* newNode = new WaitlistNode;
    newNode->id = id;
    newNode->name = name;
    newNode->row = row;    // Store the row
    newNode->col = col;    // Store the col
    newNode->flightClass = fclass;
    newNode->next = nullptr;

    if (waitlistHead == nullptr) {
        waitlistHead = newNode;
        waitlistTail = newNode;
    } else {
        waitlistTail->next = newNode;
        waitlistTail = newNode;
    }
    cout << ">> [Waitlist] " << name << " added for seat " << row << col << "." << endl;
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