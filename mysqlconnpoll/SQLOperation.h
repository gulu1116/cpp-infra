
#pragma once
#include <string>
#include <future>
#include <memory>
#include <cppconn/resultset.h>

// 前置声明
class MySQLConn;

namespace sql 
{
    class ResultSet;
}

class SQLOperation {
public:
    explicit SQLOperation(const std::string &sql) : sql_(sql) {}  // explicit 防止隐式转换
    void Execute(MySQLConn *conn);

    std::future<std::unique_ptr<sql::ResultSet>> GetFuture() {
        return promise_.get_future();
    }
private:
    std::string sql_;
    std::promise<std::unique_ptr<sql::ResultSet>> promise_;
};