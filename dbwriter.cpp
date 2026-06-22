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
        std::unique_lock<std::mutex> lock(dbMutex);
        taskQueue.push(dbTask);
    }
    cv.notify_one();
}

void DbWriter::processQueue() {
    soci::session sql(pool);
    while (true) {
        std::vector<DbTask> tasks;
        {
            std::unique_lock<std::mutex> lock(dbMutex);
            cv.wait(lock, [this] { return !taskQueue.empty() || stop; });

            if (stop && taskQueue.empty()) break;

            while (!taskQueue.empty()) {
                tasks.push_back(taskQueue.front());
                taskQueue.pop();
            }
        }

        if (tasks.empty()) continue;

        try {
            soci::transaction tr(sql);
            for (const auto&[type, orderId, transactionID, buyerId, sellerId, price, quantity, stockId, side, status] : tasks) {
                if (type == DbTask::INSERT_TRADE) {
                    sql << "INSERT INTO trades (id, buyer, seller, price, quantity, ticker, timestamp) VALUES (:id, :buyer, :seller, :price, :quantity, :ticker, NOW())",
                        soci::use(transactionID), soci::use(buyerId), soci::use(sellerId), soci::use(price), soci::use(quantity), soci::use(stockId);
                } else if (type == DbTask::INSERT_ORDER) {
                    sql << "INSERT INTO orders (id, client, ticker, price, original_quantity, remaining_quantity, status, type, timestamp) VALUES "
                           "(:id, :client, :ticker, :price, :original_quantity, :remaining_quantity, :status, :type, NOW())",
                        soci::use(orderId), soci::use(buyerId), soci::use(stockId), soci::use(price), soci::use(quantity),
                        soci::use(quantity), soci::use(status), soci::use(side);
                } else if (type == DbTask::UPDATE_ORDER_QUANTITY) {
                    sql << "UPDATE orders SET remaining_quantity = :remaining_quantity, status = :status WHERE id = :id ",
                        soci::use(quantity), soci::use(status), soci::use(orderId);
                } else if (type == DbTask::UPDATE_ORDER_STATUS) {
                    sql << "UPDATE orders SET status = :status WHERE id = :id ",
                        soci::use(status), soci::use(orderId);
                }
            }
            tr.commit();
        }
        catch (const std::exception& e) {
            std::cerr << "Database operation failed: " << e.what() << std::endl;
        }
    }
}
