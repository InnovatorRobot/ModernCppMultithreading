#ifndef LATCH_HPP
#define LATCH_HPP

/// Barrier VS Latch
///
/// Barrier: Releases when certain number of threads are waiting
///
/// Latch: Releases when count value reaches zero
///
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iostream>
#include <chrono>

int numberOfChipsLatch{1};
std::mutex pencilLatch;
void cpuWorkLatch(int time)
{
    std::this_thread::sleep_for(std::chrono::microseconds(time));
}

#include <latch>

std::latch latch{5};

void mohiShoperBarrierLatch()
{
    latch.wait();
    cpuWorkLatch(2);
    std::scoped_lock<std::mutex> lock(pencilLatch);
    numberOfChipsLatch *= 2;
    std::cout << "Mohi doubled chips\n";
}
void samanShoperBarrierLatch()
{
    cpuWorkLatch(1);
    {
        std::scoped_lock<std::mutex> lock(pencilLatch);
        numberOfChipsLatch += 3;
    }
    std::cout << "Saman added 3 chips\n";
    latch.count_down();
}

void SolveRaceConditionLatch()
{
    std::thread shopers[10];
    for (int counter{}; counter < 10; counter += 2)
    {
        shopers[counter] = std::thread(mohiShoperBarrierLatch);
        shopers[counter + 1] = std::thread(samanShoperBarrierLatch);
    }

    for (auto& shoper : shopers)
    {
        shoper.join();
    }
    std::cout << "We need " << numberOfChipsLatch << " chips\n";
}

////
///
/// Initialize count to 1
///     A simple on/off gate
///
///
/// Initialize count to N
///     Wait for N threads to complete some action
///     Wait for some action to complete N times
///
///
#endif // LATCH_HPP
