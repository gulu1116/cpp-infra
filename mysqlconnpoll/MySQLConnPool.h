#pragma once
#include <vector>
#include <memory>
#include <functional>
// #include <map>  // 要求里面的元素有序，O(log n)
#include <unordered_map> // 不要求里面的元素有序，O(1)

#include "QueryCallback.h"

// 前置声明
// blockingqueue 仅仅只能用作指针或引用
template<typename T>
class BlockingQueue;
class MySQLConn;
class SQLOperation;

namespace sql 
{
    class ResultSet;
}

class MySQLConnPool {
public:
    // 类似单例，根据库返回一个连接池
    static MySQLConnPool *GetInstance(const std::string &db);

    void InitPool(const std::string &url, int pool_size);

    // 发起请求
    // sql 语句，回调函数，future
    QueryCallback Query(const std::string &sql, std::function<void(std::unique_ptr<sql::ResultSet>)> &&cb);

private:
    MySQLConnPool(const std::string &db) : database_(db) {}
    ~MySQLConnPool();

    std::string database_;
    std::vector<MySQLConn*> pool_;
    static std::unordered_map<std::string, MySQLConnPool*> instances_; // key 是 url+db 的组合
    BlockingQueue<SQLOperation*> *task_queue_;
};