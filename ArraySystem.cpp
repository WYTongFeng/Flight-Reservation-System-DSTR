#include "FlightCommon.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <limits> // 用于清理输入缓冲区
#include <cmath> // 用于 ceil 计算页数

using namespace std;

// ==========================================
// 系统配置
// ==========================================


class ArraySystem : public FlightSystem {
private:
    string** seatMap; 
    int maxRows; 
    Passenger** passengerList; 
    int passengerCapacity; 
    int currentCount;      

    // 自动扩容 - Map
void expandSeatMap(int requiredRow) {
    if (requiredRow <= maxRows) return;
    int newMax = maxRows * 2; 
    if (newMax < requiredRow) newMax = requiredRow + 10;

    string** newMap = new string*[newMax];
    for (int i = 0; i < newMax; i++) {
        newMap[i] = new string[FlightGlobal::COLS]; // 改这里
        for (int j = 0; j < FlightGlobal::COLS; j++) newMap[i][j] = "EMPTY"; // 改这里
    }

    for (int i = 0; i < maxRows; i++) {
        for (int j = 0; j < FlightGlobal::COLS; j++) newMap[i][j] = seatMap[i][j]; // 改这里
    }
        for (int i = 0; i < maxRows; i++) delete[] seatMap[i];
        delete[] seatMap;

        seatMap = newMap;
        maxRows = newMax;
    }

    // 自动扩容 - List
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
    // 1. Constructor
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

    cout << ">> Array System Initialized." << endl;
}

    ~ArraySystem() {
        for (int i = 0; i < maxRows; i++) delete[] seatMap[i];
        delete[] seatMap;
        for (int i = 0; i < currentCount; i++) delete passengerList[i];
        delete[] passengerList;
    }

