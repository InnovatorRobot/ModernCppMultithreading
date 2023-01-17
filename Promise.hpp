#ifndef PROMISE_HPP
#define PROMISE_HPP
#include <iostream>
#include <mutex>
#include <thread>
#include <future>
#include <algorithm>
#include <chrono>

void findOdd(std::promise<int64_t>&& oddSumPromise, int64_t start, int64_t end)
{
    int64_t oddSum{};
    for (int64_t i = start; i <= end; ++i)
    {
        if (i & 1)
        {
            oddSum += i;
        }
    }
    oddSumPromise.set_value(oddSum);
}

void testPromise()
{
    int64_t start{};
    int64_t end{190000000};
    std::promise<int64_t> promise;
    auto future = promise.get_future();

    std::thread thread(findOdd, std::move(promise), start, end);
    std::cout << future.get() << "\n";
    thread.join();
}
#endif // PROMISE_HPP
