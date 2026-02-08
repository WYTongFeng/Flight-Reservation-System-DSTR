#ifndef TIMER_HPP
#define TIMER_HPP

#include <iostream>
#include <chrono>
#include <windows.h> // Required for Windows memory tracking
#include <psapi.h>   // Required for Windows memory tracking

using namespace std;
using namespace std::chrono;

class Timer {
private:
    high_resolution_clock::time_point start_time;
    high_resolution_clock::time_point end_time;

public:
    Timer() { start_time = high_resolution_clock::now(); }

    void start() { start_time = high_resolution_clock::now(); }
    void stop() { end_time = high_resolution_clock::now(); }

    // Measures current RAM usage of the program in Kilobytes [cite: 90, 114]
    long long getMemoryUsageKB() {
        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
        return pmc.WorkingSetSize / 1024;
    }

    // Helper to display theoretical complexity for your documentation 
    void printComplexity(string algorithmType) {
        cout << "\n--- Theoretical Complexity Analysis ---" << endl;
        if (algorithmType == "BubbleSort") {
            cout << "Time Complexity: O(N^2)" << endl;
            cout << "Space Complexity: O(1)" << endl;
        } else if (algorithmType == "LinearSearch") {
            cout << "Time Complexity: O(N)" << endl;
            cout << "Space Complexity: O(1)" << endl;
        } else if (algorithmType == "ArrayInsert") {
            cout << "Time Complexity: O(1)" << endl;
            cout << "Space Complexity: O(N*M) static allocation" << endl;
        }
    }

    long long getDurationInMicroseconds() {
        auto duration = duration_cast<microseconds>(end_time - start_time);
        return duration.count();
    }
};
#endif