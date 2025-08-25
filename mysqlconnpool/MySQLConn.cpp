
#include <vector>
#include <string>
#include <string_view>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "MySQLConn.h"
#include "QueryCallback.h"
#include "BlockingQueue.h"
#include "MySQLWorker.h"

// "tcp://127.0.0.1:3306;root;123456"
// 使用;分割
static std::vector<std::string_view>
Tokenize(std::string_view str, char sep, bool keepEmpty)
{
    std::vector<std::string_view> tokens;

    size_t start = 0;
    for (size_t end = str.find(sep); end != std::string_view::npos; end = str.find(sep, start))
    {
        if (keepEmpty || end - start > 0)
            tokens.push_back(str.substr(start, end - start));
        start = end + 1;
    }

    if (keepEmpty || start < str.length())
        tokens.push_back(str.substr(start));
    
    return tokens;
}

// "tcp://127.0.0.1:3306;root;123456"
MySQLConnInfo::MySQLConnInfo(const std::string &info, const std::string &db)
{
    auto tokens = Tokenize(info, ';', false);
    if (tokens.size() != 3) {
        // throw std::runtime_error("MySQLConnInfo info format error, should be url;user;password");
        printf("MySQLConnInfo info format error, should be url;user;password\n");
        return;
    }   // 写大型项目一般不会用异常机制
    
    url.assign(tokens[0]);
    user.assign(tokens[1]);
    password.assign(tokens[2]);
    database.assign(db);
}

MySQLConn::MySQLConn(const std::string &info, const std::string &db, BlockingQueue<SQLOperation*> &task_queue)
    :info_(info, db)
{
    worker_ = new MySQLWorker(this, task_queue);
    worker_->Start();
}

MySQLConn::~MySQLConn()
{
    if (worker_) {
        worker_->Stop();
        delete worker_;
        worker_ = nullptr;
    }

    if (conn_) {
        delete conn_;
    }
}

int MySQLConn::Open()
{
    int err = 0;
    try {
        driver_ = get_driver_instance();
        conn_ = driver_->connect(info_.url, info_.user, info_.password);
        if (!conn_) {
            printf("MySQLConn::Open connect failed\n");
            return -1;
        }
        conn_->setSchema(info_.database);
    } catch (sql::SQLException &e) {
        HandlerException(e);
        err = e.getErrorCode();
        printf("MySQLConn::Open sql exception: %s, code: %d\n", e.what(), err);
    }
    return err;
}

void MySQLConn::Close()
{
    if (conn_) {
        conn_->close();
        delete conn_;
        conn_ = nullptr;
    }
}

sql::ResultSet* MySQLConn::Query(const std::string &sql)
{
    try {
        sql::Statement *stmt = conn_->createStatement();
        return stmt->executeQuery(sql);
    } catch (sql::SQLException &e) {
        HandlerException(e);
        printf("MySQLConn::Query sql exception: %s, code: %d\n", e.what(), e.getErrorCode());
    }
    return nullptr;
}

void MySQLConn::HandlerException(sql::SQLException &e)
{
    if (e.getErrorCode() == 2006 || e.getErrorCode() == 2013) { // MySQL server has gone away
        printf("MySQLConn::HandlerException reconnecting...\n");
        Close();
        Open();
    } else {
        printf("MySQLConn::HandlerException sql exception: %s, code: %d\n", e.what(), e.getErrorCode());
    }
}