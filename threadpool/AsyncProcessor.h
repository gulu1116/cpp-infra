
#pragma once

#include <vector>
#include <mutex>

class QueryCallback;
class AsyncProcessor
{
public:
    void AddQueryCallback(QueryCallback &&query_callback);
    void InvokeIfReady();

private:
    std::vector<QueryCallback> pending_queries_;
};

// 容器：存储 callback
// 线程定时调用 InvokeIfReady
// 遍历容器，调用 QueryCallback::InvokeIfReady
// 如果就绪，调用回调函数，然后从容器删除
// 如果不就绪，继续保留在容器中
// 需要互斥锁保护容器