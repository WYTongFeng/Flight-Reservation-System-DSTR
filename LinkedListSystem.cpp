#include "FlightCommon.hpp"
#include <iostream>

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

    // Display (简单的打印，方便调试)
    void displayManifest() override {
        cout << "\n--- Main Passenger List (Doubly LL) ---" << endl;
        Passenger* current = head;
        if (!current) cout << "(Empty)" << endl;
        while (current != nullptr) {
            cout << "[" << current->passengerID << "] " << current->name 
                 << " | Seat: " << current->seatRow << current->seatCol << endl;
            current = current->next;
        }

        cout << "\n--- Waitlist (Singly LL) ---" << endl;
        Passenger* w = waitlistHead;
        if (!w) cout << "(Empty)" << endl;
        while (w != nullptr) {
            cout << "[WL] " << w->name << " (" << w->flightClass << ")" << endl;
            w = w->next;
        }
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