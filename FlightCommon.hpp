#ifndef FLIGHTCOMMON_HPP
#define FLIGHTCOMMON_HPP

#include <string>
#include <iostream>

using namespace std;

// ==========================================
// 1. 乘客数据结构 (Data Model)
// 统一标准：所有人都必须用这个结构体
// ==========================================
struct Passenger {
    // --- 基础数据 (来自 CSV) ---
    string passengerID; // e.g., "P1001"
    string name;        // e.g., "John Doe"
    int seatRow;        // e.g., 1
    string seatCol;     // e.g., "A"
    string flightClass; // e.g., "Economy"

    // --- 链表专用指针 (Array组请忽略，初始化为 nullptr) ---
    
    // 1. 用于 Singly Linked List (候补名单 Waitlist)
    // 2. 也用于 Doubly Linked List (主名单) 的 "后一个"
    Passenger* next = nullptr; 

    // 1. 仅用于 Doubly Linked List (主名单) 的 "前一个"
    // 2. Waitlist 候补名单不需要用这个 (保持为 nullptr)
    Passenger* prev = nullptr; 
};

// ==========================================
// 2. 系统接口 (System Interface)
// 抽象基类：强制 Array 和 Linked List 组实现相同的功能
// ==========================================
class FlightSystem {
public:
    // --- 核心功能 (每人负责 2 个) ---

    // [Function 1] Reservation (Insertion)
    // Array: 填入二维数组
    // LL: 插入双向链表 (Doubly Insert)
    virtual void addPassenger(string id, string name, int row, string col, string fclass) = 0;

    // [Function 2] Cancellation (Deletion)
    // Array: 清空格子 + 1D数组移位 (Shift)
    // LL: 双向链表节点断开 (Doubly Unlink) + 内存释放
    virtual bool removePassenger(string id) = 0;

    // [Function 3] Seat Lookup (Search)
    // ⚠️ 规则: 必须都使用 Linear Search (线性查找) 以保持对比公平
    virtual Passenger* searchPassenger(const string& id) = 0;

    // [Function 4] Manifest & Report
    // 打印乘客名单 (包含主名单 + 候补名单)
    virtual void displayManifest() = 0;
    
    // 打印可视化座位图 (Array组的高光时刻: 2D Grid)
    virtual void displaySeatingMap() = 0;

    // --- 额外算法与特殊要求 ---

    // [Requirement] Sorting Algorithm
    // ⚠️ 规则: 必须都使用 Bubble Sort (冒泡排序) 按名字 A-Z 排序
    virtual void sortAlphabetically() = 0;

    // [Requirement] Singly Linked List (候补名单)
    // 专门为了满足老师 "Use Singly and Doubly" 的要求
    // 当飞机满座时，调用此函数加入单向链表
    virtual void addToWaitlist(string id, string name, string fclass) = 0;

    // [Safety] 虚析构函数
    // 确保程序关闭时，内存被正确清理 (避免 Memory Leak)
    virtual ~FlightSystem() {}
};

#endif