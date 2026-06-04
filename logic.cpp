//
// Created by warren on 03-Jun-26.
//

#include "logic.h"

void matchBuy(Book &book) {
    // This function will run when a new buy order is made
    std::map<double, OrderList, std::greater<>>* buyBook = book.getBuyBook();
    std::map<double, OrderList, std::less<>>* sellBook = book.getSellBook();

    if (buyBook->empty() || sellBook->empty()) {
        return;
    }

    while (buyBook->begin()->first >= sellBook->begin()->first) {
        double execution_price = sellBook->begin()->first;
        OrderList orders = buyBook->begin()->second;
        int quantity = orders.getOrders()->begin()->getQuantity();
        while (quantity > 0) {
            if (sellBook->begin()->first != execution_price) {
                break;
            }
            int sell_amount = sellBook->begin()->second.getOrders()->begin()->getQuantity();
            if (sell_amount > quantity) {
                sellBook->begin()->second.getOrders()->begin()->setQuantity(sell_amount - quantity);
                book.deleteBuy(*(buyBook->begin()->second.getOrders()->begin()));
                quantity = 0;
            }
            else{
                book.deleteSell(*(buyBook->begin()->second.getOrders()->begin()));
                if (sell_amount == quantity) {
                    book.deleteBuy(*(buyBook->begin()->second.getOrders()->begin()));
                    quantity = 0;
                }
                else {
                    quantity -= sell_amount;
                    book.getBuyBook()->begin()->second.getOrders()->begin()->setQuantity(quantity);
                }
            }
        }
    }
}

void matchSell(Book &book) {
    // This function will run when a new sell order is made
    std::map<double, OrderList, std::greater<>>* buyBook = book.getBuyBook();
    std::map<double, OrderList, std::less<>>* sellBook = book.getSellBook();

    if (buyBook->empty() || sellBook->empty()) {
        return;
    }

    while (sellBook->begin()->first <= buyBook->begin()->first) {
        double execution_price = buyBook->begin()->first;
        OrderList orders = sellBook->begin()->second;
        int quantity = orders.getOrders()->begin()->getQuantity();
        while (quantity > 0) {
            if (buyBook->begin()->first != execution_price) {
                break;
            }
            int buy_amount = buyBook->begin()->second.getOrders()->begin()->getQuantity();
            if (buy_amount > quantity) {
                buyBook->begin()->second.getOrders()->begin()->setQuantity(buy_amount - quantity);
                book.deleteSell(*(sellBook->begin()->second.getOrders()->begin()));
                quantity = 0;
            }
            else{
                book.deleteBuy(*(buyBook->begin()->second.getOrders()->begin()));
                if (buy_amount == quantity) {
                    book.deleteSell(*(sellBook->begin()->second.getOrders()->begin()));
                    quantity = 0;
                }
                else {
                    quantity -= buy_amount;
                    book.getSellBook()->begin()->second.getOrders()->begin()->setQuantity(quantity);
                }
            }
        }
    }
}
