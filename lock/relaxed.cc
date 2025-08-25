#include <atomic>
#include <thread>
#include <assert.h>
#include <iostream>

std::atomic<bool> x,y;
std::atomic<int> z;

void write_x_then_y()
{
    x.store(true,std::memory_order_relaxed);  // 1
    y.store(true,std::memory_order_relaxed);  // 2
}

void read_y_then_x()
{
    while(!y.load(std::memory_order_relaxed));  // 3
    if(x.load(std::memory_order_relaxed))  // 4
        ++z;
}
// z 会不会等于 1？  不一定

int main()
{
    for (int i=0; i < 100000; i++) {
        x=false;
        y=false;
        z=0;
        std::thread b(read_y_then_x);
        std::thread a(write_x_then_y);
        b.join();
        a.join();
        int v = z.load(std::memory_order_relaxed);
        if (v != 1)
            std::cout << v << std::endl;
    }
    return 0;
}