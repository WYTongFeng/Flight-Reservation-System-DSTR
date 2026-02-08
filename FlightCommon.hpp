#ifndef FLIGHTCOMMON_HPP
#define FLIGHTCOMMON_HPP

#include <string>
#include <iostream>
#include <iomanip>
#include <limits> // For numeric_limits
#include <cmath>  // For ceil

using namespace std;

// ==========================================
// 1. Passenger Data Structure (Data Model)
// Unified Standard: Everyone must use this structure
// ==========================================
struct Passenger {
    // --- Basic Data (From CSV) ---
    string passengerID; // e.g., "P1001"
    string name;        // e.g., "John Doe"
    int seatRow;        // e.g., 1
    string seatCol;     // e.g., "A"
    string flightClass; // e.g., "Economy"

    // --- Linked List Specific Pointers (Array team please ignore, initialize to nullptr) ---
    
    // 1. Used for Singly Linked List (Waitlist)
    // 2. Also used for Doubly Linked List (Main List) as "next"
    Passenger* next = nullptr; 

    // 1. Only used for Doubly Linked List (Main List) as "previous"
    // 2. Waitlist does not need this (keep as nullptr)
    Passenger* prev = nullptr; 
};

struct WaitlistNode {
    string id;
    string name;
    int row;      // Remember the row they wanted
    string col;   // Remember the column they wanted
    string flightClass;
    WaitlistNode* next;
};

// ==========================================
// 2. Global Configuration & Utilities (FlightGlobal)
//    Both Array and LL teams share this setting
// ==========================================
namespace FlightGlobal {
    
    // --- Constant Configuration (Constants) ---
    const int COLS = 6;             // Changed back to 6 columns (A-F)
    const int COL_WIDTH = 15;       // Width 15, just enough to show the name without breaking the screen layout
    const int ROWS_PER_PAGE = 15;   // Show 15 rows per page
    const int MANIFEST_PER_PAGE = 15;
    const int DEFAULT_MAX_ROWS = 60; // A normal plane has about 50-60 rows

    // --- Helper Functions (Helpers) ---

    inline string getColString(int index) {
        if (index < 0 || index >= COLS) return "?";
        char c = 'A' + index;
        return string(1, c);
    }

    // 1. Get Column Name (0 -> "A")
    inline string getColName(int index) { return getColString(index); }

    // 2. Get Column Index ("A" -> 0)
    inline int getColIndex(string col) {
        if (col.empty()) return -1;
        char c = toupper(col[0]);
        if (c >= 'A' && c <= ('A' + COLS - 1)) return c - 'A'; 
        return -1;
    }

    // 3. Format Name Display (Handle empty seats)
    inline string formatName(string name) {
        if (name == "" || name == "EMPTY") return "---";
        return name;
    }
}

// ==========================================
// 3. System Interface (System Interface)
// Abstract Base Class: Enforces Array and Linked List teams to implement the same functionality
// ==========================================
class FlightSystem {
public:
    // --- Core Functions (Each person is responsible for 2) ---

    // [Function 1] Reservation (Insertion)
    // Array: Fill into 2D array
    // LL: Insert into Doubly Linked List (Doubly Insert)
    virtual bool addPassenger(string id, string name, int row, string col, string fclass) = 0;

    // [Function 2] Cancellation (Deletion)
    // Array: Clear cell + 1D Array Shift
    // LL: Disconnect Doubly Linked List node (Doubly Unlink) + Memory Release
    virtual bool removePassenger(string id) = 0;

    // [Function 3] Seat Lookup (Search)
    // ⚠️ Rule: Must use Linear Search to maintain fair comparison
    virtual Passenger* searchPassenger(const string& id) = 0;

    // [Function 4] Manifest & Report
    // Print passenger manifest (Includes Main List + Waitlist)
    virtual void displayManifest() = 0;
    
    // Print visual seating map (Array team's highlight: 2D Grid)
    virtual void displaySeatingMap() = 0;

    // --- Extra Algorithms & Special Requirements ---

    // [Requirement] Sorting Algorithm
    // ⚠️ Rule: Must use Bubble Sort to sort by name A-F
    virtual void sortAlphabetically() = 0; // Existing Bubble Sort
    
    virtual void sortByID() { cout << ">> Feature not available." << endl; }

    // [Requirement] Singly Linked List (Waitlist)
    // Specifically to meet the lecturer's "Use Singly and Doubly" requirement
    // When the flight is full, call this function to add to the Singly Linked List
    virtual void addToWaitlist(string id, string name, int row, string col, string fclass) = 0;
    virtual void processWaitlist() = 0;

    // [Safety] Virtual Destructor
    // Ensure memory is correctly cleaned up when program closes (Avoid Memory Leak)
    virtual ~FlightSystem() {}
};

// Check if the seat row matches the passenger's ticket class
bool validateSeatClass(int row, string fclass) {
    // 1. First Class Zone (Rows 1-3)
    if (row >= 1 && row <= 3) {
        if (fclass != "First") return false;
    }
    // 2. Business Class Zone (Rows 4-10)
    else if (row >= 4 && row <= 10) {
        if (fclass != "Business") return false;
    }
    // 3. Economy Class Zone (Rows 11+)
    else if (row >= 11) {
        if (fclass != "Economy") return false;
    }
    return true;
}

#endif