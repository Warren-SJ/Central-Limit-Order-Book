//
// Created by warren on 17-Jun-26.
//

#ifndef CLOB_DBWRITER_H
#define CLOB_DBWRITER_H

#include <cstdint>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <soci/soci.h>

struct DbTask {
    enum Type {INSERT_TRADE, INSERT_ORDER, UPDATE_ORDER_QUANTITY, UPDATE_ORDER_STATUS} type;
    uint64_t orderId;
    uint64_t transactionID;
    uint64_t buyerId;
    uint64_t sellerId;
    int price;
    int quantity;
    uint32_t stockId;
    char side;
    char status;
};

class DbWriter {
public:
    explicit DbWriter(soci::connection_pool& pool);
    ~DbWriter();
    void push(const DbTask &dbTask);

private:
    void processQueue();
    soci::connection_pool& pool;
    std::queue<DbTask> taskQueue;
    std::thread worker;
    std::mutex dbMutex;
    std::condition_variable cv;
    bool stop;
};

#endif //CLOB_DBWRITER_H
