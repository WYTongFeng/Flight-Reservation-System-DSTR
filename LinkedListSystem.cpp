#include "FlightCommon.hpp"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

// ==========================================
// CLASS: LinkedListSystem
// Description: Implements the Flight System using Linked Lists.
// Key Features:
// 1. Doubly Linked List for Main Passenger Records (Flexible Deletion).
// 2. Singly Linked List for Waitlist (Requirement).
// 3. Merge Sort for efficient sorting of Linked Lists (O(N log N)).
// ==========================================
class LinkedListSystem : public FlightSystem {
private:
    // --- Main List Variables (Doubly Linked List) ---
    Passenger* head;        // Pointer to the first passenger
    Passenger* tail;        // Pointer to the last passenger (for fast insertion)
    int currentCount;       // Total passengers

    // --- Waitlist Variables (Singly Linked List) ---
    // Stores passengers who are waiting for a seat when the flight is full.
    WaitlistNode* waitlistHead;
    WaitlistNode* waitlistTail;

    void displayManifest() override {
    // ==========================================
    // HELPER: Merge Sort Implementation
    // ==========================================
    
    // Function: Split the list into two halves using "Fast & Slow" pointers
    Passenger* split(Passenger* source) {
        Passenger* fast = source;
        Passenger* slow = source;
        
        // Fast moves 2 steps, Slow moves 1 step
        while (fast->next != nullptr && fast->next->next != nullptr) {
            fast = fast->next->next;
            slow = slow->next;
        }
        
        Passenger* temp = slow->next;
        slow->next = nullptr; // Cut the list
        if (temp) temp->prev = nullptr; // Break backward link
        return temp;
    }

    // Function: Merge two sorted lists together (Recursive)
    Passenger* merge(Passenger* first, Passenger* second) {
        if (!first) return second;
        if (!second) return first;

        Passenger* result = nullptr;

        // Compare IDs
        if (first->passengerID <= second->passengerID) {
            result = first;
            result->next = merge(first->next, second);
            if (result->next) result->next->prev = result; // Maintain Prev pointer
            result->prev = nullptr;
        } else {
            result = second;
            result->next = merge(first, second->next);
            if (result->next) result->next->prev = result;
            result->prev = nullptr;
        }
        return result;
    }

    // Function: Recursive Merge Sort Main Function
    Passenger* mergeSortRec(Passenger* node) {
        if (!node || !node->next) return node; // Base case: 0 or 1 element
        
        Passenger* second = split(node); // Divide
        node = mergeSortRec(node);       // Sort Left
        second = mergeSortRec(second);   // Sort Right
        
        return merge(node, second);      // Conquer (Merge)
    }

public:

    void processWaitlist() override {
        cout << ">> [System] Processing Waitlist for empty seats..." << endl;

        if (waitlistHead == nullptr) return;

        WaitlistNode* prev = nullptr;
        WaitlistNode* curr = waitlistHead;

        while (curr != nullptr) {
            bool assigned = false;
            int startRow, endRow;

            // 1. Determine Class Zone
            if (curr->flightClass == "First") { startRow = 1; endRow = 3; }
            else if (curr->flightClass == "Business") { startRow = 4; endRow = 10; }
            else { startRow = 11; endRow = 30; } // Assuming max 30 for loop limit

            // 2. Find Empty Seat
            for (int r = startRow; r <= endRow; r++) {
                for (int c = 0; c < FlightGlobal::COLS; c++) {
                    string colName = FlightGlobal::getColName(c);

                    // CHECK: Is (r, colName) occupied?
                    bool occupied = false;
                    Passenger* temp = head;
                    while (temp != nullptr) {
                        if (temp->seatRow == r && temp->seatCol == colName) {
                            occupied = true;
                            break;
                        }
                        temp = temp->next;
                    }

                    // If NOT occupied, we found a spot!
                    if (!occupied) {
                        cout << ">> [Auto-Assign] Moved " << curr->name << " from Waitlist to Seat " << r << colName << "." << endl;
                        addPassenger(curr->id, curr->name, r, colName, curr->flightClass);
                        assigned = true;
                        goto FOUND_SEAT_LL;
                    }
                }
            }

            FOUND_SEAT_LL:

            // 3. Remove from Waitlist
            if (assigned) {
                WaitlistNode* toDelete = curr;
                if (prev == nullptr) {
                    waitlistHead = curr->next;
                    if (waitlistHead == nullptr) waitlistTail = nullptr;
                    curr = waitlistHead;
                } else {
                    prev->next = curr->next;
                    if (prev->next == nullptr) waitlistTail = prev;
                    curr = curr->next;
                }
                delete toDelete;
            } else {
                prev = curr;
                curr = curr->next;
            }
        }
    }

    // ==========================================
    // CONSTRUCTOR & DESTRUCTOR
    // ==========================================
    LinkedListSystem() {
        head = nullptr;
        tail = nullptr;
        currentCount = 0;

        // Initialize Waitlist
        waitlistHead = nullptr;
        waitlistTail = nullptr;
        
        cout << ">> Linked List System Initialized." << endl;
    }

    ~LinkedListSystem() {
        // 1. Clean up Main Passenger List (Doubly Linked)
        Passenger* temp = head;
        while (temp != nullptr) {
            Passenger* nextNode = temp->next;
            delete temp;
            temp = nextNode;
        }

        // 2. Clean up Waitlist (Singly Linked)
        WaitlistNode* wTemp = waitlistHead;
        while (wTemp != nullptr) {
            WaitlistNode* wNext = wTemp->next;
            delete wTemp;
            wTemp = wNext;
        }
        
        cout << ">> Linked List System Destroyed (Memory Freed)." << endl;
    }

    // ==========================================
    // FUNCTION 1: Reservation (Insertion) - MODIFIED
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
        
        Passenger* temp = head;
        
        // 1. TRAVERSAL CHECK (O(N) Time Complexity)
        while (temp != nullptr) {
            // --- COLLISION CHECK ---
            if (temp->seatRow == row && temp->seatCol == col) {
                // OLD CODE: cout << ">> [Failed] Seat " << row << col << " is already occupied..."
                
                // NEW CODE: Handle the collision by adding to Waitlist
                cout << ">> [System] Seat " << row << col << " is taken by " << temp->name 
                     << ". Adding " << name << " to Waitlist..." << endl;
                
                // >>> THIS IS THE MISSING LINE <<<
                addToWaitlist(id, name, row, col, fclass); 
                
                return false; // Return false to indicate they didn't get a seat (but they ARE saved now)
            }

            // --- DUPLICATE ID CHECK ---
            if (temp->passengerID == id) {
                cout << ">> [Failed] Passenger ID " << id << " already exists." << endl;
                return false; 
            }
            temp = temp->next;
        }

        // 2. Create New Node (If no collision, proceed as normal...)
        Passenger* newP = new Passenger;
        newP->passengerID = id;
        newP->name = name;
        newP->seatRow = row;
        newP->seatCol = col;
        newP->flightClass = fclass;
        newP->next = nullptr;
        newP->prev = nullptr;

        // 3. Append to Tail
        if (head == nullptr) {
            head = newP;
            tail = newP;
        } else {
            tail->next = newP;   
            newP->prev = tail;   
            tail = newP;         
        }
        currentCount++;
        // cout << ">> [Success] ... " << endl; // Optional: Comment out to reduce spam during loading
        return true;
    }

