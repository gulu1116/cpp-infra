#pragma once

#include <condition_variable>
#include <functional>
#include <queue>
#include <mutex>
#include <thread>
//glibc  -lpthread
template <typename T>
class BlockingQueue {
public:
    BlockingQueue(bool nonblock = false) : nonblock_(nonblock) { }
    // 入队操作
    void Push(const T &value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(value);
        not_empty_.notify_one();
    }
    // 正常 pop  弹出元素
    // 异常 pop  没有弹出元素
    bool Pop(T &value) {
        std::unique_lock<std::mutex> lock(mutex_);

        // 1. mutex_.unlock()
        // 2. queue_empty && !nonblock 线程在 wait 中阻塞
        // notify_one notify_all 唤醒线程
        // 3. 假设满足条件 mutex_.lock()
        // 4. 不满足条件 回到 2
        not_empty_.wait(lock, [this]{ return !queue_.empty() || nonblock_; });
        if (queue_.empty()) return false;

        value = queue_.front();
        queue_.pop();
        return true;
    }

    // 解除阻塞在当前队列的线程
    void Cancel() {
        std::lock_guard<std::mutex> lock(mutex_);
        nonblock_ = true;
        not_empty_.notify_all();
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
        not_empty_.notify_one();
    }

    bool Pop(T &value) {
        std::unique_lock<std::mutex> lock(cons_mutex_);
        if (cons_queue_.empty() && SwapQueue_() == 0) {
            return false;
        }
        value = cons_queue_.front();
        cons_queue_.pop();
        return true;
    }

    void Cancel() {
        std::lock_guard<std::mutex> lock(prod_mutex_);
        nonblock_ = true;
        not_empty_.notify_all();
    }

private:
    int SwapQueue_() {
        std::unique_lock<std::mutex> lock(prod_mutex_);
        not_empty_.wait(lock, [this] {return !prod_queue_.empty() || nonblock_; });
        std::swap(prod_queue_, cons_queue_);
        return cons_queue_.size();
    }

    bool nonblock_;
    std::queue<T> prod_queue_;
    std::queue<T> cons_queue_;
    std::mutex prod_mutex_;
    std::mutex cons_mutex_;
    std::condition_variable not_empty_;
};