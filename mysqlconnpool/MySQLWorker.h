#pragma once
#include <thread>

// 前置声明
// sql 仅仅只能用作指针或引用
class MySQLConn;

template<typename T>
class BlockingQueue;

class SQLOperation;

class MySQLWorker {
public:
    MySQLWorker(MySQLConn *conn, BlockingQueue<SQLOperation*> &task_queue);
    ~MySQLWorker();

    void Start();
    void Stop();

private:
    void Worker(); // 线程的入口函数
    MySQLConn *conn_;
    std::thread worker_;
    BlockingQueue<SQLOperation*> *task_queue_;
};