//
// Created by warren on 03-Jun-26.
//

#ifndef CLOB_BOOK_H
#define CLOB_BOOK_H

#include <cstdint>
#include <map>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <atomic>

#include "order.h"
#include "orderList.h"
#include "orderLocation.h"

#include <memory_resource>

class DbWriter;

class Book {
public:
    explicit Book(uint32_t id);
    void addBuy(const Order &order, uint32_t stockId, DbWriter &dbWriter, std::atomic<uint64_t> &transactionId,
        std::shared_mutex& orderMutex, std::unordered_map<uint64_t, OrderLocation>& locations);
    void addSell(const Order &order, uint32_t stockId, DbWriter& dbWriter, std::atomic<uint64_t>& transactionId,
        std::shared_mutex& orderMutex, std::unordered_map<uint64_t, OrderLocation>& locations);
    uint64_t deleteBuy(uint64_t orderId);
    uint64_t deleteSell(uint64_t orderId);
    void printBook();
    std::pmr::map<int, OrderList, std::greater<>>* getBuyBook();
    std::pmr::map<int, OrderList, std::less<>>* getSellBook();
    [[nodiscard]] uint32_t getId() const;
    std::mutex& getMutex() const;

private:
    uint32_t id;
    mutable std::mutex book_mutex;
    std::pmr::unsynchronized_pool_resource pool;
    std::pmr::map<int, OrderList, std::greater<>> buy_book;
    std::pmr::map<int, OrderList, std::less<>> sell_book;
    struct orderTracker {
        OrderList* orderListPtr;
        std::pmr::list<Order>::iterator orderIt;
        int price;
    };
    std::pmr::unordered_map<uint64_t, orderTracker> order_lookup;
};

#endif //CLOB_BOOK_H
