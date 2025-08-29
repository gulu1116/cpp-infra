#include "MPSCQueue.h"

#include <thread>

#include <iostream>

struct Count {
    Count(int _v) : v(_v){}
    int v;
};

int main() {
    MPSCQueue<Count> queue;
    std::thread pd1([&]() {
        queue.Enqueue(new Count(100));
        queue.Enqueue(new Count(200));
        queue.Enqueue(new Count(300));
        queue.Enqueue(new Count(400));
    });

    std::thread pd2([&]() {
        queue.Enqueue(new Count(500));
        queue.Enqueue(new Count(600));
        queue.Enqueue(new Count(700));
        queue.Enqueue(new Count(800));
    });

    std::thread cs1([&]() {
        Count* ele;
        while(queue.Dequeue(ele)) {
            std::cout << std::this_thread::get_id() << " : pop " << ele->v << std::endl;
            delete ele;
        }
    });

    pd1.join();
    pd2.join();
    cs1.join();
    
    return 0;
}