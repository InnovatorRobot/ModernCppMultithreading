#ifndef JTHREAD_HPP
#define JTHREAD_HPP
#include <iostream>
#include <thread>
#include <vector>
////
/// \brief testJThread1
/// if joinable() is true, calls request_stop() and then join(); in either case destructs the jthread object

void testJThread1()
{
    auto lambda = [](int x)
    {
        std::cout << "Hello from thread: " << std::this_thread::get_id() << "\n";
        std::cout << "Argument passed: " << x << "\n";
    };
    std::vector<std::jthread> threads;
    for (int i = 0; i < 10; i++)
    {
        threads.push_back(std::jthread(lambda, i));
    }
    std::cout << "Hello from the main thread\n";
}
#endif // JTHREAD_HPP
