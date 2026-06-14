//
// Created by warren on 03-Jun-26.
//

#include "logic.h"
#include <algorithm>

void matchBuy(Book &book) {
    while (!book.getBuyBook()->empty() && !book.getSellBook()->empty()) {
        const auto buyPriceIt = book.getBuyBook()->begin();
        const auto sellPriceIt = book.getSellBook()->begin();

        if (buyPriceIt->first < sellPriceIt->first) break;

        auto* buyOrders = buyPriceIt->second.getOrders();
        auto* sellOrders = sellPriceIt->second.getOrders();

        while (!buyOrders->empty() && !sellOrders->empty()) {
            auto& buyOrder = buyOrders->front();
            auto& sellOrder = sellOrders->front();

            int buyQty = buyOrder.getQuantity();
            int sellQty = sellOrder.getQuantity();
            const int fillQty = std::min(buyQty, sellQty);

            buyOrder.setQuantity(buyQty - fillQty);
            sellOrder.setQuantity(sellQty - fillQty);

            const uint64_t buyId = buyOrder.getId();
            const uint64_t sellId = sellOrder.getId();
            const int buyQtyLeft = buyOrder.getQuantity();
            const int sellQtyLeft = sellOrder.getQuantity();

            if (buyQtyLeft == 0) {
                book.deleteBuy(buyId);
            }
            if (sellQtyLeft == 0) {
                book.deleteSell(sellId);
            }
            if (buyQtyLeft == 0 || sellQtyLeft == 0) {
                break;
            }
        }
    }
}

void matchSell(Book &book) {
    while (!book.getBuyBook()->empty() && !book.getSellBook()->empty()) {
        const auto buyPriceIt = book.getBuyBook()->begin();
        const auto sellPriceIt = book.getSellBook()->begin();

        if (buyPriceIt->first < sellPriceIt->first) break;

        auto* buyOrders = buyPriceIt->second.getOrders();
        auto* sellOrders = sellPriceIt->second.getOrders();

        while (!buyOrders->empty() && !sellOrders->empty()) {
            auto& buyOrder = buyOrders->front();
            auto& sellOrder = sellOrders->front();

            int buyQty = buyOrder.getQuantity();
            int sellQty = sellOrder.getQuantity();
            const int fillQty = std::min(buyQty, sellQty);

            buyOrder.setQuantity(buyQty - fillQty);
            sellOrder.setQuantity(sellQty - fillQty);

            const uint64_t buyId = buyOrder.getId();
            const uint64_t sellId = sellOrder.getId();
            const int buyQtyLeft = buyOrder.getQuantity();
            const int sellQtyLeft = sellOrder.getQuantity();

            if (buyQtyLeft == 0) {
                book.deleteBuy(buyId);
            }
            if (sellQtyLeft == 0) {
                book.deleteSell(sellId);
            }
            if (buyQtyLeft == 0 || sellQtyLeft == 0) {
                break;
            }
        }
    }
}
