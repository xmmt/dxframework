#include <dxframework/timer.hpp>

using namespace dxframework;

timer::timer() {
    start_ = std::chrono::high_resolution_clock::now();
    stop_ = std::chrono::high_resolution_clock::now();
}

double timer::get_miliseconds_elapsed() {
    if (is_running_) {
        auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start_);
        return elapsed.count();
    } else {
        auto elapsed = std::chrono::duration<double, std::milli>(stop_ - start_);
        return elapsed.count();
    }
}

void timer::restart() {
    is_running_ = true;
    start_ = std::chrono::high_resolution_clock::now();
}

bool timer::stop() {
    if (!is_running_)
        return false;
    else {
        stop_ = std::chrono::high_resolution_clock::now();
        is_running_ = false;
        return true;
    }
}

bool timer::start() {
    if (is_running_) {
        return false;
    } else {
        start_ = std::chrono::high_resolution_clock::now();
        is_running_ = true;
        return true;
    }
}