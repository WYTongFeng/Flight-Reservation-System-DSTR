#include "FlightCommon.hpp"
#include <iostream>
#include <iomanip> // 必须加这个才能用 setw (设置宽度)
#include <chrono>
using namespace std::chrono;


using namespace std;

class LinkedListSystem : public FlightSystem {
private:
    // 主名单 (双向链表)
    Passenger* head;
    Passenger* tail;

    // 候补名单 (单向链表) - Leader Week 7 Task [cite: 23]
    Passenger* waitlistHead;
    Passenger* waitlistTail;

    int nextIdCounter;

public:
    LinkedListSystem() {
        head = nullptr;
        tail = nullptr;
        waitlistHead = nullptr;
        waitlistTail = nullptr;
        nextIdCounter = 100000;
        cout << ">> Linked List System Initialized (Leader Ready)." << endl;
    }

    // 析构函数：清理内存 (防止内存泄漏) [cite: 58]
    ~LinkedListSystem() {
        // 清理主名单
        Passenger* current = head;
        while (current != nullptr) {
            Passenger* nextNode = current->next;
            delete current;
            current = nextNode;
        }
        // 清理候补名单
        current = waitlistHead;
        while (current != nullptr) {
            Passenger* nextNode = current->next;
            delete current;
            current = nextNode;
        }
    }

    // [Week 7 Task] 1. Doubly LL Insert (主名单加人) [cite: 22]
    void addPassenger(string id, string name, int row, string col, string fclass) override {

    auto start_time = high_resolution_clock::now();

    // --- ID 逻辑：CSV 同步 + 手动自动生成 ---
    string finalID;
    if (!id.empty()) {
        finalID = id;
        try {
            int idNum = stoi(id);
            if (idNum >= nextIdCounter) nextIdCounter = idNum + 1;
        } catch (...) {}
    } else {
        finalID = to_string(nextIdCounter++);
    }

    Passenger* newP = new Passenger;
    newP->passengerID = finalID;
    newP->name = name;
    newP->seatRow = row;
    newP->seatCol = col;
    newP->flightClass = fclass;

    newP->next = nullptr;
    newP->prev = nullptr;

    // --- Doubly linked list append ---
    if (head == nullptr) {
        head = newP;
        tail = newP;
    } else {
        tail->next = newP;
        newP->prev = tail;
        tail = newP;
    }

    auto stop_time = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop_time - start_time);

    // --- 只有手动 add 才显示 stats ---
    if (id.empty()) {
        size_t memoryUsed = sizeof(Passenger) + sizeof(Passenger*); 
        cout << "----------------------------------------------------" << endl;
        cout << "✅ [LL] Passenger Added Successfully!" << endl;
        cout << "   [+] Assigned ID  : " << finalID << endl;
        cout << "   [+] Time Taken   : " << duration.count() << " us (Linear Search)" << endl;
        cout << "   [+] Memory Used  : " << memoryUsed << " bytes" << endl;
        cout << "----------------------------------------------------" << endl;
    } else {
        cout << ">> [LL] Passenger " << name << " added to Main List." << endl;
    }
}


    // [Week 7 Task] 2. Doubly LL Delete (主名单删人 + 自动候补转正) [cite: 21, 24]
    bool removePassenger(string id) override {
        if (head == nullptr) return false;

        Passenger* current = head;
        while (current != nullptr) {
            if (current->passengerID == id) {
                // 找到了，准备删除
                
                // 1. 处理指针断开逻辑
                if (current == head && current == tail) { // 只有这一个人
                    head = nullptr;
                    tail = nullptr;
                } else if (current == head) { // 删除头
                    head = head->next;
                    head->prev = nullptr;
                } else if (current == tail) { // 删除尾
                    tail = tail->prev;
                    tail->next = nullptr;
                } else { // 删除中间节点
                    current->prev->next = current->next;
                    current->next->prev = current->prev;
                }

                // 记录下座位信息，留给候补的人用
                int freedRow = current->seatRow;
                string freedCol = current->seatCol;

                delete current; // 释放内存
                cout << ">> [LL] Passenger " << id << " removed." << endl;

                // [Integration] 检查候补名单，自动转正 
                promoteFromWaitlist(freedRow, freedCol);
                
                return true;
            }
            current = current->next;
        }
        return false; // 没找到
    }

    // [Helper] 候补转正逻辑 (Leader 特有功能)
    void promoteFromWaitlist(int freeRow, string freeCol) {
        if (waitlistHead != nullptr) {
            // 取出候补名单的第一个人
            Passenger* luckyPerson = waitlistHead;
            
            // 从候补名单移除
            waitlistHead = waitlistHead->next;
            if (waitlistHead == nullptr) {
                waitlistTail = nullptr;
            }

            // 加入主名单 (复用 addPassenger)
            cout << ">> [Waitlist] Promoting " << luckyPerson->name << " to Seat " << freeRow << freeCol << "!" << endl;
            addPassenger(luckyPerson->passengerID, luckyPerson->name, freeRow, freeCol, luckyPerson->flightClass);
            
            // 释放掉原来的候补节点 (因为 addPassenger 会 new 一个新的)
            delete luckyPerson; 
        }
    }

    // [Week 7 Task] 3. Waitlist Logic (候补名单加人) [cite: 23]
    void addToWaitlist(string id, string name, string fclass) override {
        Passenger* newP = new Passenger;
        newP->passengerID = id;
        newP->name = name;
        newP->flightClass = fclass;
        // 候补名单不需要座位号
        newP->seatRow = 0;
        newP->seatCol = "Wait";

        newP->next = nullptr;
        newP->prev = nullptr; // 单向链表不需要 prev，但初始化一下也没事

        if (waitlistHead == nullptr) {
            waitlistHead = newP;
            waitlistTail = newP;
        } else {
            waitlistTail->next = newP;
            waitlistTail = newP;
        }
        cout << ">> [LL] Passenger " << name << " added to Waitlist." << endl;
    }

    // [Week 7 Task] Search (Member 4 负责，但我先写个简单的遍历以免报错) [cite: 29]
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

