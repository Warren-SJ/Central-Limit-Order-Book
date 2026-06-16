//
// Created by warren on 03-Jun-26.
//

#include <algorithm>
#include <soci/soci.h>

#include "logic.h"
#include "book.h"
#include "journal.h"
#include "transaction.h"

void matchBuy(Book &book, Journal& journal, soci::connection_pool& pool) {
    while (!book.getBuyBook()->empty() && !book.getSellBook()->empty()) {
        const auto buyPriceIt = book.getBuyBook()->begin();
        const auto sellPriceIt = book.getSellBook()->begin();

        if (buyPriceIt->first < sellPriceIt->first) break;

        auto* buyOrders = buyPriceIt->second.getOrders();
        auto* sellOrders = sellPriceIt->second.getOrders();

        while (!buyOrders->empty() && !sellOrders->empty()) {
            auto& buyOrder = buyOrders->front();
            auto& sellOrder = sellOrders->front();

            const uint64_t buyClient = buyOrder.getClient();
            const uint64_t sellClient = sellOrder.getClient();

            int buyQty = buyOrder.getQuantity();
            int sellQty = sellOrder.getQuantity();
            const int fillQty = std::min(buyQty, sellQty);

            buyOrder.setQuantity(buyQty - fillQty);
            sellOrder.setQuantity(sellQty - fillQty);

            const uint64_t buyId = buyOrder.getId();
            const uint64_t sellId = sellOrder.getId();
            const int buyQtyLeft = buyOrder.getQuantity();
            const int sellQtyLeft = sellOrder.getQuantity();

            const Transaction transaction(buyClient, sellClient, book.getBuyBook()->begin()->first, fillQty, buyPriceIt->first);
            journal.logTransaction(transaction);

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

void matchSell(Book &book, Journal& journal, soci::connection_pool& pool) {
    while (!book.getBuyBook()->empty() && !book.getSellBook()->empty()) {
        const auto buyPriceIt = book.getBuyBook()->begin();
        const auto sellPriceIt = book.getSellBook()->begin();

        if (buyPriceIt->first < sellPriceIt->first) break;

        auto* buyOrders = buyPriceIt->second.getOrders();
        auto* sellOrders = sellPriceIt->second.getOrders();

        while (!buyOrders->empty() && !sellOrders->empty()) {
            auto& buyOrder = buyOrders->front();
            auto& sellOrder = sellOrders->front();

            const uint64_t buyClient = buyOrder.getClient();
            const uint64_t sellClient = sellOrder.getClient();

            int buyQty = buyOrder.getQuantity();
            int sellQty = sellOrder.getQuantity();
            const int fillQty = std::min(buyQty, sellQty);

            buyOrder.setQuantity(buyQty - fillQty);
            sellOrder.setQuantity(sellQty - fillQty);

            const uint64_t buyId = buyOrder.getId();
            const uint64_t sellId = sellOrder.getId();
            const int buyQtyLeft = buyOrder.getQuantity();
            const int sellQtyLeft = sellOrder.getQuantity();

            const Transaction transaction(buyClient, sellClient, book.getId(), fillQty, sellPriceIt->first);
            journal.logTransaction(transaction);

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
