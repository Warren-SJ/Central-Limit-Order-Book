//
// Created by warren on 03-Jun-26.
//

#ifndef CLOB_BOOK_H
#define CLOB_BOOK_H

#include <cstdint>
#include <map>
#include "orderList.h"

class Book {
public:
    Book(uint32_t id);

private:
    uint32_t id;
    std::map<double, OrderList, std::greater<>> buy_book;
    std::map<double, OrderList, std::less<>> sell_book;
};

#endif //CLOB_BOOK_H
