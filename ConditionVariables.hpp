#ifndef CONDITIONVARIABLES_HPP
#define CONDITIONVARIABLES_HPP
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
/// The condition variable has 3 main operational:
///     1: Wait
///     2: Signal
///     3: Broadcast -> similar to singnal but wake up everyone!
///

///
/// Note that these days (since, C++17) one should use std::scoped_lock instead of std::lock_guard.
///
///

/// Question:
///
/// A. I want to synchronise access to a queue for two threads.
/// B. I want to synchronise access to a queue for two threads and use a condition variable because one of the threads
/// will wait on content to be stored into the queue by the other thread.

///
/// The difference is that you can lock and unlock a std::unique_lock. std::lock_guard will be locked only once on
/// construction and unlocked on destruction.

/// So for use case B you definitely need a std::unique_lock for the condition variable. In case A it depends whether
/// you need to relock the guard.

/// std::unique_lock has other features that allow it to e.g.: be constructed without locking the mutex immediately but
/// to build the RAII wrapper (see here).

/// std::lock_guard also provides a convenient RAII wrapper, but cannot lock multiple mutexes safely. It can be used
/// when you need a wrapper for a limited scope, e.g.: a member function:
///
///
#include <optional>
int soup_servings{10};
std::mutex slow_cooker_lid;
std::condition_variable soup_taken;
bool ready{false};

std::mutex print_mutex;

void hungryPerson(int id)
{
    int put_lid_back = 0;
    while (soup_servings > 0)
    {
        std::unique_lock<std::mutex> lid_lock(slow_cooker_lid);
        if ((id == soup_servings % 2) && (soup_servings > 0))
        {
            soup_servings--;
        }
        else
        {
            put_lid_back++;
        }
    }
    std::lock_guard<std::mutex> scop_lock{print_mutex};
    std::cout << "Person " << id << " put the lid back " << put_lid_back << " times.\n";
}

void HungryPersonCodition(int id)
{
    int put_lid_back = 0;
    while (soup_servings > 0)
    {
        std::unique_lock<std::mutex> lid_lock(slow_cooker_lid);
        auto const condition = (id != soup_servings % 2) && (soup_servings > 0);
        /// In certain operating environments, the condition could have what is called a spurious wakeup,
        /// meaning it wakes up from its waiting state when it is not supposed to! By placing it inside the
        /// while loop, if the spurios wakeup occurs, the thread will see that it is still not time to
        /// continue on and it will go back to sleeping.
        while ((id != soup_servings % 2) && (soup_servings > 0))
        {
            put_lid_back++;
            soup_taken.wait(lid_lock);
        }
        if (soup_servings > 0)
        {
            soup_servings--;
            lid_lock.unlock();
            soup_taken.notify_one();
        }
    }
    std::cout << "Person " << id << " put the lid back " << put_lid_back << " times.\n";
}

std::mutex mutex_;
std::condition_variable conditionVariable_;
int balance{};
void addMoney(int money)
{
    std::unique_lock lock(mutex_);
    balance += money;
    std::cout << "Amount Added Curretn balance: " << balance << "\n";
    conditionVariable_.notify_one();
}

void withdrawMoney(int money)
{
    std::unique_lock lock(mutex_);
    conditionVariable_.wait(lock, []() { return balance == 0 ? true : false; });
    if (balance >= money)
    {
        balance -= money;
        std::cout << "Amount Deducted: " << money << "\n";
    }
    else
    {
        std::cout << "Amount cant be deducted, Curretn balance is less than: " << money << "\n";
    }
}
void testConditionVariable()
{
    std::thread hungry_threads[2];
    for (int i = 0; i < 2; i++)
    {
        hungry_threads[i] = std::thread(HungryPersonCodition, i);
    }
    for (auto& ht : hungry_threads)
    {
        ht.join();
    }
}

void testConditionVariable2()
{
    std::thread t1(addMoney, 600);
    std::thread t2(withdrawMoney, 500);
    t1.join();
    t2.join();
}

#endif // CONDITIONVARIABLES_HPP
