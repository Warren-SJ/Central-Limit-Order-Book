//
// Created by warren on 17-Jun-26.
//

#include "dbwriter.h"
#include <iostream>
#include <chrono>

DbWriter::DbWriter(soci::connection_pool& pool) : pool(pool), stop(false) {
    for (int i = 0; i < NUM_THREADS; ++i) {
        workers.emplace_back(&DbWriter::processQueue, this);
    }
}

DbWriter::~DbWriter() {
    {
        std::lock_guard<std::mutex> lock(dbMutex);
        stop = true;
    }
    cv.notify_all();
    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
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
    constexpr int maxBatchSize = 5000;

    std::vector<uint64_t> tradeIds, orderIds, tradeBuyers, tradeSellers, insertOrderIds, orderBuyers;
    std::vector<uint32_t> tradeStockIds, orderStockIds, updateOrderQuantityIds, updateOrderStatusIds;
    std::vector<int> tradePrices, tradeQuantities, orderPrices, orderQuantities, updateOrderQuantities;
    std::vector<char> tradeStatus, tradeType, orderStatuses, orderSides, updateOrderStatuses, updateOrderQuantityStatuses;

    tradeIds.reserve(maxBatchSize);
    orderIds.reserve(maxBatchSize);
    tradeBuyers.reserve(maxBatchSize);
    tradeSellers.reserve(maxBatchSize);
    insertOrderIds.reserve(maxBatchSize);
    orderBuyers.reserve(maxBatchSize);
    tradeStockIds.reserve(maxBatchSize);
    orderStockIds.reserve(maxBatchSize);
    updateOrderQuantityIds.reserve(maxBatchSize);
    updateOrderStatusIds.reserve(maxBatchSize);
    tradePrices.reserve(maxBatchSize);
    tradeQuantities.reserve(maxBatchSize);
    orderPrices.reserve(maxBatchSize);
    orderQuantities.reserve(maxBatchSize);
    updateOrderQuantities.reserve(maxBatchSize);
    tradeStatus.reserve(maxBatchSize);
    tradeType.reserve(maxBatchSize);
    orderStatuses.reserve(maxBatchSize);
    orderSides.reserve(maxBatchSize);
    updateOrderStatuses.reserve(maxBatchSize);
    updateOrderQuantityStatuses.reserve(maxBatchSize);

    while (true) {

        tradeIds.clear();
        orderIds.clear();
        tradeBuyers.clear();
        tradeSellers.clear();
        insertOrderIds.clear();
        orderBuyers.clear();
        tradeStockIds.clear();
        orderStockIds.clear();
        updateOrderQuantityIds.clear();
        updateOrderStatusIds.clear();
        tradePrices.clear();
        tradeQuantities.clear();
        orderPrices.clear();
        orderQuantities.clear();
        updateOrderQuantities.clear();
        tradeStatus.clear();
        tradeType.clear();
        orderStatuses.clear();
        orderSides.clear();
        updateOrderStatuses.clear();
        updateOrderQuantityStatuses.clear();

        std::vector<DbTask> tasks;
        {
            std::unique_lock<std::mutex> lock(dbMutex);
            cv.wait(lock, [this] { return !taskQueue.empty() || stop; });

            if (stop && taskQueue.empty()) break;

            int count = 0;
            constexpr auto maxBatchDuration = std::chrono::seconds(5);
            auto batchStart = std::chrono::steady_clock::now();
            while (!taskQueue.empty() && count < maxBatchSize) {
                tasks.push_back(taskQueue.front());
                taskQueue.pop();
                count++;
                if (std::chrono::steady_clock::now() - batchStart >= maxBatchDuration) {
                    break;
                }
            }
        }

        if (tasks.empty()) continue;

        for (const auto&[type, orderId, tradeId, buyerId, sellerId, price, quantity, stockId, side, status] : tasks) {
            if (type == DbTask::INSERT_ORDER) {
                orderIds.push_back(orderId);
                orderBuyers.push_back(buyerId);
                orderStockIds.push_back(stockId);
                orderPrices.push_back(price);
                orderQuantities.push_back(quantity);
                orderStatuses.push_back(status);
                orderSides.push_back(side);
            }
            else if (type == DbTask::INSERT_TRADE) {
                tradeIds.push_back(tradeId);
                tradeBuyers.push_back(buyerId);
                tradeSellers.push_back(sellerId);
                tradePrices.push_back(price);
                tradeQuantities.push_back(quantity);
                tradeStockIds.push_back(stockId);
            }
            else if (type == DbTask::UPDATE_ORDER_QUANTITY) {
                updateOrderQuantityIds.push_back(orderId);
                updateOrderQuantities.push_back(quantity);
                updateOrderQuantityStatuses.push_back(status);
            }
            else if (type == DbTask::UPDATE_ORDER_STATUS) {
                updateOrderStatusIds.push_back(orderId);
                updateOrderStatuses.push_back(status);
            }
        }
        try {
            soci::transaction tr(sql);
            if (!tradeIds.empty()) {
                sql << "INSERT INTO trades (id, buyer, seller, price, quantity, ticker, timestamp) VALUES (:id, :buyer, :seller, :price, :quantity, :ticker, NOW())",
                soci::use(tradeIds), soci::use(tradeBuyers), soci::use(tradeSellers), soci::use(tradePrices), soci::use(tradeQuantities), soci::use(tradeStockIds);
            }
            if (!orderIds.empty()) {
                sql << "INSERT INTO orders (id, client, ticker, price, original_quantity, remaining_quantity, status, type, timestamp) VALUES ""(:id, :client, :ticker, :price, :original_quantity, :remaining_quantity, :status, :type, NOW())",
                soci::use(orderIds), soci::use(orderBuyers), soci::use(orderStockIds), soci::use(orderPrices), soci::use(orderQuantities),soci::use(orderQuantities), soci::use(orderStatuses), soci::use(orderSides);
            }
            if (!updateOrderQuantityIds.empty()) {
                sql << "UPDATE orders SET remaining_quantity = :remaining_quantity, status = :status WHERE id = :id ",
                soci::use(updateOrderQuantities), soci::use(updateOrderQuantityStatuses), soci::use(updateOrderQuantityIds);
            }
            if (!updateOrderStatusIds.empty()) {
                sql << "UPDATE orders SET status = :status WHERE id = :id ",
                soci::use(updateOrderStatuses), soci::use(updateOrderStatusIds);
            }
            tr.commit();
        }
        catch (const std::exception& e) {
            std::cerr << "Database operation failed: " << e.what() << std::endl;
        }
    }
}