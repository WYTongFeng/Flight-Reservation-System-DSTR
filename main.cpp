#include <iostream>
#include <string>
#include <limits> // 用于 numeric_limits

// 引入我们的核心头文件
#include "FlightCommon.hpp"

// 引入子系统的头文件 (虽然还没写完，但先include进来)
// 注意：你需要确保这两个 .cpp 文件里至少有空类定义，否则报错
#include "ArraySystem.cpp"
#include "LinkedListSystem.cpp"

using namespace std;

// 辅助函数：显示菜单
void showMenu() {
    cout << "\n=== Flight Reservation System ===" << endl;
    cout << "1. Switch to ARRAY System" << endl;
    cout << "2. Switch to LINKED LIST System" << endl;
    cout << "3. Exit Program" << endl;
    cout << "===============================" << endl;
    cout << "Select Option: ";
}

// 辅助函数：子系统菜单
void showSubMenu(string systemName) {
    cout << "\n--- " << systemName << " Operations ---" << endl;
    cout << "1. Add Passenger (Reservation)" << endl;
    cout << "2. Remove Passenger (Cancellation)" << endl;
    cout << "3. Search Passenger" << endl;
    cout << "4. Display Manifest & Seat Map" << endl;
    cout << "5. Sort Alphabetically" << endl;
    cout << "6. Add to Waitlist (Simulation)" << endl;
    cout << "0. Back to Main Menu" << endl;
    cout << "-------------------------------" << endl;
    cout << "Select Operation: ";
}

// 驱动函数：处理具体的业务逻辑
void runSystem(FlightSystem* sys, string name) {
    int choice;
    do {
        showSubMenu(name);
        if (!(cin >> choice)) { // 防止输入字符导致死循环
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }

        string id, pname, seatCol, fclass;
        int row;

        switch (choice) {
            case 1: // Add
                cout << "Enter ID: "; cin >> id;
                cout << "Enter Name: "; cin.ignore(); getline(cin, pname); // 允许名字带空格
                cout << "Enter Row (1-100): "; cin >> row;
                cout << "Enter Col (A-F): "; cin >> seatCol;
                cout << "Enter Class: "; cin >> fclass;
                sys->addPassenger(id, pname, row, seatCol, fclass);
                break;
            case 2: // Remove
                cout << "Enter Passenger ID to remove: "; cin >> id;
                if (sys->removePassenger(id)) cout << ">> Removed successfully.\n";
                else cout << ">> Passenger not found.\n";
                break;
            case 3: // Search
                cout << "Enter Passenger ID to search: "; cin >> id;
                {
                    Passenger* p = sys->searchPassenger(id);
                    if (p) cout << ">> Found: " << p->name << " at " << p->seatRow << p->seatCol << endl;
                    else cout << ">> Not found.\n";
                }
                break;
            case 4: // Manifest
                sys->displayManifest();
                sys->displaySeatingMap();
                break;
            case 5: // Sort
                sys->sortAlphabetically();
                cout << ">> Sorted successfully.\n";
                sys->displayManifest(); // 排序后立即显示
                break;
            case 6: // Waitlist
                cout << "Enter ID: "; cin >> id;
                cout << "Enter Name: "; cin.ignore(); getline(cin, pname);
                cout << "Enter Class: "; cin >> fclass;
                sys->addToWaitlist(id, pname, fclass);
                break;
            case 0:
                cout << "Returning..." << endl;
                break;
            default:
                cout << "Invalid option!" << endl;
        }
    } while (choice != 0);
}

int main() {
    // 创建两个系统的实例
    // 注意：这两个类必须在 ArraySystem.cpp 和 LinkedListSystem.cpp 里定义好
    FlightSystem* arraySys = new ArraySystem();
    FlightSystem* listSys = new LinkedListSystem();

    int mainChoice;
    while (true) {
        showMenu();
        if (!(cin >> mainChoice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (mainChoice == 1) {
            runSystem(arraySys, "ARRAY BASED");
        } 
        else if (mainChoice == 2) {
            runSystem(listSys, "LINKED LIST BASED");
        } 
        else if (mainChoice == 3) {
            cout << "Exiting..." << endl;
            break;
        } 
        else {
            cout << "Invalid choice!" << endl;
        }
    }

    // 清理内存
    delete arraySys;
    delete listSys;
    return 0;
}