
#pragma once
#include <future>
#include <functional>
#include <memory>
#include <cppconn/resultset.h>

// 前置声明
namespace sql 
{
    class ResultSet;
}

class QueryCallback {
public:
    QueryCallback(std::future<std::unique_ptr<sql::ResultSet>> &&future,
                  std::function<void(std::unique_ptr<sql::ResultSet>)> &&cb)
        : future_(std::move(future)), cb_(std::move(cb)) {}

    // promise 拿到结果，future 就绪
    bool InvokeIfReady() {
        if (future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            cb_(std::move(future_.get())); // 调用回调函数
            return true;
        }
        return false;
    }
    
private:
    std::future<std::unique_ptr<sql::ResultSet>> future_;
    std::function<void(std::unique_ptr<sql::ResultSet>)> cb_;  // 回调函数，参数是 sql 语句执行结果
};