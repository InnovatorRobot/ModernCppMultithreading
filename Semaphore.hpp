#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP
/// Mutex vs Semaphore
/// Mutex can only be acquired and release by the same thread
///
/// Semaphore can be acquired and release by different threads
///
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <chrono>

class Semaphore
{
public:
    explicit Semaphore(uint32_t count) : count_(count) {}
    void acquire()
    {
        std::unique_lock<std::mutex> ulock(mutex_);
        while (!count_)
        {
            semaphoreCondition_.wait(ulock);
        }
        --count_;
    }

    void release()
    {
        std::unique_lock<std::mutex> ulock(mutex_);
        ++count_;
        ulock.unlock();
        semaphoreCondition_.notify_one();
    }

private:
    uint32_t count_{};
    std::mutex mutex_;
    std::condition_variable semaphoreCondition_;
};

Semaphore semaphore{4};

void cellPhone(int id)
{
    semaphore.acquire();
    std::cout << "Phone " << id << " is chargin...\n";
    srand(id);
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 2000 + 1000));
    std::cout << "Phone " << id << " is done charging...\n";
    semaphore.release();
}

void testSemaphore()
{
    std::thread phones[10];
    int counter{};
    for (auto& phone : phones)
    {
        phone = std::thread(cellPhone, counter);
        ++counter;
    }

    for (auto& phone : phones)
    {
        phone.join();
    }
}
#endif // SEMAPHORE_HPP
