#include "MySQLConnPool.h"
#include "AsyncProcessor.h"
#include <cppconn/resultset.h>
#include <iostream>
#include <thread>
#include <chrono>

/*
g++ AsyncProcessor.cpp main.cpp MySQLConn.cpp MySQLConnPool.cpp MySQLWorker.cpp SQLOperation.cpp -o main -lpthread -lmysqlcppconn -std=c++17 -g
*/

void HandleQueryResult(std::unique_ptr<sql::ResultSet> res)
{
    while (res->next())
    {
        std::cout << "cid: " << res->getInt("cid") << " caption: " << res->getString("caption") << std::endl;
    }
}

int main()
{
    // Initialize the connection pool for the first database
    MySQLConnPool *pool1 = MySQLConnPool::GetInstance("edu_svc");
    pool1->InitPool("tcp://127.0.0.1:3306;root;123456", 10);
    AsyncProcessor response_handler;

    // Query the first database
    auto query_callback1 = pool1->Query("SELECT * FROM class", HandleQueryResult);
    response_handler.AddQueryCallback(std::move(query_callback1));

    // Periodically invoke callbacks if they are ready
    while (true)
    {
        response_handler.InvokeIfReady();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}