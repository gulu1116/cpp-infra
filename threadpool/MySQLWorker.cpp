#include "MySQLWorker.h"

#include "MySQLConn.h"
#include "SQLOperation.h"
#include "blockingqueue.h"

MySQLWorker::MySQLWorker(MySQLConn *conn, BlockingQueue<SQLOperation*> &task_queue) 
    : conn_(conn), task_queue_(&task_queue) 
{}

MySQLWorker::~MySQLWorker() {
    Stop();
}

void MySQLWorker::Start() {
    worker_ = std::thread(&MySQLWorker::Worker, this);
}

void MySQLWorker::Stop() {
    if (worker_.joinable()) {
        worker_.join();
    }
}

// 线程的入口函数
void MySQLWorker::Worker() {
    while (true) {
        SQLOperation *op = nullptr;
        if (!task_queue_->Pop(op)) {
            break; // 退出线程
        }
        if (op) {
            op->Execute(conn_);
            delete op;
        }
    }
}