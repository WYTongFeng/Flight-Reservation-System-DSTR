#include "FlightCommon.hpp"
#include <iostream>
#include <vector> // 既然是 ArraySystem，之后你可能会用到 vector 或 array

using namespace std;

class ArraySystem : public FlightSystem {
public:
    // 构造函数
    ArraySystem() {
        cout << ">> Array System Initialized." << endl;
    }

    // [Function 1] Add Passenger
    void addPassenger(string id, string name, int row, string col, string fclass) override {
        // TODO: 写在这里：把数据存入你的数组
        cout << "[Array] Add Passenger not implemented yet." << endl;
    }

    // [Function 2] Remove Passenger
    bool removePassenger(string id) override {
        // TODO: 写在这里：从数组删除
        cout << "[Array] Remove Passenger not implemented yet." << endl;
        return false;
    }

    // [Function 3] Search Passenger
    Passenger* searchPassenger(const string& id) override {
        // TODO: 写在这里：线性查找
        cout << "[Array] Search Passenger not implemented yet." << endl;
        return nullptr;
    }

    // [Function 4] Display Manifest
    void displayManifest() override {
        cout << "[Array] Display Manifest not implemented yet." << endl;
    }

    // Display Map
    void displaySeatingMap() override {
        cout << "[Array] Seat Map not implemented yet." << endl;
    }

    // Sort
    void sortAlphabetically() override {
        cout << "[Array] Sort not implemented yet." << endl;
    }

    // Waitlist
    void addToWaitlist(string id, string name, string fclass) override {
        cout << "[Array] Waitlist not implemented yet." << endl;
    }
};