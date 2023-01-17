#ifndef MEASURESPEEDUP_HPP
#define MEASURESPEEDUP_HPP
#include <iostream>
#include <future>
uint64_t sequentialSum(uint32_t lo, uint32_t hi)
{
    uint64_t sum{};
    for (auto i = lo; i < hi; ++i)
    {
        sum += i;
    }
    return sum;
}

uint64_t parallelSum(uint32_t lo, uint32_t hi, uint8_t depth = 0)
{
    if (depth > 3)
    {
        uint64_t sum{};
        for (auto i = lo; i < hi; ++i)
        {
            sum += i;
        }
        return sum;
    }
    else
    {
        auto mid = (hi + lo) / 2;
        auto left = std::async(std::launch::async, parallelSum, lo, mid, depth + 1);
        auto right = parallelSum(mid, hi, depth + 1);
        return left.get() + right;
    }
}

void testSpeedUp()
{
    const int numEvalRuns{10};
    const int sumValues{1000000};

    std::cout << "evaluating sequential implementation...\n";
    std::chrono::duration<double> sequentialTime{0};
    auto sequentialResult = sequentialSum(0, sumValues); // warm-up

    for (int i = 0; i < numEvalRuns; i++)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        sequentialSum(0, sumValues);
        sequentialTime += std::chrono::high_resolution_clock::now() - startTime;
    }
    sequentialTime /= numEvalRuns;

    std::cout << "evaluating parallel implementation...\n";
    std::chrono::duration<double> parallelTime(0);
    auto parallelResult = parallelSum(0, sumValues); // warm-up

    for (int i = 0; i < numEvalRuns; i++)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        parallelSum(0, sumValues);
        parallelTime += std::chrono::high_resolution_clock::now() - startTime;
    }
    parallelTime /= numEvalRuns;

    // display sequential and parallel results for comparison
    if (sequentialResult != parallelResult)
    {
        printf(
            "ERROR: Result mismatch!\n  Sequential Result = %lu\n  Parallel Result = %lu\n", sequentialResult,
            parallelResult);
    }

    printf("Average Sequential Time: %.1f ms\n", sequentialTime.count() * 1000);
    printf("  Average Parallel Time: %.1f ms\n", parallelTime.count() * 1000);
    printf("Speedup: %.2f\n", sequentialTime / parallelTime);
    auto numThread = std::thread::hardware_concurrency();
    printf("Efficiency %.2f%%\n", 100 * (sequentialTime / parallelTime) / std::thread::hardware_concurrency());
}
#endif // MEASURESPEEDUP_HPP
