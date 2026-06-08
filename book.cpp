//
// Created by warren on 03-Jun-26.
//

#include "book.h"
#include "logic.h"

#include <iostream>
#include <ostream>
#include <ranges>

Book::Book(const uint32_t id) : id(id) {

}

void Book::addBuy(const Order &order) {
    const int price = static_cast<int>(order.getPrice() * 100);
    buy_book[price].addOrder(order);
    matchBuy(*this);
}

void Book::addSell(const Order &order) {
    const int price = static_cast<int>(order.getPrice() * 100);
    sell_book[price].addOrder(order);
    matchSell(*this);
}

uint64_t Book::deleteBuy(const Order &order) {
    const int price = static_cast<int>(order.getPrice() * 100);
    if (const auto it = buy_book.find(price); it != buy_book.end()) {
        const uint64_t orderId = order.getId();
        buy_book[price].deleteOrder(orderId);
        if (buy_book[price].getOrders()->empty()) {
            buy_book.erase(price);
        }
        return id;
    }
    return 0;
}

uint64_t Book::deleteSell(const Order &order) {
    const int price = static_cast<int>(order.getPrice() * 100);
    if (const auto it = sell_book.find(price); it != sell_book.end()) {
        const uint64_t orderId = order.getId();
        sell_book[price].deleteOrder(orderId);
        if (sell_book[price].getOrders()->empty()) {
            sell_book.erase(price);
        }
        return id;
    }
    return 0;
}

void Book::printBook() {
    std::cout << "Buy Book:" << std::endl;
    for (auto &val: buy_book | std::views::values) {
        for (const auto &order : *val.getOrders()) {
            std::cout << " OrderID: " << order.getId() << " Client" << order.getClient() <<  " Price: " << order.getPrice()/100.0 << " Quantity: " << order.getQuantity() << std::endl;
        }
    }
    std::cout << "Sell Book:" << std::endl;
    for (auto &val: sell_book | std::views::values) {
        for (const auto &order : *val.getOrders()) {
            std::cout << " OrderID: " << order.getId() << " Client: " << order.getClient() <<  " Price: " << order.getPrice()/100.0 << " Quantity: " << order.getQuantity() << std::endl;
        }
    }
}

std::map<int, OrderList, std::greater<>>* Book::getBuyBook() {
    return &buy_book;
}

std::map<int, OrderList, std::less<>>* Book::getSellBook() {
    return &sell_book;
}
