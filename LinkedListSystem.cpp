#include "FlightCommon.hpp"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

class LinkedListSystem : public FlightSystem {
private:
    Passenger* head;
    Passenger* tail;
    int currentCount;

    // --- NEW: Waitlist Variables (Singly Linked List) ---
    WaitlistNode* waitlistHead;
    WaitlistNode* waitlistTail;

public:
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
        Passenger* temp = head;
        while (temp != nullptr) {
            Passenger* nextNode = temp->next;
            delete temp;
            temp = nextNode;
        }
    }

    // ==========================================
    // Function 1: Reservation (Insertion)
    // ==========================================
void addPassenger(string id, string name, int row, string col, string fclass) override {
    
    Passenger* temp = head;
    
    // TRAVERSAL CHECK (O(N))
    // We check BOTH "Seat Collision" and "Duplicate ID" in one pass.
    while (temp != nullptr) {
        // Check Seat
        if (temp->seatRow == row && temp->seatCol == col) {
            cout << ">> [Failed] Seat " << row << col << " is already occupied by " << temp->name << "." << endl;
            return; 
        }
        // Check ID
        if (temp->passengerID == id) {
            cout << ">> [Failed] Passenger ID " << id << " already exists." << endl;
            return; 
        }
        temp = temp->next;
    }

    // If we get here, it's safe to add.
    Passenger* newP = new Passenger;
    newP->passengerID = id;
    newP->name = name;
    newP->seatRow = row;
    newP->seatCol = col;
    newP->flightClass = fclass;
    newP->next = nullptr;
    newP->prev = nullptr;

    // Append to Tail (O(1) with tail pointer)
    if (head == nullptr) {
        head = newP;
        tail = newP;
    } else {
        tail->next = newP;
        newP->prev = tail;
        tail = newP;
    }
    currentCount++;
    cout << ">> [Success] Passenger " << name << " (" << id << ") added to linked list." << endl;
}

    // ==========================================
    // Function 2: Cancellation (Deletion)
    // ==========================================
    bool removePassenger(string id) override {
        if (head == nullptr) return false;

        Passenger* current = head;

        // Traverse to find node
        while (current != nullptr) {
            if (current->passengerID == id) {
                // FOUND! Now unlink it.
                
                // Case 1: Head Node
                if (current == head) {
                    head = current->next;
                    if (head != nullptr) head->prev = nullptr;
                    else tail = nullptr; // List became empty
                }
                // Case 2: Tail Node
                else if (current == tail) {
                    tail = current->prev;
                    tail->next = nullptr;
                }
                // Case 3: Middle Node
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
    // Function 3: Search (Linear Search)
    // ==========================================
    Passenger* searchPassenger(const string& id) override {
        Passenger* current = head;
        while (current != nullptr) {
            if (current->passengerID == id) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

    // ==========================================
    // Function 4: Display Map
    // ==========================================
    // Challenge: Linked Lists don't have rows/cols structure. 
    // To match the Array System's visual, we must build a temporary "View"
    void displaySeatingMap() override {
        // 1. Find max row to know how much to print
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

            // RENDER LOGIC: For every row, we must search the list (Inefficient, but necessary for LL)
            for (int r = startRow; r < endRow; r++) {
                int actualRow = r + 1;
                
                // Determine Class Label
                string rowClass = "Eco"; 
                if (r < 3) rowClass = "Fst";      
                else if (r < 10) rowClass = "Bus";

                cout << rowClass << setw(2) << setfill('0') << actualRow << setfill(' ') << " ";

                // Print Columns A-F
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

                    // Format output
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
    // Function 5: Manifest
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

        Passenger* current = head;
        while (current != nullptr) {
            string fullSeat = to_string(current->seatRow) + current->seatCol;
            cout << left << setw(10) << current->passengerID 
                 << left << setw(20) << current->name 
                 << left << setw(10) << fullSeat
                 << left << setw(15) << current->flightClass << endl;
            current = current->next;
        }
        cout << "==============================================================" << endl;
        cout << "Press Any Key + Enter to continue...";
        string dummy; cin >> dummy;
    }

    // ==========================================
    // Function 6: Bubble Sort (Swapping Data)
    // ==========================================
    void sortAlphabetically() override {
        if (head == nullptr || head->next == nullptr) return;

        bool swapped;
        Passenger* ptr1;
        Passenger* lptr = nullptr;

        cout << ">> Sorting Linked List by Name..." << endl;

        do {
            swapped = false;
            ptr1 = head;

            while (ptr1->next != lptr) {
                if (ptr1->name > ptr1->next->name) {
                    // SWAP DATA (Easier and safer than swapping nodes)
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
    // Singly Linked List Waitlist (Requirement)
    // ==========================================
    void addToWaitlist(string id, string name, string fclass) override {
        // Create Node
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
};