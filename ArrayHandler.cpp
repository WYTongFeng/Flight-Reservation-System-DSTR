#include "ArrayHandler.hpp"
#include <fstream>
#include <sstream>

// Constructor to initialize the current size of the array
ArrayHandler::ArrayHandler() : currentSize(0) {}

// CSV Loader - Handles file input and data parsing [cite: 35, 41, 73]
void ArrayHandler::loadFromCSV(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        // Error Handling: Missing file 
        cout << "[Error] Critical: Could not open file " << filename << endl;
        return;
    }

    string line;
    // Skip the header line (PassengerID, Name, etc.) [cite: 19]
    getline(file, line); 

    // Read each line until end of file or array is full [cite: 25]
    while (getline(file, line) && currentSize < MAX_SEATS) {
        stringstream ss(line);
        string rowStr, colStr;
        
        // Parsing CSV: PassengerID, Name, SeatRow, SeatColumn, Class [cite: 19]
        getline(ss, passengerArray[currentSize].id, ',');
        getline(ss, passengerArray[currentSize].name, ',');
        getline(ss, rowStr, ',');
        getline(ss, colStr, ',');
        getline(ss, passengerArray[currentSize].travelClass, ',');

        // Convert string to appropriate data types 
        if (!rowStr.empty()) {
            passengerArray[currentSize].seatRow = stoi(rowStr);
        }
        if (!colStr.empty()) {
            passengerArray[currentSize].seatCol = colStr[0];
        }
        
        currentSize++;
    }
    file.close();
    cout << "[System] Array loaded with " << currentSize << " passengers." << endl;
}

// Seat Lookup: Linear Search (O(n)) for fairness comparison [cite: 38, 66, 70]
int ArrayHandler::seatLookup(string targetID) {
    for (int i = 0; i < currentSize; i++) {
        if (passengerArray[i].id == targetID) {
            return i; // Return the index of the found passenger
        }
    }
    // Error Handling: Passenger not found 
    return -1; 
}

// Cancellation: Removes passenger and performs element shifting [cite: 37, 70]
void ArrayHandler::cancellation(string targetID) {
    int index = seatLookup(targetID); // First, find the passenger [cite: 38]
    if (index == -1) {
        cout << "[Error] Cancellation Failed: ID " << targetID << " not found." << endl;
        return;
    }
    
    // Core Array Logic: Shift all elements after the index to the left 
    for (int i = index; i < currentSize - 1; i++) {
        passengerArray[i] = passengerArray[i + 1];
    }
    
    currentSize--; // Reduce the count of passengers
    cout << "[System] Passenger " << targetID << " removed. Array elements shifted." << endl;
}

// Bubble Sort: Sorting passengers by Name for performance comparison [cite: 57, 70]
void ArrayHandler::bubbleSortByName() {
    if (currentSize <= 1) return; // Nothing to sort 

    for (int i = 0; i < currentSize - 1; i++) {
        for (int j = 0; j < currentSize - i - 1; j++) {
            // Compare names of adjacent passengers
            if (passengerArray[j].name > passengerArray[j + 1].name) {
                // Swap the passenger data
                Passenger temp = passengerArray[j];
                passengerArray[j] = passengerArray[j + 1];
                passengerArray[j + 1] = temp;
            }
        }
    }
    cout << "[System] Array successfully sorted by Name." << endl;
}

// Manifest Report: Print all passenger data in the array [cite: 38, 43]
void ArrayHandler::displayManifest() {
    if (currentSize == 0) {
        cout << "[Notice] System Empty: No records to display." << endl;
        return;
    }
    cout << "\n--- Current Passenger Manifest (Array Version) ---" << endl;
    for (int i = 0; i < currentSize; i++) {
        cout << "ID: " << passengerArray[i].id 
             << " | Name: " << passengerArray[i].name 
             << " | Seat: " << passengerArray[i].seatRow << passengerArray[i].seatCol 
             << " | Class: " << passengerArray[i].travelClass << endl;
    }
}
