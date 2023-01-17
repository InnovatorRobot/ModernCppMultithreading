#ifndef MUTEX_HPP
#define MUTEX_HPP
#include <mutex>
#include <iostream>
#include <thread>
#include <shared_mutex>

///////////////////////////////////////////////////////////////////////////////////
/// \brief The RecursiveMutex class
///
///
class RecursiveMutex
{
public:
    void func1()
    {
        std::lock_guard<std::recursive_mutex> gaurd(rMutex_);
        shared_ = "func1";
        std::cout << "in fun1, shared variable is now " << shared_ << " threadID: " << std::this_thread::get_id()
                  << '\n';
    }
    void func2()
    {
        std::lock_guard<std::recursive_mutex> gaurd(rMutex_);
        shared_ = "func2";
        std::cout << "in fun2, shared variable is now " << shared_ << " threadID: " << std::this_thread::get_id()
                  << '\n';
        func1(); // recursive lock becomes useful here
        std::cout << "back in fun2, shared variable is " << shared_ << " threadID: " << std::this_thread::get_id()
                  << '\n';
    }

private:
    std::recursive_mutex rMutex_;
    std::string shared_;
};

///////////////////////////////////////////////////////////////////////////////////
/// \brief The ThreadSafeCounter class
///
///
class ThreadSafeCounter
{
public:
    ThreadSafeCounter() = default;

    // Multiple threads/readers can read the counter's value at the same time.
    unsigned int get() const
    {
        std::shared_lock lock(mutex_);
        return value_;
    }

    // Only one thread/writer can increment/write the counter's value.
    void increment()
    {
        std::unique_lock lock(mutex_);
        ++value_;
    }

    // Only one thread/writer can reset/write the counter's value.
    void reset()
    {
        std::unique_lock lock(mutex_);
        value_ = 0;
    }

private:
    mutable std::shared_mutex mutex_;
    unsigned int value_ = 0;
};

///////////////////////////////////////////////////////////////////////////////////
/// \brief The TimeMutex class
///
///
class TimeMutex
{
public:
    void operator()(int i)
    {
        if (m_.try_lock_for(std::chrono::seconds(2)))
        {
            ++counter_;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "Thread " << i << " Entered!\n";
            m_.unlock();
        }
        else
        {
            std::cout << "Thread " << i << " Could NOT Entered!\n";
        }
    }

private:
    static inline std::timed_mutex m_;
    inline static int counter_{};
};

///////////////////////////////////////////////////////////////////////////////////
/// \brief The UniqueLock class
///
///
class UniqueLock
{
public:
    void defaultUniqueLock(int loopFor)
    {
        std::unique_lock lock(m_);
        for (int i = 0; i < 10; ++i)
        {
            buffer_++;
            std::cout << std::this_thread::get_id() << " " << buffer_ << "\n";
        }
    }

    void deferLockUniqueLock(int loopFor)
    {
        std::unique_lock lock(m_, std::defer_lock);

        lock.lock();
        for (int i = 0; i < 10; ++i)
        {
            buffer_++;
            std::cout << std::this_thread::get_id() << " " << buffer_ << "\n";
        }
    }

    void tryLockUniqueLock(int loopFor)
    {
        std::unique_lock lock(m_, std::try_to_lock);
        for (int i = 0; i < 10; ++i)
        {
            buffer_++;
            std::cout << std::this_thread::get_id() << " " << buffer_ << "\n";
        }
    }

private:
    static inline int buffer_{};
    static inline std::mutex m_;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void testRecursiveMutex()
{
    RecursiveMutex x;
    std::thread t1(&RecursiveMutex::func1, &x);
    std::thread t2(&RecursiveMutex::func2, &x);
    t1.join();
    t2.join();
}

void testSharedMutex()
{
    ThreadSafeCounter counter;

    auto increment_and_print = [&counter]()
    {
        for (int i = 0; i < 3; i++)
        {
            counter.increment();
            std::cout << std::this_thread::get_id() << ' ' << counter.get() << '\n';

            // Note: Writing to std::cout actually needs to be synchronized as well
            // by another std::mutex. This has been omitted to keep the example small.
        }
    };

    std::thread thread1(increment_and_print);
    std::thread thread2(increment_and_print);

    thread1.join();
    thread2.join();
}

void testTimeMutex()
{
    TimeMutex timeMutex1;
    TimeMutex timeMutex2;
    std::thread t1(std::ref(timeMutex1), 1);
    std::thread t2(std::ref(timeMutex2), 2);
    t1.join();
    t2.join();
}
#endif // MUTEX_HPP
