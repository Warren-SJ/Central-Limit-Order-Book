//
// Created by warren on 03-Jun-26.
//


#include <iostream>
#include <ostream>
#include <ranges>

#include "journal.h"
#include "book.h"
#include "logic.h"

Book::Book(const uint32_t id) : id(id) {

}

void Book::addBuy(const Order &order, Journal& journal) {
    const int price = order.getPrice();
    const auto it = buy_book[price].addOrder(order);
    order_lookup[order.getId()] = {price, it};
    matchBuy(*this, journal);
}

void Book::addSell(const Order &order, Journal& journal) {
    const int price = order.getPrice();
    const auto it = sell_book[price].addOrder(order);
    order_lookup[order.getId()] = {price, it};
    matchSell(*this, journal);
}

uint64_t Book::deleteBuy(const uint64_t orderId) {
    const auto it = order_lookup.find(orderId);
    if (it == order_lookup.end()) return 0;
    int price = it->second.price;
    const auto order_it = it->second.it;
    buy_book[price].removeOrder(order_it);
    if (buy_book[price].getOrders()->empty()) {
        buy_book.erase(price);
    }
    order_lookup.erase(it);
    return orderId;
}

uint64_t Book::deleteSell(const uint64_t orderId) {
    const auto it = order_lookup.find(orderId);
    if (it == order_lookup.end()) return 0;
    int price = it->second.price;
    const auto order_it = it->second.it;
    sell_book[price].removeOrder(order_it);
    if (sell_book[price].getOrders()->empty()) {
        sell_book.erase(price);
    }
    order_lookup.erase(it);
    return orderId;
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

uint32_t Book::getId() const {
    return id;
}
