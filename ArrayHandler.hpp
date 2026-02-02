#ifndef ARRAY_HANDLER_HPP
#define ARRAY_HANDLER_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// Define the Passenger structure as per dataset requirements [cite: 18, 19]
struct Passenger {
    string id;
    string name;
    int seatRow;
    char seatCol;
    string travelClass;
};

// Member 2 Class to handle Array-based operations
class ArrayHandler {
private:
    static const int MAX_SEATS = 200; // Fixed size as per static array requirement [cite: 71]
    Passenger passengerArray[MAX_SEATS];
    int currentSize;

public:
    ArrayHandler();
    
    // Core Functionalities for Member 2 [cite: 35, 37, 38]
    void loadFromCSV(string filename);
    int seatLookup(string targetID); // Linear Search
    void cancellation(string targetID); // Deletion with Shifting
    void bubbleSortByName(); // Sorting for fairness comparison
    void displayManifest();
    
    // Getter for performance testing
    int getCurrentSize() { return currentSize; }
};

#endif