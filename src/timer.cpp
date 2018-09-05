#include "timer.h"

void Timer::start() {
    begin = std::chrono::steady_clock::now();
}

void Timer::stop() {
    end = std::chrono::steady_clock::now();
}

std::chrono::milliseconds::rep Timer::duration() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
}