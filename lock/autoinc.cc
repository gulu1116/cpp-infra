#include <chrono>
#include <iostream>
#include <thread>
#include <assert.h>

// #define USE_ATOMIC 0
#define USE_ATOMIC 1

#if USE_MUTEX
    #include <mutex>
    std::mutex mtx;
    int count = 0;
#elif USE_SPINLOCK
    #include "spinlock.h"
    using spinlock_t = struct spinlock;
    spinlock_t spin;
    int count = 0;
#elif USE_ATOMIC
    #include <atomic>
    std::atomic<int> count{0};
#else
    int count = 0;
#endif


void incrby(int num) {
    for (int i=0; i < num; i++) {
#if USE_MUTEX
        mtx.lock();
        ++count;
        mtx.unlock();
#elif USE_SPINLOCK
        spinlock_lock(&spin);
        ++count;
        spinlock_unlock(&spin);
#elif USE_ATOMIC
        // std::this_thread::sleep_for(std::chrono::milliseconds(1));
        count.fetch_add(1);
#else
        ++count;
#endif
    }
}

int main() {
#ifdef USE_SPINLOCK
        spinlock_init(&spin);
#endif
    for (int i = 0; i < 100; i++) {
#ifdef USE_ATOMIC
        count.store(0);
#else
        count = 0;
#endif
        std::thread a(incrby, 500);
        std::thread b(incrby, 500);
        std::thread c(incrby, 500);
        std::thread d(incrby, 500);
        a.join();
        b.join();
        c.join();
        d.join();
#ifdef USE_ATOMIC
        if (count.load() != 2000) {
#else
        if (count != 2000) {
#endif
            std::cout << "i:" << i << " count:" << count << std::endl;
            break;
        }
    }
    return 0;
}

