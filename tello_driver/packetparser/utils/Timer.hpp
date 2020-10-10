#pragma once
#include <chrono>
#include <iostream>

struct Timer
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> duration;

    Timer()
    {
        start = std::chrono::high_resolution_clock::now();
    }
    ~Timer()
    {
        end = std::chrono::system_clock::now();
        duration = end - start;

        double ms = duration.count() * 1000.0f;
        std::cout << "Timer took: " << ms << "ms " << '\n';
    }
};
