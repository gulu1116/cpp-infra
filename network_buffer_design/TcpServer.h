#pragma once

#include <memory>
#include <functional>

class TcpConn;
class EventLoop;
// 服务器类
class TcpServer {
public:
    using NewConnCallback = std::function<void(std::shared_ptr<TcpConn>)>;

    TcpServer(EventLoop& evloop);
    ~TcpServer();

    void Start(uint16_t port, NewConnCallback cb);

private:
    void HandleAccept(uint32_t events);

    EventLoop& evloop_;
    int listen_fd_;
    NewConnCallback new_conn_cb_;
    std::function<void(uint32_t)> accept_handler_;
};