// =========================================================
    // [MANIFEST] Text Report with Interactive Pagination (LL Version)
    // 完全复刻 ArraySystem 的 UI 和交互逻辑
    // =========================================================
    void displayManifest() override {
        // 1. 统计总人数 (LL 必须先遍历一次)
        int currentCount = 0;
        Passenger* temp = head;
        while (temp != nullptr) {
            currentCount++;
            temp = temp->next;
        }

        if (currentCount == 0) {
            cout << "[System Empty]" << endl;
            return;
        }

        // 2. 计算页数
        int totalPages = (currentCount + FlightGlobal::MANIFEST_PER_PAGE - 1) / FlightGlobal::MANIFEST_PER_PAGE;
        int currentPage = 1;

        // 3. 交互循环 (While True)
        while (true) {
            cout << string(50, '\n'); // 清屏模拟

            cout << "==============================================================" << endl;
            cout << "             PASSENGER MANIFEST (LINKED LIST)                 " << endl;
            cout << "==============================================================" << endl;
            cout << left << setw(10) << "ID" 
                 << left << setw(25) << "Name" 
                 << left << setw(10) << "Seat" 
                 << left << setw(15) << "Class" << endl;
            cout << "--------------------------------------------------------------" << endl;

            // --- 链表定位逻辑 (核心不同点) ---
            int startIndex = (currentPage - 1) * FlightGlobal::MANIFEST_PER_PAGE;
            int endIndex = startIndex + FlightGlobal::MANIFEST_PER_PAGE;
            if (endIndex > currentCount) endIndex = currentCount;

            // 让指针跳到当前页的起点 (Skip)
            Passenger* current = head;
            for (int i = 0; i < startIndex; i++) {
                if (current != nullptr) current = current->next;
            }

            // 打印这一页的数据
            for (int i = startIndex; i < endIndex; i++) {
                if (current == nullptr) break;
                
                string fullSeat = to_string(current->seatRow) + current->seatCol;
                cout << left << setw(10) << current->passengerID 
                     << left << setw(25) << current->name 
                     << left << setw(10) << fullSeat
                     << left << setw(15) << current->flightClass << endl;
                
                current = current->next;
            }

            // --- 底部菜单 (完全一致) ---
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
    // [SEAT MAP] Visual Map with Interactive Pagination (LL Version)
    // 完全复刻 ArraySystem 的 UI 和交互逻辑
    // =========================================================
    void displaySeatingMap() override {
        // 1. 找出最大行数 (模拟 Array 的 maxRows)
        int maxRowFound = 0;
        Passenger* p = head;
        while (p != nullptr) {
            if (p->seatRow > maxRowFound) maxRowFound = p->seatRow;
            p = p->next;
        }
        // 至少显示 20 行，与 ArraySystem 逻辑保持一致
        if (maxRowFound < 20) maxRowFound = 20;

        // 2. 构建临时地图 (因为链表查坐标太慢，必须用临时数组辅助)
        string** tempMap = new string*[maxRowFound];
        for (int i = 0; i < maxRowFound; i++) {
            tempMap[i] = new string[FlightGlobal::COLS];
            for (int j = 0; j < FlightGlobal::COLS; j++) tempMap[i][j] = "EMPTY"; // 默认 EMPTY
        }

        // 填入数据
        p = head;
        while (p != nullptr) {
            int r = p->seatRow - 1;
            int c = FlightGlobal::getColIndex(p->seatCol);
            if (r >= 0 && r < maxRowFound && c != -1) {
                tempMap[r][c] = p->name; // 存名字
            }
            p = p->next;
        }

        // 3. 计算 lastActiveRow (完全复刻 Array 逻辑)
        int lastActiveRow = 0;
        for(int i = maxRowFound - 1; i >= 0; i--) {
            bool empty = true;
            for(int j=0; j<FlightGlobal::COLS; j++) {
                if(tempMap[i][j] != "EMPTY") { empty = false; break; }
            }
            if(!empty) { lastActiveRow = i + 1; break; }
        }
        if (lastActiveRow < 20) lastActiveRow = 20;

        // 4. 分页计算
        int totalPages = (lastActiveRow + FlightGlobal::ROWS_PER_PAGE - 1) / FlightGlobal::ROWS_PER_PAGE;
        int currentPage = 1;

        // 5. 交互循环
        while (true) {
            cout << string(50, '\n'); 

            cout << "========================================================================================================================" << endl;
            cout << "                                              FLIGHT SEATING MAP (LINKED LIST)                                          " << endl;
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
                    string display = FlightGlobal::formatName(tempMap[i][j]); 
                    string finalStr = "[" + display + "]";
                    cout << left << setw(FlightGlobal::COL_WIDTH) << finalStr;
                    if (j == 2) cout << "    "; // 过道
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

        // 清理临时内存
        for (int i = 0; i < maxRowFound; i++) delete[] tempMap[i];
        delete[] tempMap;

        cout << ">> Exiting Map View..." << endl;
    }

// [Member 4 Task] Sorting - Bubble Sort (Week 8)
    // 使用 "Data Swap" (只换数据，不换节点) 的方式，最安全
    void sortAlphabetically() override {
        // 1. 安全检查：如果链表是空的，或者只有1个人，那就不需要排序
        cout << ">>> [DEBUG] LinkedListSystem::sortAlphabetically() CALLED\n";

        if (head == nullptr || head->next == nullptr) {
            cout << ">> [LL] List is empty or has only 1 passenger. No sort needed." << endl;
            return;
        }

        bool swapped;          // 标记：这一轮有没有发生过交换？
        Passenger* ptr1;       // 游标：当前指着谁
        Passenger* lptr = nullptr; // 终点标记：每一轮冒泡后，最大的泡泡会跑到最后，下次就不用比它了

        cout << ">> [LL] Sorting Linked List by Name (Bubble Sort)..." << endl;

        // 2. 冒泡排序主循环 (do-while)
        do {
            swapped = false; // 每一轮开始前，先假设没交换过
            ptr1 = head;     // 每次都从头开始跑

            // 内层循环：从头走到“终点”
            while (ptr1->next != lptr) {
                // 3. 比较名字 (A-Z)
                // 如果当前这个人的名字 > 下一个人的名字 (比如 "Jack" > "Alice")
                if (ptr1->name > ptr1->next->name) {
                    
                    // 4. 交换数据 (Swap Data Only)
                    // 也就是“交换名牌”，不动指针
                    
                    // A. 交换 ID
                    string tempID = ptr1->passengerID;
                    ptr1->passengerID = ptr1->next->passengerID;
                    ptr1->next->passengerID = tempID;

                    // B. 交换 Name
                    string tempName = ptr1->name;
                    ptr1->name = ptr1->next->name;
                    ptr1->next->name = tempName;

                    // C. 交换 Row
                    int tempRow = ptr1->seatRow;
                    ptr1->seatRow = ptr1->next->seatRow;
                    ptr1->next->seatRow = tempRow;

                    // D. 交换 Col
                    string tempCol = ptr1->seatCol;
                    ptr1->seatCol = ptr1->next->seatCol;
                    ptr1->next->seatCol = tempCol;

                    // E. 交换 Class
                    string tempClass = ptr1->flightClass;
                    ptr1->flightClass = ptr1->next->flightClass;
                    ptr1->next->flightClass = tempClass;

                    swapped = true; // 举手：报告！我刚才交换了一次！
                }
                ptr1 = ptr1->next; // 游标往后移，去比下一对
            }
            // 一轮跑完后，最后那个已经是最大的了，下次不用比它
            lptr = ptr1; 

        } while (swapped); // 只要这一轮发生了交换，说明还不够乱，继续跑下一轮。如果一轮下来都没交换，说明排好了。

        cout << ">> [LL] Sorting Complete." << endl;
        
        // 排序完后顺便打印一下名单给大家看
        displayManifest(); 
    }
};