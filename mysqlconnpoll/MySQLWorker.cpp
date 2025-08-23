
#include "MySQLWorker.h"

#include "BlockingQueue.h"
#include "SQLOperation.h"
#include "MySQLConn.h"

MySQLWorker::MySQLWorker(MySQLConn *conn, BlockingQueue<SQLOperation *> &task_queue)
    : conn_(conn), task_queue_(task_queue)
{
}

MySQLWorker::~MySQLWorker()
{
    Stop();
}

void MySQLWorker::Start()
{
    worker_ = std::thread(&MySQLWorker::Worker, this);
}

void MySQLWorker::Stop()
{
    if (worker_.joinable()) {
        worker_.join();
    }
}

void MySQLWorker::Worker() {
    while (true) {
        SQLOperation *op = nullptr;
        if (!task_queue_.Pop(op)) {
            break;
        }
        op->Execute(conn_);
        delete op;
    }
}