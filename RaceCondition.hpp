#ifndef RACECONDITION_HPP
#define RACECONDITION_HPP
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iostream>
#include <chrono>
#include <barrier>
/// Prove of race condition

int numberOfChips{1};
std::mutex pencil;
void cpuWork(int time)
{
    std::this_thread::sleep_for(std::chrono::microseconds(time));
}

void mohiShoper()
{
    cpuWork(1);
    std::scoped_lock<std::mutex> lock(pencil);
    numberOfChips *= 2;
    std::cout << "Mohi doubled chips\n";
}
void samanShoper()
{
    cpuWork(2);
    std::scoped_lock<std::mutex> lock(pencil);
    numberOfChips += 3;
    std::cout << "Saman added 3 chips\n";
}

void proveRaceCondition()
{
    std::thread shopers[10];
    for (int counter{}; counter < 10; counter += 2)
    {
        shopers[counter] = std::thread(mohiShoper);
        shopers[counter + 1] = std::thread(samanShoper);
    }

    for (auto& shoper : shopers)
    {
        shoper.join();
    }
    std::cout << "We need " << numberOfChips << " chips\n";
}

/// Solving the Race Condition by Barrier!
///

auto onCompletion = []() noexcept
{
    // locking not needed here
    static auto phase = "... done\n"
                        "Cleaning up...\n";
    std::cout << phase;
    phase = "... done\n";
};

std::barrier syncPoint(10, onCompletion);
void mohiShoperBarrier()
{
    syncPoint.arrive_and_wait();
    cpuWork(2);
    std::scoped_lock<std::mutex> lock(pencil);
    numberOfChips *= 2;
    std::cout << "Mohi doubled chips\n";
}
void samanShoperBarrier()
{
    cpuWork(1);
    {
        std::scoped_lock<std::mutex> lock(pencil);
        numberOfChips += 3;
    }
    std::cout << "Saman added 3 chips\n";
    syncPoint.arrive_and_wait();
}

void SolveRaceCondition()
{
    std::thread shopers[10];
    for (int counter{}; counter < 10; counter += 2)
    {
        shopers[counter] = std::thread(mohiShoperBarrier);
        shopers[counter + 1] = std::thread(samanShoperBarrier);
    }

    for (auto& shoper : shopers)
    {
        shoper.join();
    }
    std::cout << "We need " << numberOfChips << " chips\n";
}

#endif // RACECONDITION_HPP