// ==========================================
    // Function 1: Reservation (Modified for FULL LIST)
    // ==========================================
    void addPassenger(string id, string name, int row, string col, string fclass) override {
        // 1. 自动扩容检测
        if (row > maxRows) expandSeatMap(row);
        if (currentCount >= passengerCapacity) expandPassengerList();

        int rIndex = row - 1;
        int cIndex = FlightGlobal::getColIndex(col); // 加上 FlightGlobal::

        if (rIndex < 0 || cIndex == -1) return;

        // ==========================================================
        // 【关键修改】不管座位是否冲突，先把人加进 1D 名单 (passengerList)
        // 这样你的 Sort 就能排 10,000 个人了！
        // ==========================================================
        Passenger* newP = new Passenger;
        newP->passengerID = id;
        newP->name = name;
        newP->seatRow = row;
        newP->seatCol = col;
        newP->flightClass = fclass;
        passengerList[currentCount++] = newP; // 存入名单，人数+1

        // ==========================================================
        // 然后再处理 2D 地图 (seatMap) - 只有空位才填名字
        // ==========================================================
        if (seatMap[rIndex][cIndex] != "EMPTY") {
            // 如果座位被占，只在控制台提示一下，但不要 return！
            // 因为人已经加进上面的名单了
            // cout << ">> [Note] Seat " << row << col << " occupied. Passenger added to list only." << endl;
        } else {
            // 如果座位是空的，把名字写在地图上
            seatMap[rIndex][cIndex] = name;
        }
    }

    // [Function 2] Remove Passenger
    bool removePassenger(string id) override {
        int index = -1;
        
        // 1. Linear Search: Find the passenger's position
        for (int i = 0; i < currentCount; i++) {
            if (passengerList[i] != nullptr && passengerList[i]->passengerID == id) {
                index = i;
                break;
            }
        }

        // 2. If not found, we use your original logic or a fail message
        if (index == -1) {
            cout << "[Array] Remove Passenger failed: ID not found." << endl;
            return false;
        }

        // 3. Sync with 2D Seat Map: Clear the name on the map
        int rIndex = passengerList[index]->seatRow - 1;
        int cIndex = FlightGlobal::getColIndex(passengerList[index]->seatCol);
        if (rIndex >= 0 && rIndex < maxRows && cIndex != -1) {
            seatMap[rIndex][cIndex] = "EMPTY"; 
        }

        // 4. Delete object and Shift elements (The core Member 2 task)
        delete passengerList[index]; 
        for (int i = index; i < currentCount - 1; i++) {
            passengerList[i] = passengerList[i + 1]; 
        }
        passengerList[currentCount - 1] = nullptr; 
        currentCount--;

        cout << "[Array] Success: Passenger " << id << " removed." << endl;
        return true;
    }

    // [Function 3] Search Passenger
    Passenger* searchPassenger(const string& id) override {
        // Linear Search logic
        for (int i = 0; i < currentCount; i++) {
            if (passengerList[i] != nullptr && passengerList[i]->passengerID == id) {
                cout << "[Array] Passenger " << id << " found." << endl;
                return passengerList[i]; 
            }
        }
        
        cout << "[Array] Search: ID " << id << " not found." << endl;
        return nullptr;
    }

    // =========================================================
    // [FUNCTION 4] Visual Map - Horizontal & Interactive Paging
    // =========================================================
    void displaySeatingMap() override {
        int lastActiveRow = 0;
        for(int i = maxRows - 1; i >= 0; i--) {
            bool empty = true;
            for(int j=0; j<FlightGlobal::COLS; j++) {
                if(seatMap[i][j] != "EMPTY") { empty = false; break; }
            }
            if(!empty) { lastActiveRow = i + 1; break; }
        }
        if (lastActiveRow < 20) lastActiveRow = 20; 

        int totalPages = (lastActiveRow + FlightGlobal::ROWS_PER_PAGE - 1) / FlightGlobal::ROWS_PER_PAGE;
        int currentPage = 1;

        while (true) {
            cout << string(50, '\n'); 

            cout << "========================================================================================================================" << endl;
            cout << "                                              FLIGHT SEATING MAP (FULL VIEW)                                            " << endl;
            cout << "========================================================================================================================" << endl;
            cout << "         " 
                 << left << setw(FlightGlobal::COL_WIDTH) << "[A]" 
                 << left << setw(FlightGlobal::COL_WIDTH) << "[B]" 
                 << left << setw(FlightGlobal::COL_WIDTH) << "[C]" 
                 << "    " 
                 << left << setw(FlightGlobal::COL_WIDTH) << "[D]" 
                 << left << setw(FlightGlobal::COL_WIDTH) << "[E]" 
                 << left << setw(FlightGlobal::COL_WIDTH) << "[F]" << endl;
            cout << "------------------------------------------------------------------------------------------------------------------------" << endl;

            int startRow = (currentPage - 1) * FlightGlobal::ROWS_PER_PAGE;
            int endRow = startRow + FlightGlobal::ROWS_PER_PAGE; 
            if (endRow > lastActiveRow) endRow = lastActiveRow;

            for (int i = startRow; i < endRow; i++) {
                string rowClass = "Econo"; 
                if (i < 3) rowClass = "First";      
                else if (i < 10) rowClass = "Busin"; 
                
                cout << rowClass << " " << right << setw(2) << setfill('0') << (i + 1) << " "; 
                cout << setfill(' '); 

                for (int j = 0; j < FlightGlobal::COLS; j++) {
                    string display = FlightGlobal::formatName(seatMap[i][j]); 
                    string finalStr = "[" + display + "]";
                    cout << left << setw(FlightGlobal::COL_WIDTH) << finalStr;
                    if (j == 2) cout << "    "; 
                }
                cout << endl;
            }
            cout << "------------------------------------------------------------------------------------------------------------------------" << endl;

            cout << "PAGE " << currentPage << " OF " << totalPages << " | Total Rows: " << lastActiveRow << endl;
            cout << "[N] Next Page   [P] Prev Page   [0] Exit Map   [1-" << totalPages << "] Jump to Page" << endl;
            cout << ">> Enter choice: ";

            string input;
            cin >> input;

            if (input == "0") break;
            else if (input == "n" || input == "N") {
                if (currentPage < totalPages) currentPage++;
            }
            else if (input == "p" || input == "P") {
                if (currentPage > 1) currentPage--;
            }
            else {
                try {
                    int pageChoice = stoi(input);
                    if (pageChoice >= 1 && pageChoice <= totalPages) currentPage = pageChoice;
                } catch (...) {}
            }
        }
        cout << ">> Exiting Map View..." << endl;
    }

    // =========================================================
    // [MANIFEST] Text Report with Interactive Pagination
    // =========================================================
    void displayManifest() override {
        if (currentCount == 0) {
            cout << "[System Empty]" << endl;
            return;
        }

        int totalPages = (currentCount + FlightGlobal::MANIFEST_PER_PAGE - 1) / FlightGlobal::MANIFEST_PER_PAGE;
        int currentPage = 1;

        while (true) {
            cout << string(50, '\n'); 

            cout << "==============================================================" << endl;
            cout << "             PASSENGER MANIFEST (SORTED LIST)                 " << endl;
            cout << "==============================================================" << endl;
            cout << left << setw(10) << "ID" 
                 << left << setw(25) << "Name" 
                 << left << setw(10) << "Seat" 
                 << left << setw(15) << "Class" << endl;
            cout << "--------------------------------------------------------------" << endl;

            int startIndex = (currentPage - 1) * FlightGlobal::MANIFEST_PER_PAGE;
            int endIndex = startIndex + FlightGlobal::MANIFEST_PER_PAGE;
            if (endIndex > currentCount) endIndex = currentCount;

            for (int i = startIndex; i < endIndex; i++) {
                if (passengerList[i] == nullptr) continue;
                
                string fullSeat = to_string(passengerList[i]->seatRow) + passengerList[i]->seatCol;
                cout << left << setw(10) << passengerList[i]->passengerID 
                     << left << setw(25) << passengerList[i]->name 
                     << left << setw(10) << fullSeat
                     << left << setw(15) << passengerList[i]->flightClass << endl;
            }
            cout << "--------------------------------------------------------------" << endl;
            cout << "PAGE " << currentPage << " OF " << totalPages << " | Total Passengers: " << currentCount << endl;
            cout << "[N] Next Page   [P] Prev Page   [0] Exit List   [1-" << totalPages << "] Jump to Page" << endl;
            cout << ">> Enter choice: ";

            string input;
            cin >> input;

            if (input == "0") break;
            else if (input == "n" || input == "N") {
                if (currentPage < totalPages) currentPage++;
            }
            else if (input == "p" || input == "P") {
                if (currentPage > 1) currentPage--;
            }
            else {
                try {
                    int pageChoice = stoi(input);
                    if (pageChoice >= 1 && pageChoice <= totalPages) currentPage = pageChoice;
                } catch (...) {}
            }
        }
        cout << ">> Exiting Manifest View..." << endl;
    }

    // =========================================================
    // [SORTING] Bubble Sort + Interactive Pagination
    // =========================================================
    void sortAlphabetically() override {
        if (currentCount < 2) {
            cout << ">> Not enough passengers to sort." << endl;
            return;
        }

        cout << ">> Sorting " << currentCount << " passengers (Bubble Sort)... Please wait." << endl;
        // Bubble Sort
        for (int i = 0; i < currentCount - 1; i++) {
            for (int j = 0; j < currentCount - i - 1; j++) {
                if (passengerList[j] == nullptr || passengerList[j+1] == nullptr) continue; 
                if (passengerList[j]->name > passengerList[j + 1]->name) {
                    Passenger* temp = passengerList[j];
                    passengerList[j] = passengerList[j + 1];
                    passengerList[j + 1] = temp;
                }
            }
        }
        cout << ">> Sorting Complete!" << endl;
        
        // 自动调用带分页的 Manifest
        displayManifest();
    }

    // [Member 3] Waitlist
    void addToWaitlist(string id, string name, string fclass) override {
        cout << "[Array] Waitlist is handled by Linked List system mostly." << endl;
    }
};