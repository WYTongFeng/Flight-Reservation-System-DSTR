#ifndef TIMER_HPP
#define TIMER_HPP

#include <iostream>
#include <chrono> // C++ 标准时间库，用于高精度计时

using namespace std;
using namespace std::chrono;

class Timer {
private:
    // 记录开始时间点
    high_resolution_clock::time_point start_time;
    // 记录结束时间点
    high_resolution_clock::time_point end_time;

public:
    // 构造函数：创建对象时自动开始计时
    Timer() {
        start_time = high_resolution_clock::now();
    }

    // 功能：手动重置/开始计时
    void start() {
        start_time = high_resolution_clock::now();
    }

    // 功能：停止计时
    void stop() {
        end_time = high_resolution_clock::now();
    }

    // 功能：获取流逝的时间（微秒 Microseconds）- 精度更高，适合测快速操作
    long long getDurationInMicroseconds() {
        auto duration = duration_cast<microseconds>(end_time - start_time);
        return duration.count();
    }

    // 功能：获取流逝的时间（毫秒 Milliseconds）
    long long getDurationInMilliseconds() {
        auto duration = duration_cast<milliseconds>(end_time - start_time);
        return duration.count();
    }
    
    // 析构函数
    ~Timer() {}
};

#endif