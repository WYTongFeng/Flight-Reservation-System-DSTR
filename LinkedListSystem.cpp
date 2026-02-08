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
    // FUNCTION 1: Reservation (Insertion)
    // ==========================================
    bool addPassenger(string id, string name, int row, string col, string fclass) override {
        
        Passenger* temp = head;
        
        // 1. TRAVERSAL CHECK (O(N) Time Complexity)
        // Contrast with Array: Array checks seat in O(1).
        // Linked List MUST traverse the whole list to find duplicates or collisions.
        while (temp != nullptr) {
            // Check Seat Collision
            if (temp->seatRow == row && temp->seatCol == col) {
                cout << ">> [Failed] Seat " << row << col << " is already occupied by " << temp->name << "." << endl;
                return false; 
            }
            // Check Duplicate ID
            if (temp->passengerID == id) {
                cout << ">> [Failed] Passenger ID " << id << " already exists." << endl;
                return false; 
            }
            temp = temp->next;
        }

        // 2. Create New Node
        Passenger* newP = new Passenger;
        newP->passengerID = id;
        newP->name = name;
        newP->seatRow = row;
        newP->seatCol = col;
        newP->flightClass = fclass;
        newP->next = nullptr;
        newP->prev = nullptr;

        // 3. Append to Tail (O(1) insertion if Tail pointer exists)
        if (head == nullptr) {
            head = newP;
            tail = newP;
        } else {
            tail->next = newP;   // Link old tail to new node
            newP->prev = tail;   // Link new node back to old tail
            tail = newP;         // Update tail
        }
        currentCount++;
        cout << ">> [Success] Passenger " << name << " (" << id << ") added to linked list." << endl;
        return true;
    }

    // ==========================================
    // FUNCTION 2: Cancellation (Deletion)
    // ==========================================
    bool removePassenger(string id) override {
        if (head == nullptr) return false;

        Passenger* current = head;

        // Traverse to find node
        while (current != nullptr) {
            if (current->passengerID == id) {
                // FOUND! Now unlink it (Pointer rewiring)
                
                // Case 1: Removing Head Node
                if (current == head) {
                    head = current->next;
                    if (head != nullptr) head->prev = nullptr;
                    else tail = nullptr; // List became empty
                }
                // Case 2: Removing Tail Node
                else if (current == tail) {
                    tail = current->prev;
                    tail->next = nullptr;
                }
                // Case 3: Removing Middle Node
                else {
                    current->prev->next = current->next;
                    current->next->prev = current->prev;
                }

                delete current; // Free memory
                currentCount--;
                return true;
            }
            current = current->next;
        }
        return false; // Not found
    }

    // ==========================================
    // FUNCTION 3: Search (Linear Search)
    // ==========================================
    Passenger* searchPassenger(const string& id) override {
        Passenger* current = head;
        // Traverse linearly O(N)
        while (current != nullptr) {
            if (current->passengerID == id) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

    // ==========================================
    // FUNCTION 4: Display Map
    // Description: Renders the visual grid.
    // Note: Inefficient for Linked Lists (requires O(N) search for every cell).
    // ==========================================
    void displaySeatingMap() override {
        // 1. Find max row to determine map size
        int maxRow = 20; 
        Passenger* temp = head;
        while (temp != nullptr) {
            if (temp->seatRow > maxRow) maxRow = temp->seatRow;
            temp = temp->next;
        }

        int totalPages = (maxRow + FlightGlobal::ROWS_PER_PAGE - 1) / FlightGlobal::ROWS_PER_PAGE;
        int currentPage = 1;

        while (true) {
            cout << string(50, '\n'); 
            cout << "==========================================================================" << endl;
            cout << "                    FLIGHT SEATING MAP (LINKED LIST)                      " << endl;
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
            if (endRow > maxRow) endRow = maxRow;

            // RENDER LOGIC: Nested Loop
            for (int r = startRow; r < endRow; r++) {
                int actualRow = r + 1;
                
                string rowClass = "Eco"; 
                if (r < 3) rowClass = "Fst";      
                else if (r < 10) rowClass = "Bus";

                cout << rowClass << setw(2) << setfill('0') << actualRow << setfill(' ') << " ";

                for (int c = 0; c < FlightGlobal::COLS; c++) {
                    string colName = FlightGlobal::getColName(c);
                    
                    // SEARCH the list for this specific seat (row, col)
                    string foundName = "EMPTY";
                    Passenger* p = head;
                    while (p != nullptr) {
                        if (p->seatRow == actualRow && p->seatCol == colName) {
                            foundName = p->name;
                            break;
                        }
                        p = p->next;
                    }

                    if (foundName != "EMPTY") foundName = FlightGlobal::formatName(foundName);
                    if (foundName.length() > 12) foundName = foundName.substr(0, 9) + "..";

                    cout << "[" << left << setw(10) << foundName << "] ";
                    if (c == 2) cout << "    "; 
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
    // FUNCTION 5: Manifest
    // ==========================================
    void displayManifest() override {
        if (head == nullptr) {
            cout << ">> [Manifest] List is empty." << endl;
            return;
        }

        cout << "\n==============================================================" << endl;
        cout << "             PASSENGER MANIFEST (LINKED LIST)" << endl;
        cout << "==============================================================" << endl;
        cout << left << setw(10) << "ID" 
             << left << setw(20) << "Name" 
             << left << setw(10) << "Seat" 
             << left << setw(15) << "Class" << endl;
        cout << "--------------------------------------------------------------" << endl;

        // Display Main Doubly Linked List
        Passenger* current = head;
        while (current != nullptr) {
            string fullSeat = to_string(current->seatRow) + current->seatCol;
            cout << left << setw(10) << current->passengerID 
                 << left << setw(20) << current->name 
                 << left << setw(10) << fullSeat
                 << left << setw(15) << current->flightClass << endl;
            current = current->next;
        }
        
        // --- DISPLAY WAITLIST (Singly Linked List) ---
        // Added this section to demonstrate Singly Linked List usage to lecturer
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
    void addToWaitlist(string id, string name, string fclass) override {
        WaitlistNode* newNode = new WaitlistNode;
        newNode->id = id;
        newNode->name = name;
        newNode->flightClass = fclass;
        newNode->next = nullptr;

        // Add to Tail (Singly Linked List Logic)
        if (waitlistHead == nullptr) {
            waitlistHead = newNode;
            waitlistTail = newNode;
        } else {
            waitlistTail->next = newNode; // Point old tail to new node
            waitlistTail = newNode;       // Update tail
        }
        cout << ">> [Waitlist] " << name << " added to priority queue." << endl;
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