#ifndef TIMER_HPP
#define TIMER_HPP

#include <iostream>
#include <chrono>
#include <windows.h> 
#include <psapi.h>   

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

    // This is the missing function causing your error
    long long getDurationInMilliseconds() {
        auto duration = duration_cast<milliseconds>(end_time - start_time);
        return duration.count();
    }

    long long getDurationInMicroseconds() {
        auto duration = duration_cast<microseconds>(end_time - start_time);
        return duration.count();
    }

    long long getMemoryUsageKB() {
        PROCESS_MEMORY_COUNTERS_EX pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
            return pmc.WorkingSetSize / 1024;
        }
        return 0;
    }

    void printComplexity(string algorithmType) {
        cout << "\n--- Theoretical Complexity Analysis ---" << endl;
        if (algorithmType == "BubbleSort") {
            cout << "Time Complexity: O(N^2)" << endl;
            cout << "Space Complexity: O(1)" << endl;
        } else if (algorithmType == "LinearSearch") {
            cout << "Time Complexity: O(N)" << endl;
            cout << "Space Complexity: O(1)" << endl;
        }
    }
};

#endif