#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

/// In some scenarios, rather than creating new thread for every single tasks, it can be more efficient to use a
/// ThreadPool, which creates and maintains a small collection of worker threads!
///
/// Manage the overhead of creating a new thread!
///

#include <iostream>
#include <thread>

#include <boost/asio.hpp>

void vegetableChopper(int id)
{
    std::cout << "thread " << std::this_thread::get_id() << " chopped vegetable " << id << "\n";
}

void testThreadWithoutPool()
{
    std::thread choppers[100];
    for (int i{0}; i < 100; i++)
    {
        choppers[i] = std::thread(vegetableChopper, i);
    }
    for (auto& thread : choppers)
    {
        thread.join();
    }
}

void testThreadPool()
{
    boost::asio::thread_pool pool(4);
    for (int i{0}; i < 100; i++)
    {
        boost::asio::post(pool, [i]() { vegetableChopper(i); });
    }
    pool.join();
}
#endif // THREADPOOL_HPP
