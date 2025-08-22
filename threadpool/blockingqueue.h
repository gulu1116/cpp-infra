#pragma once

#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>
#include <mutex>

template <typename T>
class BlockingQueue {
public:
    // 阻塞队列的构造函数
    // nonblock_ 为 true 时，表示非阻塞模式
    // nonblock_ 为 false 时，表示阻塞模式
    explicit BlockingQueue(bool nonblock = false) : nonblock_(nonblock) {}

    // 入队操作
    void Push(const T &value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(value);
        not_empty_.notify_one();  // 通知一个等待的线程
    }
    // 正常 pop 弹出元素
    // 异常 pop 没有弹出元素
    bool Pop(T &value) {
        std::unique_lock<std::mutex> lock(mutex_);  // 可以手动 lock.unlock()

        // 队列不为空或非阻塞，唤醒一个等待的线程
        // 1. mutex_.unlock() 解除阻塞
        // 2. queue_.empty() && !nonblock_ 线程在 wait 中阻塞
        // notify_one  notify_all 唤醒 wait 中阻塞的线程
        // 3. 假设满足条件  mutex_.lock() 重新上锁
        // 4. 不满足条件，则继续阻塞在 wait 中，回到2
        not_empty_.wait(lock, [this] { return !queue_.empty() || nonblock_; });

        // 如果队列为空，且非阻塞模式，则返回 false
        if (queue_.empty() && nonblock_) {
            return false;
        }
        value = queue_.front();
        queue_.pop();
        return true;
    }

    // 解除阻塞在当前队列的线程
    void Cancel() {
        std::lock_guard<std::mutex> lock(mutex_);
        nonblock_ = true;  // 设置为非阻塞模式
        not_empty_.notify_all();  // 唤醒所有等待的线程
    }

private:
    bool nonblock_;
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable not_empty_;
};

template <typename T>
class BlockingQueuePro {
public:
    BlockingQueuePro(bool nonblock = false) : nonblock_(nonblock) {}

    void Push(const T &value) {
        std::lock_guard<std::mutex> lock(prod_mutex_);
        prod_queue_.push(value);
        not_empty_.notify_one();  // 通知一个等待的线程
    }

    bool Pop(T &value) {
        std::lock_guard<std::mutex> lock(cons_mutex_);
        if (cons_queue_.empty() && SwapQueue_() == 0) {
            return false;
        }
        value = cons_queue_.front();
        cons_queue_.pop();
        return true;
    }

    void Cancel() {
        std::lock_guard<std::mutex> lock(prod_mutex_);
        nonblock_ = true;  // 设置为非阻塞模式
        not_empty_.notify_all();  // 唤醒所有等待的线程
    }

private:
    int SwapQueue_() {
        std::unique_lock<std::mutex> lock(prod_mutex_);
        not_empty_.wait(lock, [this] { return !prod_queue_.empty() || nonblock_; });
        std::swap(prod_queue_, cons_queue_);  // 交换生产者和消费者队列
        return cons_queue_.size();  // 返回消费者队列的大小
    }

    bool nonblock_;  // 是否为非阻塞模式
    std::queue<T> prod_queue_;  // 队列
    std::queue<T> cons_queue_;  // 队列
    std::mutex prod_mutex_;  // 生产者锁
    std::mutex cons_mutex_;  // 消费者锁
    std::condition_variable not_empty_;  // 条件变量
};
