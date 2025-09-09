#pragma once
#include <string>
#include <cppconn/driver.h>
// #include "SQLOperation.h"

// 前置声明
// sql 仅仅只能用作指针或引用
namespace sql 
{
    class Driver;
    class Connection;
    class SQLException;
    class ResultSet;
}

template<typename T>
class BlockingQueue;
class SQLOperation;
class MySQLWorker;

// 连接信息，关联的数据
struct MySQLConnInfo {
    explicit MySQLConnInfo(const std::string &info, const std::string &db);
    std::string user;
    std::string password;
    std::string url; // ip:port
    std::string database;
};

class MySQLConn {
public:
    MySQLConn(const std::string &info, const std::string &db, BlockingQueue<SQLOperation*> &task_queue);
    ~MySQLConn();

    int Open(); // 异常，重新连接
    void Close();

    sql::ResultSet* Query(const std::string &sql); 

private:
    void HandlerException(sql::SQLException &e); // 处理异常，重新连接
    sql::Driver *driver_;
    sql::Connection *conn_;
    MySQLWorker *worker_;
    MySQLConnInfo info_;
};