#pragma once

/*
#pragma once 类似：
#ifndef _HEADER_NAME_H_
#define _HEADER_NAME_H_
// 头文件内容
#endif
*/

// #include <blockingqueue.h>
#include <thread>
#include <functional>
#include <vector>

// 前置声明
// blockingqueue 仅仅只能用作指针或引用
template<typename T>
class BlockingQueue;


class ThreadPool {
public:
    // 初始化线程池
    // ThreadPool tp = 12
    // explicit 也就是防止上面的赋值情况，只允许 ThreadPool tp = ThreadPool(12);
    explicit ThreadPool(int thread_num);

    // 停止线程池
    ~ThreadPool();
    // 提交任务到线程池
    // 任务是一个函数对象，可以是 lambda 表达式或 std::function
    // 这里的 Post 函数是非阻塞的
    // 如果任务队列已满，则直接返回，不会阻塞等待
    void Post(std::function<void()> task);

private:
    void Worker();

    // std::queue<std::function<void()>> queue_;
    // 这里队列是临界资源，要加锁，上面的方式不是线程安全的
    std::unique_ptr<BlockingQueue<std::function<void()>>> task_queue_;   // 阻塞队列
    std::vector<std::thread> workers_;                  // 线程集合
};