#ifndef DEVIDECONQUER_HPP
#define DEVIDECONQUER_HPP
#include <iostream>
#include <thread>
#include <future>
uint64_t sum(uint32_t lo, uint32_t hi, uint32_t depth = 0)
{
    if (depth > 3)
    {
        uint64_t res{};
        for (auto i = lo; i < hi; i++)
        {
            res += i;
        }
        return res;
    }
    else // devide and conquer
    {
        auto mid = (hi + lo) / 2;
        auto left = std::async(std::launch::async, sum, lo, mid, depth + 1);
        auto right = sum(mid, hi, depth + 1);
        return left.get() + right;
    }
}

void testDevideConquerFuture()
{
    std::cout << sum(0, 1000000000) << "\n";
}
#endif // DEVIDECONQUER_HPP
