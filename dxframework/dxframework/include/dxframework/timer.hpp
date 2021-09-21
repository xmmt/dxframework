#pragma once

#include <chrono>

namespace dxframework {

class timer {
public:
    timer();
    double get_miliseconds_elapsed();
    void restart();
    bool stop();
    bool start();

private:
    bool is_running_ = false;
    std::chrono::time_point<std::chrono::steady_clock> start_;
    std::chrono::time_point<std::chrono::steady_clock> stop_;
};

} // namespace dxframework