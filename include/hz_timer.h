/**
 * HZ Timer 2.0
 * For making a code segment is executed under a specified Hertz rate
 *
 * Author: Chester Abrahams
 * GitHub: https://github.com/ChesterOfTheSemester/HZ-Timer
 */

#ifndef HZ_TIMER
#define HZ_TIMER

#include <iostream>
#include <map>
#include <chrono>

struct {
    std::map<int, std::pair<int, int>> data;

    bool test(int hz, int uuid = 0, int timespan = 1000) {
        const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        const int threshold = timespan / hz;

        auto &entry = data[uuid];

        if (entry.first == 0) {
            entry.first = now;
            entry.second = -1;
        }

        const int elapsed = now - entry.first;

        if (elapsed > threshold) {
            entry.first = now - (elapsed % threshold);
            entry.second = (entry.second + 1) & 0xFFF;
            return entry.second >= 1;
        }

        return false;
    }
} hz_timer;

#endif
