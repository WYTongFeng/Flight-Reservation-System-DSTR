#include "FlightCommon.hpp"
#include <iostream>
#include <iomanip> // 必须加这个才能用 setw (设置宽度)

using namespace std;

class LinkedListSystem : public FlightSystem {
private:
    // 主名单 (双向链表)
    Passenger* head;
    Passenger* tail;

    // 候补名单 (单向链表) - Leader Week 7 Task [cite: 23]
    Passenger* waitlistHead;
    Passenger* waitlistTail;

public:
    LinkedListSystem() {
        head = nullptr;
        tail = nullptr;
        waitlistHead = nullptr;
        waitlistTail = nullptr;
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
        Passenger* newP = new Passenger;
        newP->passengerID = id;
        newP->name = name;
        newP->seatRow = row;
        newP->seatCol = col;
        newP->flightClass = fclass;
        
        newP->next = nullptr;
        newP->prev = nullptr;

        if (head == nullptr) {
            // 链表是空的
            head = newP;
            tail = newP;
        } else {
            // 插入到末尾 (Append)
            tail->next = newP;   // 旧尾巴指向新节点
            newP->prev = tail;   // 新节点指回旧尾巴
            tail = newP;         // 更新尾巴指针
        }
        cout << ">> [LL] Passenger " << name << " added to Main List." << endl;
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

// [Week 7 Task] Manifest: 打印完整名单 (美化版)
void displayManifest() override {
    // 1. 打印表头 (Header)
    cout << "\n==============================================================" << endl;
    cout << "               FLIGHT MANIFEST (LINKED LIST)                  " << endl;
    cout << "==============================================================" << endl;

    // set: 设置, w: width (宽度)
    // left: 让文字靠左对齐 (默认是靠右)
    cout << left << setw(10) << "ID" 
         << left << setw(20) << "Name" 
         << left << setw(10) << "Seat" 
         << left << setw(15) << "Class" << endl;
    cout << "--------------------------------------------------------------" << endl;

    // 2. 准备开始遍历主名单
    Passenger* current = head; // 定义一个指针，像手指一样指向链表的头(第一个人)

    if (current == nullptr) {
        // 如果头是空的，说明没乘客
        cout << "[System Empty] No passengers in Main List." << endl;
    } else {
        // 3. 循环遍历 (While Loop)
        while (current != nullptr) {
            // 拼凑座位号: 比如 Row 1 和 Col "A" -> 变成 "1A"
            string fullSeat = to_string(current->seatRow) + current->seatCol;

            // 打印这一行的数据，保持和表头一样的宽度
            cout << left << setw(10) << current->passengerID
                 << left << setw(20) << current->name
                 << left << setw(10) << fullSeat
                 << left << setw(15) << current->flightClass << endl;
            
            // 4. 关键步骤：指针后移 (Move to next)
            current = current->next; 
        }
    }

    // 5. 打印候补名单 (Waitlist)
    cout << "\n------------------------- WAITLIST ---------------------------" << endl;
    Passenger* w = waitlistHead; // 让手指指向候补名单的头

    if (w == nullptr) {
        cout << "(Waitlist is Empty)" << endl;
    } else {
        while (w != nullptr) {
             cout << left << setw(10) << w->passengerID
                 << left << setw(20) << w->name
                 << left << setw(10) << "WAITING" // 候补没有座位，写死 WAITING
                 << left << setw(15) << w->flightClass << endl;
            
            w = w->next; // 别忘了移动指针！否则会死循环
        }
    }
    cout << "==============================================================" << endl;
}

    // Leader 不需要负责 Map，留空
    void displaySeatingMap() override {
        cout << "[LL] Map not available (Array team's job)." << endl;
    }

    // Sorting 先留空，Week 8 Member 4 才做
    void sortAlphabetically() override {
        cout << "[LL] Sort not implemented yet." << endl;
    }
};