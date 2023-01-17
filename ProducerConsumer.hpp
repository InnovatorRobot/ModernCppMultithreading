#ifndef PRODUCERCONSUMER_HPP
#define PRODUCERCONSUMER_HPP
#include <queue>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
template<typename Type>
class ServingLine
{
public:
    ServingLine() {}
    void serveSoup(Type i)
    {
        std::unique_lock<std::mutex> lockGaurd(mLock);
        soupQueue.push(i);
        lockGaurd.unlock();
        soupServed.notify_one();
    }

    Type takeSoup()
    {
        std::unique_lock<std::mutex> lockGaurd(mLock);
        while (soupQueue.empty())
        {
            soupServed.wait(lockGaurd);
        }
        auto bowl = soupQueue.front();
        soupQueue.pop();
        return bowl;
    }

private:
    std::queue<Type> soupQueue;
    std::mutex mLock;
    std::condition_variable soupServed;
};

auto servingLine = ServingLine<int>();

void soupProducer()
{
    int counnter{0};
    while (counnter < 10000)
    {
        servingLine.serveSoup(counnter);
        ++counnter;
    }
    servingLine.serveSoup(-1);
    std::cout << "Producer is done serving soup!\n";
}

void soupConsumer()
{
    int soupEaten{};
    while (true)
    {
        auto bowl = servingLine.takeSoup();
        if (bowl == -1)
        {
            std::cout << "Consumer are " << soupEaten << " bowls of soup\n";
            servingLine.serveSoup(-1);
            return;
        }
        else
        {
            ++soupEaten;
        }
    }
}

void testProducerConsumer()
{
    std::thread olivia(soupProducer);
    std::thread barron(soupConsumer);
    std::thread steve(soupConsumer);
    olivia.join();
    barron.join();
    steve.join();
}

std::mutex mutexProducerConsumer_;
std::condition_variable conditionalVariable_;
std::deque<int> buffer_;
constexpr int bufferSize_{50};
void producer(int val)
{
    while (val > 0)
    {
        std::unique_lock lock(mutexProducerConsumer_);
        conditionalVariable_.wait(lock, []() { return buffer_.size() < bufferSize_; });
        buffer_.push_back(val);
        std::cout << "Producer value is: " << val << "\n";
        val--;
        lock.unlock();
        conditionalVariable_.notify_one();
    }
    std::unique_lock lock(mutexProducerConsumer_);
    conditionalVariable_.wait(lock, []() { return buffer_.size() < bufferSize_; });
    buffer_.push_back(-1);
    lock.unlock();
    conditionalVariable_.notify_one();
}

void consumer()
{
    while (true)
    {
        std::unique_lock lock(mutexProducerConsumer_);
        conditionalVariable_.wait(lock, []() { return buffer_.size() > 0; });
        auto const val = buffer_.back();
        buffer_.pop_back();
        if (val == -1)
        {
            std::cout << "Consumer END\n";
            break;
        }
        std::cout << "Consumer value is: " << val << "\n";
        lock.unlock();
        conditionalVariable_.notify_one();
    }
}

void testProducerConsumer2()
{
    std::thread t1(producer, 100);
    std::thread t2(consumer);
    t1.join();
    t2.join();
}
#endif // PRODUCERCONSUMER_HPP
