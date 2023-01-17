#ifndef FUTURE_HPP
#define FUTURE_HPP

/// Launching ASynchronous tasks is a great way to accomplish multiple things at once
///
/// Task1 needs to get the result back from the Task2 when it is done!
///
/// This is where a mechanism called future can be used!
///

#include <iostream>
#include <future>

auto howManyVegetables()
{
    std::cout << "Mohi is counting vegetables...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    return 42;
}

class Functor // Functor (Function Object)
{
public:
    int operator()(int x)
    {
        while (x-- > 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
        return 43;
    }
};

void testFuture()
{
    std::cout << "Saman asks Mohi how many vegetables are in ther pantry.\n";
    auto result = std::async(std::launch::async, howManyVegetables);
    std::cout << "Saman can do other things while he is waiting for the Mohi results...\n";
    std::cout << "Mohi responsed with " << result.get() << "\n";
}
#endif // FUTURE_HPP
