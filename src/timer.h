#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
private:
    std::chrono::steady_clock::time_point begin, end;

public:
    void start();
    void stop();

    std::chrono::milliseconds::rep duration();
};

#endif /* TIMER_H */