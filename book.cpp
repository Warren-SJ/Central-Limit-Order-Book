#include <iostream>
#include <ranges>

#include "book.h"

#include <shared_mutex>

#include "logic.h"
#include "order.h"

Book::Book(const uint32_t id) : id(id), pool(), buy_book(&pool), sell_book(&pool), order_lookup(&pool) {}

void Book::addBuy(const Order &order, const uint32_t stockId, DbWriter &dbWriter, std::atomic<uint64_t> &transactionId,
                  std::shared_mutex& orderMutex, std::unordered_map<uint64_t, OrderLocation>& locations) {
    const int price = order.getPrice();
    auto [It, inserted] = buy_book.try_emplace(price);
    const auto orderIt = It->second.addOrder(order);
    order_lookup[order.getId()]= {&(It->second), orderIt, price};
    matchBuy(*this, stockId, dbWriter, transactionId, orderMutex, locations);
}

void Book::addSell(const Order &order, const uint32_t stockId, DbWriter& dbWriter, std::atomic<uint64_t>& transactionId,
                  std::shared_mutex& orderMutex, std::unordered_map<uint64_t, OrderLocation>& locations) {
    const int price = order.getPrice();
    auto [It, inserted] = sell_book.try_emplace(price);
    const auto orderIt = It->second.addOrder(order);
    order_lookup[order.getId()]= {&(It->second), orderIt, price};
    matchSell(*this, stockId, dbWriter, transactionId,  orderMutex, locations);
}

uint64_t Book::deleteBuy(const uint64_t orderId) {
    const auto it = order_lookup.find(orderId);
    if (it == order_lookup.end()) return 0;
    OrderList* listPtr = it->second.orderListPtr;
    const auto order_it = it->second.orderIt;
    listPtr->removeOrder(order_it);
    if (listPtr->getOrders()->empty()) {
        buy_book.erase(it->second.price);
    }
    order_lookup.erase(it);
    return orderId;
}

uint64_t Book::deleteSell(const uint64_t orderId) {
    const auto it = order_lookup.find(orderId);
    if (it == order_lookup.end()) return 0;
    OrderList* listPtr = it->second.orderListPtr;
    const auto order_it = it->second.orderIt;
    listPtr->removeOrder(order_it);
    if (listPtr->getOrders()->empty()) {
        sell_book.erase(it->second.price);
    }
    order_lookup.erase(it);
    return orderId;
}

void Book::printBook() {
    std::cout << "Buy Book:" << std::endl;
    for (auto &val: buy_book | std::views::values) {
        for (const auto &order : *val.getOrders()) {
            std::cout << " OrderID: " << order.getId() << " Client: " << order.getClient() <<  " Price: " << order.getPrice()/100.0 << " Quantity: " << order.getQuantity() << std::endl;
        }
    }
    std::cout << "Sell Book:" << std::endl;
    for (auto &val: sell_book | std::views::values) {
        for (const auto &order : *val.getOrders()) {
            std::cout << " OrderID: " << order.getId() << " Client: " << order.getClient() <<  " Price: " << order.getPrice()/100.0 << " Quantity: " << order.getQuantity() << std::endl;
        }
    }
}

std::pmr::map<int, OrderList, std::greater<>>* Book::getBuyBook() {
    return &buy_book;
}

std::pmr::map<int, OrderList, std::less<>>* Book::getSellBook() {
    return &sell_book;
}

uint32_t Book::getId() const {
    return id;
}

std::mutex& Book::getMutex() const {
    return book_mutex;
}
