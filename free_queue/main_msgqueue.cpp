#include "msgqueue.h"

#include <cstddef>
#include <thread>

#include <iostream>

// 侵入式还是非侵入式
struct Count {
    Count(int _v) : v(_v), next(nullptr) {}
    int v;
    Count *next;
};

int main() {

// linkoff  Count 偏移多少个字节就是用于链接下一个节点的next指针
    msgqueue_t* queue = msgqueue_create(1024, sizeof(int));
    std::thread pd1([&]() {
        msgqueue_put(new Count(100), queue);
        msgqueue_put(new Count(200), queue);
        msgqueue_put(new Count(300), queue);
        msgqueue_put(new Count(400), queue);
    });

    std::thread pd2([&]() {
        msgqueue_put(new Count(500), queue);
        msgqueue_put(new Count(600), queue);
        msgqueue_put(new Count(700), queue);
        msgqueue_put(new Count(800), queue);
    });

    std::thread cs1([&]() {
        Count *cnt;
        while((cnt = (Count *)msgqueue_get(queue)) != NULL) {
            std::cout << std::this_thread::get_id() << " : pop " << cnt->v << std::endl;
            delete cnt;
        }
    });
    std::thread cs2([&]() {
        Count *cnt;
        while((cnt = (Count *)msgqueue_get(queue)) != NULL) {
            std::cout << std::this_thread::get_id() << " : pop " << cnt->v << std::endl;
            delete cnt;
        }
    });

    pd1.join();
    pd2.join();
    cs1.join();
    cs2.join();

    msgqueue_destroy(queue);
    
    return 0;
}