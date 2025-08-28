#include <iostream>
#include "shared_ptr.h"
#include <thread>
#include <vector>
#include <chrono>
#include <memory>

void test_shared_ptr_thread_safety() {
    shared_ptr<int> ptr(new int(42));

    // 创建多个线程，每个线程都增加和减少引用计数
    const int num_threads = 10;
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&ptr]() {
            for (int j = 0; j < 10000; ++j) {
                shared_ptr<int> local_ptr(ptr);
                // 短暂暂停，增加线程切换的可能性
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }

    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }

    // 检查引用计数是否正确
    std::cout << "use_count: " << ptr.use_count() << std::endl;
    if (ptr.use_count() == 1) {
        std::cout << "Test passed: shared_ptr is thread-safe!" << std::endl;
    } else {
        std::cout << "Test failed: shared_ptr is not thread-safe!" << std::endl;
    }
}

// 测试代码
int main() {
    shared_ptr<int> ptr1(new int(10));
    std::cout << "ptr1 use_count: " << ptr1.use_count() << std::endl;  // 1

    {
        shared_ptr<int> ptr2 = ptr1;
        std::cout << "ptr1 use_count: " << ptr1.use_count() << std::endl;  // 2
        std::cout << "ptr2 use_count: " << ptr2.use_count() << std::endl;  // 2
    }

    std::cout << "ptr1 use_count: " << ptr1.use_count() << std::endl;  // 1

    shared_ptr<int> ptr3(new int(20));
    ptr1 = ptr3;
    std::cout << "ptr1 use_count: " << ptr1.use_count() << std::endl;  // 2
    std::cout << "ptr3 use_count: " << ptr3.use_count() << std::endl;  // 2

    ptr1.reset();
    std::cout << "ptr1 use_count: " << ptr1.use_count() << std::endl;  // 0
    std::cout << "ptr3 use_count: " << ptr3.use_count() << std::endl;  // 1

    test_shared_ptr_thread_safety();
    return 0;
}