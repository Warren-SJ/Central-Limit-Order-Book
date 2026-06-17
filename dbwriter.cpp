//
// Created by warren on 17-Jun-26.
//

#include "dbwriter.h"
#include <iostream>

DbWriter::DbWriter(soci::connection_pool& pool) : pool(pool), stop(false) {
    worker = std::thread(&DbWriter::processQueue, this);
}

DbWriter::~DbWriter() {
    {
        std::lock_guard<std::mutex> lock(dbMutex);
        stop = true;
    }
    cv.notify_one();
    if (worker.joinable()) {
        worker.join();
    }
}

void DbWriter::push(const DbTask &dbTask) {
    {
        std::lock_guard<std::mutex> lock(dbMutex);
        taskQueue.push(dbTask);
    }
    cv.notify_one();
}

void DbWriter::processQueue() {
    soci::session sql(pool);
    while (true) {
        DbTask task{};
        {
            std::unique_lock<std::mutex> lock(dbMutex);
            cv.wait(lock, [this] { return !taskQueue.empty() || stop; });

            if (stop && taskQueue.empty()) break;

            task = taskQueue.front();
            taskQueue.pop();
        }

        try {
            if (task.type == DbTask::INSERT_TRADE) {
                sql << "INSERT INTO trades (id, buyer, seller, price, quantity, ticker, timestamp) VALUES (:id, :buyer, :seller, :price, :quantity, :ticker, NOW())",
                    soci::use(task.transactionID), soci::use(task.buyerId), soci::use(task.sellerId), soci::use(task.price), soci::use(task.quantity), soci::use(task.stockId);
            } else if (task.type == DbTask::INSERT_ORDER) {
                sql << "INSERT INTO orders (id, client, ticker, price, original_quantity, remaining_quantity, status, type, timestamp) VALUES "
                       "(:id, :client, :ticker, :price, :original_quantity, :remaining_quantity, :status, :type, NOW())",
                    soci::use(task.orderId), soci::use(task.buyerId), soci::use(task.stockId), soci::use(task.price), soci::use(task.quantity),
                    soci::use(task.quantity), soci::use(task.status), soci::use(task.side);
            } else if (task.type == DbTask::UPDATE_ORDER) {
                sql << "UPDATE orders SET remaining_quantity = :remaining_quantity, status = :status WHERE id = :id ",
                    soci::use(task.quantity), soci::use(task.status), soci::use(task.orderId);
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Database operation failed: " << e.what() << std::endl;
        }
    }
}
