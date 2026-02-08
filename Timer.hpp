#ifndef TIMER_HPP
#define TIMER_HPP

#include <iostream>
#include <chrono> // C++ Standard Time Library, used for high-precision timing

using namespace std;
using namespace std::chrono;

class Timer {
private:
    // Record start time point
    high_resolution_clock::time_point start_time;
    // Record end time point
    high_resolution_clock::time_point end_time;

public:
    // Constructor: Automatically start timing when object is created
    Timer() {
        start_time = high_resolution_clock::now();
    }

    // Function: Manually reset/start timing
    void start() {
        start_time = high_resolution_clock::now();
    }

    // Function: Stop timing
    void stop() {
        end_time = high_resolution_clock::now();
    }

    // Function: Get elapsed time (Microseconds) - Higher precision, suitable for measuring fast operations
    long long getDurationInMicroseconds() {
        auto duration = duration_cast<microseconds>(end_time - start_time);
        return duration.count();
    }

    // Function: Get elapsed time (Milliseconds)
    long long getDurationInMilliseconds() {
        auto duration = duration_cast<milliseconds>(end_time - start_time);
        return duration.count();
    }
    
    // Destructor
    ~Timer() {}
};

#endif // TIMER_HPP