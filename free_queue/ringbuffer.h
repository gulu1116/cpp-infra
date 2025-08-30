#pragma once

// SPSC 单生产者单消费者

#include <atomic>
#include <type_traits>

template<typename T, std::size_t Capacity>
class RingBuffer {
public:
    static_assert(Capacity && !(Capacity & (Capacity - 1)), "Capacity must be power of 2");
    RingBuffer() : read_(0), write_(0) {}
    ~RingBuffer() {
        std::size_t r = read_.load(std::memory_order_relaxed);
        std::size_t w = write_.load(std::memory_order_relaxed);
        while (r != w) {
            reinterpret_cast<T *>(&buffer_[r])->~T();
            r = (r + 1) & (Capacity - 1);
        }
    }

    // 这里使用万能引用和完美转发，支持左值和右值
    template<typename U>
    bool Push(U && value) {
        const std::size_t w = write_.load(std::memory_order_relaxed);
        const std::size_t next_w = (w + 1) & (Capacity - 1); //  (w + 1) % Capacity 优化
        // 检查缓冲区是否已满
        if (next_w == read_.load(std::memory_order_acquire)) {
            return false;
        }
        // 完美转发
        new (&buffer_[w]) T(std::forward<U>(value));
        write_.store(next_w, std::memory_order_release);
        return true;
    }

    bool Pop(T & value) {
        const std::size_t r = read_.load(std::memory_order_relaxed);
        if (r == write_.load(std::memory_order_acquire)) {
            return false;
        }

        // 取出元素并析构
        value = std::move(*reinterpret_cast<T *>(&buffer_[r]));
        reinterpret_cast<T *>(&buffer_[r])->~T();
        read_.store((r + 1) & (Capacity - 1), std::memory_order_release);
        return true;
    }

    std::size_t Size() const {
        const std::size_t r = read_.load(std::memory_order_acquire);
        const std::size_t w = write_.load(std::memory_order_acquire);
        return (w >= r) ? (w - r) : (Capacity - r + w);
    }

private:
    alignas(64) std::atomic<std::size_t> read_; // 设置对齐值
    alignas(64) std::atomic<std::size_t> write_;
    alignas(64) std::aligned_storage_t<sizeof(T), alignof(T)> buffer_[Capacity]; // 未初始化的内存，支持 pod 和 非pod 类型

};

