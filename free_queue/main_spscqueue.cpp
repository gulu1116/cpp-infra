#include <iostream>
#include <thread>
#include <cassert>
#include "ringbuffer.h"

// 测试 RingBuffer 的基本功能
void TestBasicFunctionality() {
    RingBuffer<int, 8> buffer;

    // 测试写入和读取元素
    assert(buffer.Push(1));
    assert(buffer.Push(2));

    int value;
    assert(buffer.Pop(value) && value == 1);
    assert(buffer.Pop(value) && value == 2);

    // 测试缓冲区为空时的读取
    assert(!buffer.Pop(value));

    std::cout << "TestBasicFunctionality passed!" << std::endl;
}

// 测试 RingBuffer 在多线程环境下的表现
void TestMultiThreaded() {
    RingBuffer<int, 1024> buffer;
    const size_t num_items = 100000;

    auto producer = [&buffer, num_items]() {
        for (size_t i = 0; i < num_items; ++i) {
            while (!buffer.Push(i)) {
                // 等待缓冲区有空间
            }
        }
    };

    auto consumer = [&buffer, num_items]() {
        for (size_t i = 0; i < num_items; ++i) {
            int value;
            while (!buffer.Pop(value)) {
                // 等待缓冲区有数据
            }
            assert(value == i);
        }
    };

    std::thread producer_thread(producer);
    std::thread consumer_thread(consumer);

    producer_thread.join();
    consumer_thread.join();

    std::cout << "TestMultiThreaded passed!" << std::endl;
}

int main() {
    TestBasicFunctionality();
    TestMultiThreaded();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