    // ==========================================
    // FUNCTION 2: Cancellation (Deletion)
    // ==========================================
bool removePassenger(string id) override {
    if (head == nullptr) return false;

    Passenger* current = head;
    int freedRow = -1;
    string freedCol = "";
    bool found = false;

    // 1. Find and Remove Passenger
    while (current != nullptr) {
        if (current->passengerID == id) {
            
            freedRow = current->seatRow;
            freedCol = current->seatCol;

            if (current == head) {
                head = current->next;
                if (head != nullptr) head->prev = nullptr;
                else tail = nullptr;
            }
            else if (current == tail) {
                tail = current->prev;
                tail->next = nullptr;
            }
            else {
                current->prev->next = current->next;
                current->next->prev = current->prev;
            }

            delete current;
            currentCount--;
            found = true;
            cout << ">> [System] Passenger " << id << " removed. Seat " << freedRow << freedCol << " is now free." << endl;
            break; 
        }
        current = current->next;
    }

    if (!found) return false;

    // ==========================================
    // AUTO-FILL FROM WAITLIST (Corrected Names)
    // ==========================================
    WaitlistNode* prev = nullptr;
    WaitlistNode* curr = waitlistHead;

    while (curr != nullptr) {
        // ERROR FIX: Use 'row' and 'col' here too
        if (curr->row == freedRow && curr->col == freedCol) {
            
            cout << ">> [Waitlist] Found match! Promoting " << curr->name << " to Seat " << freedRow << freedCol << "." << endl;

            addPassenger(curr->id, curr->name, freedRow, freedCol, curr->flightClass);

            if (prev == nullptr) {
                waitlistHead = curr->next;
                if (waitlistHead == nullptr) waitlistTail = nullptr;
            } else {
                prev->next = curr->next;
                if (prev->next == nullptr) waitlistTail = prev;
            }
            
            delete curr;
            break; 
        }
        prev = curr;
        curr = curr->next;
    }

    return true;
}

