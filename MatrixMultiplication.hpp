#ifndef MATRIXMULTIPLICATION_HPP
#define MATRIXMULTIPLICATION_HPP
#include <iostream>
#include <thread>

#include <boost/asio.hpp>
size_t numberOfCores = 4;

void seqMatrixMulti(long** A, size_t aRowSize, size_t aColSize, long** B, size_t bRowSize, size_t bColSize, long** C)
{
    for (size_t i{}; i < aRowSize; ++i)
    {
        for (size_t j{}; j < bColSize; ++j)
        {
            C[i][j] = 0;
            for (size_t k{}; k < aRowSize; ++k)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void lastStep(long** A, long** B, long** C, size_t i, size_t j, size_t aRowSize)
{
    for (size_t k{}; k < aRowSize; ++k)
    {
        C[i][j] += A[i][k] * B[k][j];
    }
}

void parallelMatrixMulti(long** A, size_t aRowSize, size_t aColSize, long** B, size_t bRowSize, size_t bColSize, long** C)
{
    boost::asio::thread_pool pool(numberOfCores);
    int k = 0;
    for (size_t i{}; i < aRowSize; ++i)
    {
        for (size_t j{}; j < bColSize; ++j)
        {
            C[i][j] = 0;
            //            workers[k] = std::thread(lastStep, A, B, C, i, j, aRowSize);
            boost::asio::post(pool, [i, j, aRowSize, A, B, &C]() { lastStep(A, B, C, i, j, aRowSize); });
            //            ++k;
        }
    }
    pool.join();
}

void parallelWorker(
    long** A,
    size_t aRowSize,
    size_t aColSize,
    long** B,
    size_t bRowSize,
    size_t bColSize,
    long** C,
    size_t cRowStart,
    size_t cRowEnd)
{
    for (size_t i{cRowStart}; i < cRowEnd; ++i)
    {
        for (size_t j{}; j < bColSize; ++j)
        {
            C[i][j] = 0;
            for (size_t k{}; k < aRowSize; ++k)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}
void parallelMatrixMulti2(long** A, size_t aRowSize, size_t aColSize, long** B, size_t bRowSize, size_t bColSize, long** C)
{
    size_t chunk_size = ceil((float)aRowSize / numberOfCores);
    std::thread threads[numberOfCores];
    for (int i = 0; i < numberOfCores; ++i)
    {
        size_t start_row_c = std::min(i * chunk_size, aRowSize);
        size_t end_row_c = std::min((i + 1) * chunk_size, aRowSize);
        threads[i] =
            std::thread(parallelWorker, A, aRowSize, aColSize, B, bRowSize, bColSize, C, start_row_c, end_row_c);
    }
    for (auto& t : threads)
    {
        t.join();
    }
}

void testMatrixMulti()
{
    const int NUM_EVAL_RUNS = 3;
    const size_t NUM_ROWS_A = 1000;
    const size_t NUM_COLS_A = 1000;
    const size_t NUM_ROWS_B = NUM_COLS_A;
    const size_t NUM_COLS_B = 1000;

    // intialize A and B with values in range 1 to 100
    long** A = (long**)malloc(NUM_ROWS_A * sizeof(long));
    long** B = (long**)malloc(NUM_ROWS_B * sizeof(long));
    if ((A == NULL) || (A == NULL))
    {
        exit(2);
    }
    for (size_t i = 0; i < NUM_ROWS_A; i++)
    {
        A[i] = (long*)malloc(NUM_COLS_A * sizeof(long));
        B[i] = (long*)malloc(NUM_COLS_B * sizeof(long));
        if ((A[i] == NULL) || (A[i] == NULL))
        {
            exit(2);
        }
        for (size_t j = 0; j < NUM_COLS_A; j++)
        {
            A[i][j] = rand() % 100 + 1;
            B[i][j] = rand() % 100 + 1;
        }
    }

    // allocate arrays for sequential and parallel results
    long** sequential_result = (long**)malloc(NUM_ROWS_A * sizeof(long));
    long** parallel_result = (long**)malloc(NUM_ROWS_A * sizeof(long));
    if ((sequential_result == NULL) || (parallel_result == NULL))
    {
        exit(2);
    }
    for (size_t i = 0; i < NUM_ROWS_A; i++)
    {
        sequential_result[i] = (long*)malloc(NUM_COLS_B * sizeof(long));
        parallel_result[i] = (long*)malloc(NUM_COLS_B * sizeof(long));
        if ((sequential_result[i] == NULL) || (parallel_result[i] == NULL))
        {
            exit(2);
        }
    }

    printf("Evaluating Sequential Implementation...\n");
    std::chrono::duration<double> sequential_time(0);
    seqMatrixMulti(A, NUM_COLS_A, NUM_ROWS_A, B, NUM_COLS_B, NUM_ROWS_B, sequential_result); // "warm up"
    for (int i = 0; i < NUM_EVAL_RUNS; i++)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        seqMatrixMulti(A, NUM_COLS_A, NUM_ROWS_A, B, NUM_COLS_B, NUM_ROWS_B, sequential_result);
        sequential_time += std::chrono::high_resolution_clock::now() - startTime;
    }
    sequential_time /= NUM_EVAL_RUNS;

    printf("Evaluating Parallel Implementation...\n");
    std::chrono::duration<double> parallel_time(0);
    parallelMatrixMulti2(A, NUM_COLS_A, NUM_ROWS_A, B, NUM_COLS_B, NUM_ROWS_B, parallel_result); // "warm up"
    for (int i = 0; i < NUM_EVAL_RUNS; i++)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        parallelMatrixMulti2(A, NUM_COLS_A, NUM_ROWS_A, B, NUM_COLS_B, NUM_ROWS_B, parallel_result);
        parallel_time += std::chrono::high_resolution_clock::now() - startTime;
    }
    parallel_time /= NUM_EVAL_RUNS;

    // verify sequential and parallel results
    for (size_t i = 0; i < NUM_ROWS_A; i++)
    {
        for (size_t j = 0; j < NUM_COLS_B; j++)
        {
            if (sequential_result[i][j] != parallel_result[i][j])
            {
                printf(
                    "ERROR: Result mismatch at row %ld, col %ld! parallel_result = %ld   sequential_result = %ld\n", i,
                    j, parallel_result[i][j], sequential_result[i][j]);
            }
        }
    }
    printf("Average Sequential Time: %.2f ms\n", sequential_time.count() * 1000);
    printf("  Average Parallel Time: %.2f ms\n", parallel_time.count() * 1000);
    printf("Speedup: %.2f\n", sequential_time / parallel_time);
    printf("Efficiency %.2f%%\n", 100 * (sequential_time / parallel_time) / numberOfCores);
}
#endif // MATRIXMULTIPLICATION_HPP
