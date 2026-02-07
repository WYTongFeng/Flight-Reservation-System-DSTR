#include <iostream>
#include <string>
#include <limits> // 用于 numeric_limits
#include <fstream>
#include <sstream>
#include <vector>

#include <algorithm> // ✅ for transform
#include <cctype>    // ✅ for tolower/toupper

// 引入我们的核心头文件
#include "FlightCommon.hpp"
#include "Timer.hpp"

// 引入子系统的头文件 (虽然还没写完，但先include进来)
// 注意：你需要确保这两个 .cpp 文件里至少有空类定义，否则报错
#include "ArraySystem.cpp"
#include "LinkedListSystem.cpp"

using namespace std;

// ===============================
// ✅ Input Validation Helpers
// ===============================

// 把字符串转小写
string toLowerStr(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c){ return (char)tolower(c); });
    return s;
}

// 读取 row（强制 1-30）
int readRow_1to30() {
    int row;
    while (true) {
        cout << "Enter Row (1-30): ";
        if (cin >> row && row >= 1 && row <= 30) return row;

        cout << ">> [Error] Row must be between 1 and 30.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// 读取 col（强制 A-F，自动把 a->A）
string readCol_AtoF() {
    string col;
    while (true) {
        cout << "Enter Col (A-F): ";
        cin >> col;

        if (!col.empty()) {
            char c = (char)toupper((unsigned char)col[0]);
            if (c >= 'A' && c <= 'F') return string(1, c);
        }
        cout << ">> [Error] Column must be A, B, C, D, E, or F.\n";
    }
}

// 读取 class（强制 First/Business/Economy，接受 first/business/economic 等写法）
string readClass_FBE() {
    string fclass;
    while (true) {
        cout << "Enter Class (First/Business/Economy): ";
        cin >> fclass;

        string x = toLowerStr(fclass);

        if (x == "first") return "First";
        if (x == "business" || x == "busin") return "Business";
        if (x == "economy" || x == "economic" || x == "econo") return "Economy";

        cout << ">> [Error] Class must be First, Business, or Economy.\n";
    }
}

// ===============================
// 读取 CSV 数据
// ===============================
void loadData(FlightSystem* sys, string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // 跳过标题行

    int count = 0;
    while (getline(file, line)) {
        stringstream ss(line);
        string id, name, rowStr, col, fclass;

        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, rowStr, ',');
        getline(ss, col, ',');
        getline(ss, fclass, ',');

        // ✅ 去除 Windows \r，避免 col / class 变成 "A\r"
        if (!col.empty() && col.back() == '\r') col.pop_back();
        if (!fclass.empty() && fclass.back() == '\r') fclass.pop_back();

        if (!id.empty()) {
            try {
                int row = stoi(rowStr);
                sys->addPassenger(id, name, row, col, fclass); // CSV 旧数据
                count++;
            } catch (...) {
                // stoi 失败就跳过
            }
        }
    }

    cout << ">> Loaded " << count << " passengers from " << filename << endl;
    file.close();
}

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
    cout << "4. Display Seat Map" << endl;
    cout << "5. Display Manifest" << endl;
    cout << "6. Add to Waitlist (Simulation)" << endl;
    cout << "0. Back to Main Menu" << endl;
    cout << "-------------------------------" << endl;
    cout << "Select Operation: ";
}

// 驱动函数：处理具体的业务逻辑
void runSystem(FlightSystem* sys, string name) {
    int choice;
    string id, pname, seatCol, fclass;
    int row;

    cout << ">> Running system pointer=" << sys << " (" << name << ")\n"; // ✅ 加在这里

    do {
        showSubMenu(name);
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: { // ✅ Add (含输入验证)
                cout << "Enter Name: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, pname);

                row = readRow_1to30();
                seatCol = readCol_AtoF();
                fclass = readClass_FBE();

                sys->addPassenger("", pname, row, seatCol, fclass);
                break;
            }

            case 2: // Remove
                cout << "Enter Passenger ID to remove: ";
                cin >> id;
                if (sys->removePassenger(id)) cout << ">> Removed successfully.\n";
                else cout << ">> Passenger not found.\n";
                break;

            case 3: // Search
                cout << "Enter Passenger ID to search: ";
                cin >> id;
                {
                    Passenger* p = sys->searchPassenger(id);
                    if (p) cout << ">> Found: " << p->name << " at " << p->seatRow << p->seatCol << endl;
                    else cout << ">> Not found.\n";
                }
                break;

            case 4: // Display Map
                sys->displaySeatingMap();
                break;

            case 5: // Sort / Manifest
                sys->sortAlphabetically();
                break;

            case 6: { // Waitlist（这里也修掉 cin.ignore() 只忽略1个字的坑）
                cout << "Enter ID: ";
                cin >> id;

                cout << "Enter Name: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, pname);

                fclass = readClass_FBE();
                sys->addToWaitlist(id, pname, fclass);
                break;
            }

            case 0:
                cout << "Returning..." << endl;
                break;

            default:
                cout << "Invalid option!" << endl;
        }
    } while (choice != 0);
}

int main() {
    FlightSystem* arraySys = new ArraySystem();
    FlightSystem* listSys = new LinkedListSystem();

    cout << "arraySys=" << arraySys << "  listSys=" << listSys << endl;  // ✅ 加在这里

    string filename = "flight_passenger_data.csv.csv";

    cout << ">> Loading Array System from CSV..." << endl;
    loadData(arraySys, filename);

    cout << ">> Loading into Linked List System..." << endl;
    loadData(listSys, filename);

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

    delete arraySys;
    delete listSys;
    return 0;
}
