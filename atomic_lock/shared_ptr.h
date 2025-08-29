#pragma once

#include <atomic>

// shared_ptr<int> p1(new int(42));
// shared_ptr<int> p1 = new int(42);  // 不允许隐式转换
// shared_ptr<int> p2 = p1; // 引用计数加1

class A {
public:
    void func() {}
};

// shared_ptr<A> p(new A());
// p->func();

template <typename T>
class shared_ptr {  // 16字节
public:
    shared_ptr() : ptr_(nullptr), ref_count_(nullptr) {}

    explicit shared_ptr(T* ptr) : ptr_(ptr), ref_count_(ptr ? new std::atomic<std::size_t>(1) : nullptr) {}

    ~shared_ptr() {
        release();
    }

    // 拷贝构造函数
    shared_ptr(const shared_ptr<T>& other) : ptr_(other.ptr_), ref_count_(other.ref_count_) {
        if (ref_count_) {
            ref_count_->fetch_add(1, std::memory_order_relaxed);
        }
    }

    // shared_ptr<int> p2 = p1 = p3; // 链式赋值
    // 拷贝赋值运算符
    shared_ptr<T>& operator=(const shared_ptr<T>& other) { // 自赋值检查
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            ref_count_ = other.ref_count_;
            if (ref_count_) {
                ref_count_->fetch_add(1, std::memory_order_relaxed);
            }
        }
        return *this;
    }

    // 移动构造
    // nonexcept 保证不会抛出异常，编译器会生成更高效的代码
    // STL
    shared_ptr<T>(shared_ptr<T>&& other) noexcept : ptr_(other.ptr_), ref_count_(other.ref_count_) {
        other.ptr_ = nullptr;
        other.ref_count_ = nullptr;
    }

    // 移动赋值
    shared_ptr<T>& operator=(shared_ptr<T>&& other) noexcept {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            ref_count_ = other.ref_count_;
            other.ptr_ = nullptr;
            other.ref_count_ = nullptr;
        }
        return *this;
    }

    // 重载*和->运算符
    T& operator*() const {
        return *ptr_;
    }

    T* operator->() const {
        return ptr_;
    }

    std::size_t use_count() const {
        return ref_count_ ? ref_count_->load(std::memory_order_acquire) : 0;
    }

    T* get() const {
        return ptr_;
    }

    // 重置shared_ptr
    void reset(T* p = nullptr) {
        release();
        ptr_ = p;
        ref_count_ = p ? new std::atomic<std::size_t>(1) : nullptr;
    }

private:
    void release() {
        if (ref_count_ && ref_count_->fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete ptr_;
            delete ref_count_;
        }
    }
    T* ptr_;
    std::atomic<std::size_t>* ref_count_;  // 指向原子变量的指针
};
