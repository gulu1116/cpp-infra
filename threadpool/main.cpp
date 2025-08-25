#include "AsyncProcessor.h"
#include "MySQLConnPool.h"
#include <thread>
#include <chrono>
#include <iostream>

#include <cppconn/resultset.h>

/*
    g++ AsyncProcessor.cpp main.cpp MySQLConn.cpp MySQLConnPool.cpp MySQLWorker.cpp SQLOperation.cpp 
    -o main -lpthread -lmysqlcppconn -g
*/

void HandleQueryResult(std::unique_ptr<sql::ResultSet> res) {
    if (res) {
        while (res->next()) {
            std::cout << "id: " << res->getInt("id") << " name: " << res->getString("name") << std::endl;
        }
    } else {
        std::cout << "Query failed or returned no results." << std::endl;
    }
}

int main()
{
    // 初始化连接池
    MySQLConnPool *pool1 = MySQLConnPool::GetInstance("gulu");
    pool1->InitPool("tcp://192.168.8.154:3306;lqf;051370645", 10);
    AsyncProcessor processor_handler;

    // 发起查询
    auto query_callback1 = pool1->Query("SELECT * FROM gulu.course", HandleQueryResult);
    processor_handler.AddQueryCallback(std::move(query_callback1));

    // 主线程定时调用 InvokeIfReady
    // 模拟事件循环
    // 每 100 毫秒检查一次
    while (true) {
        processor_handler.InvokeIfReady();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return 0;
}