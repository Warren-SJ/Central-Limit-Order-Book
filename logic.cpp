//
// Created by warren on 03-Jun-26.
//

#include "logic.h"

void matchOrder(Book &book){
    // This function will implement the logic to match buy and sell orders in the book.
    // It will check the highest bid price against the lowest ask price and execute trades accordingly.
    const std::map<double, OrderList, std::greater<>> buyBook = book.getBuyBook();
    const std::map<double, OrderList, std::less<>> sellBook = book.getSellBook();

    if (buyBook.empty() || sellBook.empty()) {
        return;
    }

    double bid = buyBook.begin()->first;
    double ask = sellBook.begin()->first;

    if (bid < ask) {
        return;
    }
    std::cout << "Matching possible" <<std::endl;
}