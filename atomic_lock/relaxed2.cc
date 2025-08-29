#include <atomic>
#include <thread>
#include <iostream>
#include <unistd.h>

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x() {
    sleep(1);
    x.store(true, std::memory_order_relaxed);
}

void write_y() {
    sleep(1);
    y.store(true, std::memory_order_relaxed);
}

void read_x_then_y() {
    while (!x.load(std::memory_order_relaxed))
        ;
    if (y.load(std::memory_order_relaxed))
        ++z;
}

void read_y_then_x() {
    while (!y.load(std::memory_order_relaxed))
        ;
    if (x.load(std::memory_order_relaxed))
        ++z;
}

int main() {
    int zero_count = 0;
    for (int i = 0; i < 1000; i++) {
        x = false;
        y = false;
        z = 0;
        
        std::thread a(write_x);
        std::thread b(write_y);
        std::thread c(read_x_then_y);
        std::thread d(read_y_then_x);
        
        a.join();
        b.join();
        c.join();
        d.join();
        
        std::cout << z.load() << " ";
    }
    
    // std::cout << "Total iterations with z=0: " << zero_count << std::endl;
    return 0;
}