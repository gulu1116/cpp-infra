#pragma once

/*
#pragma once 类似：
#ifndef _HEADER_NAME_H_
#define _HEADER_NAME_H_
// 头文件内容
#endif
*/

#include <functional>

class ThreadPool {
public:
    void Post(std::function<void()> task);
};