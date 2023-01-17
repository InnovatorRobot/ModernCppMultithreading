#ifndef MERGESORT_HPP
#define MERGESORT_HPP
#include <boost/asio/post.hpp>
#include <cmath>
#include <future>
#include <iostream>
#include <thread>
#include <vector>
#include <boost/asio/thread_pool.hpp>
using Matrix = std::vector<int>;
const auto Cores = std::thread::hardware_concurrency();
void merge(Matrix array, size_t left, size_t mid, size_t right)
{
    auto const subArrayOne = mid - left + 1;
    auto const subArrayTwo = right - mid;

    // Create temp arrays
    auto *leftArray = new int[subArrayOne], *rightArray = new int[subArrayTwo];

    // Copy data to temp arrays leftArray[] and rightArray[]
    for (auto i = 0; i < subArrayOne; i++)
        leftArray[i] = array[left + i];
    for (auto j = 0; j < subArrayTwo; j++)
        rightArray[j] = array[mid + 1 + j];

    auto indexOfSubArrayOne = 0,   // Initial index of first sub-array
        indexOfSubArrayTwo = 0;    // Initial index of second sub-array
    int indexOfMergedArray = left; // Initial index of merged array

    // Merge the temp arrays back into array[left..right]
    while (indexOfSubArrayOne < subArrayOne && indexOfSubArrayTwo < subArrayTwo)
    {
        if (leftArray[indexOfSubArrayOne] <= rightArray[indexOfSubArrayTwo])
        {
            array[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
            indexOfSubArrayOne++;
        }
        else
        {
            array[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
            indexOfSubArrayTwo++;
        }
        indexOfMergedArray++;
    }
    // Copy the remaining elements of
    // left[], if there are any
    while (indexOfSubArrayOne < subArrayOne)
    {
        array[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
        indexOfSubArrayOne++;
        indexOfMergedArray++;
    }
    // Copy the remaining elements of
    // right[], if there are any
    while (indexOfSubArrayTwo < subArrayTwo)
    {
        array[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
        indexOfSubArrayTwo++;
        indexOfMergedArray++;
    }
    delete[] leftArray;
    delete[] rightArray;
}

void sequentialMergeSort(Matrix array, size_t left, size_t right)
{
    if (left < right)
    {
        size_t mid{(right + left) / 2};
        sequentialMergeSort(array, left, mid);
        sequentialMergeSort(array, mid + 1, right);
        merge(array, left, mid, right);
    }
}

void parallelMergeSort(Matrix array, size_t left, size_t right, size_t depth = 0)
{
    if (depth >= std::log(Cores))
    {
        sequentialMergeSort(array, left, right);
    }
    else
    {
        if (left < right)
        {
            size_t mid{(right + left) / 2};
            std::thread thread = std::thread(parallelMergeSort, array, left, mid, depth + 1);
            parallelMergeSort(array, mid + 1, right, depth + 1);
            thread.join();
            merge(array, left, mid, right);
        }
    }
}

void testMergeSort()
{
    const int NUM_EVAL_RUNS = 100;
    const int N = 1000; // number of elements to sort

    Matrix original_array(N), sequential_result(N), parallel_result(N);
    for (int i = 0; i < N; i++)
    {
        original_array[i] = rand();
    }

    printf("Evaluating Sequential Implementation...\n");
    std::chrono::duration<double> sequential_time(0);
    std::copy(std::begin(original_array), std::end(original_array), sequential_result.begin());
    sequentialMergeSort(sequential_result, 0, N - 1); // "warm up"
    for (int i = 0; i < NUM_EVAL_RUNS; i++)
    {
        std::copy(std::begin(original_array), std::end(original_array), sequential_result.begin()); // reset result array
        auto start_time = std::chrono::high_resolution_clock::now();
        sequentialMergeSort(sequential_result, 0, N - 1);
        sequential_time += std::chrono::high_resolution_clock::now() - start_time;
    }
    sequential_time /= NUM_EVAL_RUNS;

    printf("Evaluating Parallel Implementation...\n");
    std::chrono::duration<double> parallel_time(0);
    std::copy(std::begin(original_array), std::end(original_array), parallel_result.begin());
    parallelMergeSort(parallel_result, 0, N - 1); // "warm up"
    for (int i = 0; i < NUM_EVAL_RUNS; i++)
    {
        std::copy(std::begin(original_array), std::end(original_array), parallel_result.begin());
        auto start_time = std::chrono::high_resolution_clock::now();
        parallelMergeSort(parallel_result, 0, N - 1);
        parallel_time += std::chrono::high_resolution_clock::now() - start_time;
    }
    parallel_time /= NUM_EVAL_RUNS;

    // verify sequential and parallel results are same
    for (int i = 0; i < N; i++)
    {
        if (sequential_result[i] != parallel_result[i])
        {
            printf("ERROR: Result mismatch at index %d!\n", i);
        }
    }
    printf("Average Sequential Time: %.2f ms\n", sequential_time.count() * 1000);
    printf("  Average Parallel Time: %.2f ms\n", parallel_time.count() * 1000);
    printf("Speedup: %.2f\n", sequential_time / parallel_time);
    printf("Efficiency %.2f%%\n", 100 * (sequential_time / parallel_time) / std::thread::hardware_concurrency());
}

#endif // MERGESORT_HPP
