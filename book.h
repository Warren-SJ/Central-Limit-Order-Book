//
// Created by warren on 03-Jun-26.
//

#ifndef CLOB_BOOK_H
#define CLOB_BOOK_H

#include <cstdint>
#include <map>
#include <unordered_map>

#include "orderList.h"
#include "order.h"
#include "journal.h"

class Book {
public:
    explicit Book(uint32_t id);
    void addBuy(const Order &order, Journal &journal);
    void addSell(const Order &order, Journal &journal);
    uint64_t deleteBuy(uint64_t orderId);
    uint64_t deleteSell(uint64_t orderId);
    void printBook();
    std::map<int, OrderList, std::greater<>>* getBuyBook();
    std::map<int, OrderList, std::less<>>* getSellBook();
    [[nodiscard]] uint32_t getId() const;
private:
    uint32_t id;
    std::map<int, OrderList, std::greater<>> buy_book;
    std::map<int, OrderList, std::less<>> sell_book;
    struct orderLocation {
        int price;
        std::list<Order>::iterator it;
    };
    std::unordered_map<uint64_t, orderLocation> order_lookup;
};

#endif //CLOB_BOOK_H
