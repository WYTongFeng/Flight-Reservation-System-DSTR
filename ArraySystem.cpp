#include "FlightCommon.hpp"
#include <iostream>
#include <iomanip> // 用于 setw 格式化输出
#include <string>

using namespace std;

// 定义飞机的物理参数 (根据 CSV 数据分析得出)
const int MAX_ROWS = 30; // 假设飞机有 30 排
const int MAX_COLS = 6;  // A-F 共 6 列
const int MAX_CAPACITY = 200; // 这里的容量设小一点，或者设为 10000 (看 CSV 大小)
// 注意：如果 CSV 有 10000 人，但飞机只有 180 个座位，这里实际上只能装 180 人。

class ArraySystem : public FlightSystem {
private:
    // [Member 1 核心数据结构] 2D Array - 用于直观的座位图
    // 存储 PassengerID，如果为空则存 "---"
    string seatMap[MAX_ROWS][MAX_COLS];

    // [Member 2 核心数据结构] 1D Array - 用于名单管理和线性搜索
    // 为了符合 "No vector" 规则，我们用原生指针数组
    Passenger* passengerList[MAX_CAPACITY]; 
    int currentCount; // 当前乘客数量

    // [Helper] 将列号 "A"-"F" 转换为索引 0-5
    int getColIndex(string col) {
        if (col == "A") return 0;
        if (col == "B") return 1;
        if (col == "C") return 2;
        if (col == "D") return 3;
        if (col == "E") return 4;
        if (col == "F") return 5;
        return -1; // 无效列
    }

public:
    // 构造函数：初始化数组
    ArraySystem() {
        currentCount = 0;
        
        // 1. 初始化 2D 座位图 (全部设为 Empty)
        for (int i = 0; i < MAX_ROWS; i++) {
            for (int j = 0; j < MAX_COLS; j++) {
                seatMap[i][j] = "---";
            }
        }

        // 2. 初始化 1D 列表 (全部设为 nullptr)
        for (int i = 0; i < MAX_CAPACITY; i++) {
            passengerList[i] = nullptr;
        }

        cout << ">> Array System Initialized (Max Rows: " << MAX_ROWS << ", Max Cols: " << MAX_COLS << ")" << endl;
    }

    // 析构函数：释放内存
    ~ArraySystem() {
        for (int i = 0; i < currentCount; i++) {
            delete passengerList[i]; // 释放每一个 new 出来的乘客对象
        }
    }

    // =========================================================
    // [Member 1 Task] Reservation (Insertion) - 预订座位
    // 负责：坐标转换 + 冲突检测 + 填入 2D 数组 + 填入 1D 数组
    // =========================================================
    void addPassenger(string id, string name, int row, string col, string fclass) override {
        // 1. 检查是否满员 (针对 1D 数组)
        if (currentCount >= MAX_CAPACITY) {
            cout << ">> [Error] System Full! Cannot add " << name << endl;
            // 这里其实可以调用 addToWaitlist，但那是 Member 3 的事
            return;
        }

        // 2. 验证座位坐标是否合法 (Error Handling)
        int rIndex = row - 1; // 转换为 0-based 索引
        int cIndex = getColIndex(col);

        if (rIndex < 0 || rIndex >= MAX_ROWS || cIndex == -1) {
            cout << ">> [Error] Invalid Seat: " << row << col << " (Out of bounds)" << endl;
            return;
        }

        // 3. 检查座位是否已被占用 (Collision Detection)
        if (seatMap[rIndex][cIndex] != "---") {
            cout << ">> [Error] Seat " << row << col << " is already occupied by " << seatMap[rIndex][cIndex] << "!" << endl;
            // 可以在这里调用 addToWaitlist(id, name, fclass);
            return;
        }

        // 4. 执行预订 (核心逻辑)
        
        // A. 创建新乘客对象
        Passenger* newP = new Passenger;
        newP->passengerID = id;
        newP->name = name;
        newP->seatRow = row;
        newP->seatCol = col;
        newP->flightClass = fclass;
        // Array 系统不用 next/prev 指针，保持 nullptr 即可

        // B. 存入 1D 数组 (Manifest)
        passengerList[currentCount] = newP;
        currentCount++;

        // C. 存入 2D 数组 (Seat Map)
        seatMap[rIndex][cIndex] = id; // 在地图上标记 ID

        cout << ">> [Array] Booked " << name << " at Seat " << row << col << endl;
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

    // =========================================================
    // [Member 1 Task] Display Seating Map - 打印可视化座位图
    // 负责：双层循环遍历 2D 数组并美化输出
    // =========================================================
    void displaySeatingMap() override {
        cout << "\n==============================================" << endl;
        cout << "           FLIGHT SEATING MAP (2D ARRAY)      " << endl;
        cout << "==============================================" << endl;
        cout << "      A      B      C      D      E      F" << endl;
        
        for (int i = 0; i < MAX_ROWS; i++) {
            // 打印行号 (例如 "01 ", "02 ")
            cout << setw(2) << setfill('0') << (i + 1) << "  ";
            
            // 内层循环：打印每一列
            for (int j = 0; j < MAX_COLS; j++) {
                string status = seatMap[i][j];
                // 如果是空位，显示 [---]
                // 如果有人，显示 ID (或者可以显示 [OCC] 节省空间)
                // 为了美观，我们检查一下长度，如果 ID 太长可以截断，或者用占位符
                if (status == "---") {
                    cout << "[---]  ";
                } else {
                    cout << "[" << "OCC" << "]  "; // 显示 OCC 表示有人
                }
            }
            cout << endl; // 换行
        }
        cout << setfill(' '); // 恢复默认填充
        cout << "==============================================" << endl;
    }

    // =========================================================
    // [Member 4 Task] Manifest & Report - 打印名单
    // =========================================================
    void displayManifest() override {
        cout << "\n---------------- ARRAY MANIFEST -----------------" << endl;
        cout << left << setw(10) << "ID" 
             << left << setw(20) << "Name" 
             << left << setw(10) << "Seat" 
             << left << setw(15) << "Class" << endl;
        cout << "-------------------------------------------------" << endl;

        if (currentCount == 0) {
            cout << "[System Empty]" << endl;
        } else {
            for (int i = 0; i < currentCount; i++) {
                string fullSeat = to_string(passengerList[i]->seatRow) + passengerList[i]->seatCol;
                cout << left << setw(10) << passengerList[i]->passengerID
                     << left << setw(20) << passengerList[i]->name
                     << left << setw(10) << fullSeat
                     << left << setw(15) << passengerList[i]->flightClass << endl;
            }
        }
    }

    // [Member 4] Sorting - Bubble Sort
    void sortAlphabetically() override {
        // 简单的冒泡排序，按 Name 排序
        for (int i = 0; i < currentCount - 1; i++) {
            for (int j = 0; j < currentCount - i - 1; j++) {
                if (passengerList[j]->name > passengerList[j + 1]->name) {
                    // 交换指针
                    Passenger* temp = passengerList[j];
                    passengerList[j] = passengerList[j + 1];
                    passengerList[j + 1] = temp;
                }
            }
        }
        cout << ">> [Array] Sorted by Name." << endl;
    }

    // [Member 3] Waitlist
    void addToWaitlist(string id, string name, string fclass) override {
        cout << "[Array] Waitlist is handled by Linked List system mostly." << endl;
    }
};