
#include "blockingqueue.h"
#include "threadpool.h"


ThreadPool::ThreadPool(int thread_num) {   // 防止隐式转换问题
    task_queue_ = std::make_unique<BlockingQueue<std::function<void()>>>();
    for (size_t i = 0; i < thread_num; ++i) {
        workers_.emplace_back([this] -> void { Worker(); });
        // 每个线程执行 Worker 函数
    }
}


ThreadPool::~ThreadPool() {
    task_queue_->Cancel();  // 解除阻塞在当前队列的线程
    for (auto &worker : workers_) {
        if (worker.joinable()) {
            worker.join();  // 等待线程结束
        }
    }
}

void ThreadPool::Post(std::function<void()> task) {
    task_queue_->Push(task);
}

void ThreadPool::Worker() {
    while (true) {
        std::function<void()> task;
        if (!task_queue_->Pop(task)) {
            break;  // 非阻塞模式下，队列为空，退出线程
        }
        task();  // 执行任务
    }
}