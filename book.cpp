//
// Created by warren on 03-Jun-26.
//

#include "book.h"
#include "logic.h"

#include <iostream>
#include <ostream>

Book::Book(const uint32_t id) : id(id) {

}

void Book::addBuy(const Order &order) {
    const double price = order.getPrice();
    buy_book[price].addOrder(order);
    matchOrder(*this);
}

void Book::addSell(const Order &order) {
    const double price = order.getPrice();
    sell_book[price].addOrder(order);
    matchOrder(*this);
}

uint64_t Book::deleteBuy(const Order &order) {
    const double price = order.getPrice();
    if (const auto it = buy_book.find(price); it != buy_book.end()) {
        uint64_t id = order.getId();
        buy_book[price].deleteOrder(id);
        return id;
    }
    return 0;
}

uint64_t Book::deleteSell(const Order &order) {
    const double price = order.getPrice();
    if (const auto it = sell_book.find(price); it != sell_book.end()) {
        uint64_t id = order.getId();
        sell_book[price].deleteOrder(id);
        return id;
    }
    return 0;
}

void Book::printBook() {
    std::cout << "Buy Book:" << std::endl;
    for (auto & it : buy_book) {
        for (const auto &order : it.second.getOrders()) {
            std::cout << " OrderID: " << order.getId() << "Client" << order.getClient() <<  " Price: " << order.getPrice() << " Quantity: " << order.getQuantity() << std::endl;
        }
    }
    std::cout << "Sell Book:" << std::endl;
    for (auto & it : sell_book) {
        for (const auto &order : it.second.getOrders()) {
            std::cout << " OrderID: " << order.getId() << " Client: " << order.getClient() <<  " Price: " << order.getPrice() << " Quantity: " << order.getQuantity() << std::endl;
        }
    }
}

std::map<double, OrderList, std::greater<>> Book::getBuyBook() const {
    return buy_book;
}

std::map<double, OrderList, std::less<>> Book::getSellBook() const {
    return sell_book;
}
