#include <atomic>
#include <thread>
#include <iostream>

std::atomic<int> x{0};

void thread_func1()
{
    for (int i = 0; i < 100000; ++i)
    {
        x.store(i, std::memory_order_relaxed);
    }
}

void thread_func2()
{
    for (int i = 0; i < 100000; ++i)
    {
        x.store(-i, std::memory_order_relaxed);
    }
}

int main()
{
    std::thread t1(thread_func1);
    std::thread t2(thread_func2);

    t1.join();
    t2.join();

    std::cout << "Final value of x = " << x.load(std::memory_order_relaxed) << std::endl;

    return 0;
}
