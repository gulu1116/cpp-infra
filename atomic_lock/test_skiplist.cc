#include "skiplist.h"

#include <algorithm>
#include <chrono>
#include <initializer_list>
#include <iostream>
#include <thread>
#include <mutex>

using namespace ROCKSDB_NAMESPACE;

class IntComparator {
public:
    int operator() (const int &a, const int &b) const {
        return a-b;
    }
};

using OrderList = SkipList<int, IntComparator>;

OrderList glist{IntComparator{}};
std::mutex mtx;
std::atomic<int> quit{0};

void dump() {
    OrderList::Iterator it{&glist};
    std::cout << std::this_thread::get_id();
    for (it.SeekToFirst(); it.Valid(); it.Next()) {
        std::cout << " " << it.key();
    }
    std::cout << std::endl;
}

// 写 加锁  读不加锁
void worker_write(std::initializer_list<int> list) {
    for (auto it = list.begin(); it != list.end(); it++) {
        std::lock_guard<std::mutex> guard{mtx};
        glist.Insert(*it);
    }
    quit.fetch_add(1);
}

void worker_read() {
    std::this_thread::sleep_for(std::chrono::microseconds(0));
    while (quit.load() != 3) {
        std::this_thread::sleep_for(std::chrono::microseconds(0));
        dump();
    }
    dump();
}

int main() {
    std::thread a(worker_write, std::initializer_list<int>{2,9,30,3,7});
    std::thread b(worker_write, std::initializer_list<int>{110,1,6,34,87});
    std::thread c(worker_write, std::initializer_list<int>{88,4,26,64,13});
    std::thread d(worker_read);
    std::thread e(worker_read);
    a.join();
    b.join();
    c.join();
    d.join();
    e.join();
    return 0;
}