    // ==========================================
    // FUNCTION 3: Search (Modified to check Waitlist)
    // ==========================================
    Passenger* searchPassenger(const string& id) override {
        // 1. First, check the Main Flight (Seated Passengers)
        Passenger* current = head;
        while (current != nullptr) {
            if (current->passengerID == id) {
                return current; // Found on the plane!
            }
            current = current->next;
        }

        // 2. If not found, check the Waitlist
        WaitlistNode* wTemp = waitlistHead;
        while (wTemp != nullptr) {
            if (wTemp->id == id) {
                // FOUND IN WAITLIST!
                // Problem: This function must return a 'Passenger*', but wTemp is a 'WaitlistNode*'.
                // Solution: Create a static temporary passenger to return.
                
                static Passenger tempResult; 
                // Using 'static' ensures this object survives after the function ends so main.cpp can read it.
                
                tempResult.passengerID = wTemp->id;
                tempResult.name = wTemp->name + " [WAITLIST]"; // Add a tag so you know
                tempResult.seatRow = 0;      // 0 indicates no seat
                tempResult.seatCol = "WL";   // "WL" for Waitlist
                tempResult.flightClass = wTemp->flightClass;
                
                return &tempResult;
            }
            wTemp = wTemp->next;
        }

        return nullptr; // Really not found anywhere
    }

    // ==========================================
    // FUNCTION 4: Display Map
    // Description: Renders the visual grid.
    // Note: Inefficient for Linked Lists (requires O(N) search for every cell).
    // ==========================================
void displaySeatingMap() override {
    // 1. Calculate Last Active Row (Iterate through list to find max)
    int maxRowDisplayed = 60; 
    /* Optional: 
    Passenger* tempMax = head;
    while(tempMax) {
        if(tempMax->seatRow > maxRowDisplayed) maxRowDisplayed = tempMax->seatRow;
        tempMax = tempMax->next;
    } */

    int totalPages = (maxRowDisplayed + FlightGlobal::ROWS_PER_PAGE - 1) / FlightGlobal::ROWS_PER_PAGE;
    int currentPage = 1;

    while (true) {
        cout << string(50, '\n'); 
        cout << "==========================================================================================" << endl;
        cout << "                       FLIGHT SEATING MAP (LINKED LIST SYSTEM)                            " << endl;
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
            
            cout << rowClass << right << setw(2) << setfill('0') << actualRow << setfill(' ') << " ";

            for (int c = 0; c < FlightGlobal::COLS; c++) {
                string colName = FlightGlobal::getColName(c);
                
                // [FIX] Declare 'display' EXACTLY ONCE here
                string display = "EMPTY";
                
                // Linked List Logic: Search for the passenger
                Passenger* p = head;
                while (p != nullptr) {
                    if (p->seatRow == actualRow && p->seatCol == colName) {
                        display = p->name;
                        break;
                    }
                    p = p->next;
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
    // ALGORITHM 1: Bubble Sort (By Name)
    // Strategy: Swap Data instead of Nodes for simplicity
    // ==========================================
    void sortAlphabetically() override {
        if (head == nullptr || head->next == nullptr) return;

        bool swapped;
        Passenger* ptr1;
        Passenger* lptr = nullptr;

        cout << ">> Sorting Linked List by Name (Bubble Sort)..." << endl;

        do {
            swapped = false;
            ptr1 = head;

            while (ptr1->next != lptr) {
                if (ptr1->name > ptr1->next->name) {
                    // SWAP DATA (Efficiency: Avoiding complex pointer re-wiring)
                    swap(ptr1->passengerID, ptr1->next->passengerID);
                    swap(ptr1->name, ptr1->next->name);
                    swap(ptr1->seatRow, ptr1->next->seatRow);
                    swap(ptr1->seatCol, ptr1->next->seatCol);
                    swap(ptr1->flightClass, ptr1->next->flightClass);
                    swapped = true;
                }
                ptr1 = ptr1->next;
            }
            lptr = ptr1;
        } while (swapped);

        cout << ">> Sort Complete." << endl;
        displayManifest();
    }

    // ==========================================
    // WAITLIST IMPLEMENTATION (Singly Linked List)
    // ==========================================
    void addToWaitlist(string id, string name, int row, string col, string fclass) override {
        WaitlistNode* newNode = new WaitlistNode;
        newNode->id = id;
        newNode->name = name;
        newNode->row = row;
        newNode->col = col;
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
    // Complexity: O(N log N) - Ideal for Linked Lists
    // ==========================================
    void sortByID() override {
        if (head == nullptr || head->next == nullptr) {
            cout << ">> Not enough passengers to sort." << endl;
            return;
        }
        cout << ">> [Linked List] Sorting by ID using MERGE SORT..." << endl;

        // Perform Merge Sort
        head = mergeSortRec(head);

        // Fix the Tail Pointer (Crucial after pointer manipulation)
        Passenger* temp = head;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        tail = temp;

        cout << ">> Sort Complete (Merge Sort)." << endl;
        displayManifest();
    }
};