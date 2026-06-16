//
// Created by warren on 03-Jun-26.
//

#include <algorithm>
#include <soci/soci.h>

#include "logic.h"
#include "book.h"
#include "transaction.h"

void matchBuy(Book &book, int32_t stockId, soci::session& sql, std::atomic<uint64_t>& transactionId) {
    while (!book.getBuyBook()->empty() && !book.getSellBook()->empty()) {
        const auto buyPriceIt = book.getBuyBook()->begin();
        const auto sellPriceIt = book.getSellBook()->begin();

        if (buyPriceIt->first < sellPriceIt->first) break;

        int price = buyPriceIt->first;

        auto* buyOrders = buyPriceIt->second.getOrders();
        auto* sellOrders = sellPriceIt->second.getOrders();

        while (!buyOrders->empty() && !sellOrders->empty()) {
            auto& buyOrder = buyOrders->front();
            auto& sellOrder = sellOrders->front();

            const uint64_t buyClient = buyOrder.getClient();
            const uint64_t sellClient = sellOrder.getClient();
            const uint64_t buyId = buyOrder.getId();
            const uint64_t sellId = sellOrder.getId();

            int buyQty = buyOrder.getQuantity();
            int sellQty = sellOrder.getQuantity();
            const int fillQty = std::min(buyQty, sellQty);
            buyOrder.setQuantity(buyQty - fillQty);
            sellOrder.setQuantity(sellQty - fillQty);

            const int buyQtyLeft = buyOrder.getQuantity();
            const int sellQtyLeft = sellOrder.getQuantity();
            char buyStatus = buyQtyLeft == 0 ? 'F' : 'P';
            char sellStatus = sellQtyLeft == 0 ? 'F' : 'P';

            uint64_t currentTradeId = transactionId.fetch_add(1, std::memory_order_relaxed);

            //const Transaction transaction(buyClient, sellClient, book.getBuyBook()->begin()->first, fillQty, buyPriceIt->first);

            sql << "INSERT INTO trades (id, buyer, seller, price, quantity, ticker, timestamp) VALUES (:id, :buyer, :seller, :price, :quantity, :ticker, :NOW())",
                soci::use(currentTradeId), soci::use(buyClient), soci::use(sellClient), soci::use(price), soci::use(fillQty), soci::use(stockId);

            sql <<"UPDATE orders SET remaining_quantity = :remaining_quantity, status = :status WHERE id = :id ",
            soci::use(buyQtyLeft), soci::use(buyStatus), soci::use(buyId), soci::use(buyClient);

            sql <<"UPDATE orders SET remaining_quantity = :remaining_quantity, status = :status WHERE id = :id ",
            soci::use(buyQtyLeft), soci::use(sellStatus), soci::use(sellId), soci::use(sellClient);

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

void matchSell(Book &book, int32_t stockId, soci::session& sql, std::atomic<uint64_t>& transactionId) {
    while (!book.getBuyBook()->empty() && !book.getSellBook()->empty()) {
        const auto buyPriceIt = book.getBuyBook()->begin();
        const auto sellPriceIt = book.getSellBook()->begin();

        if (buyPriceIt->first < sellPriceIt->first) break;

        int price = buyPriceIt->first;

        auto* buyOrders = buyPriceIt->second.getOrders();
        auto* sellOrders = sellPriceIt->second.getOrders();

        while (!buyOrders->empty() && !sellOrders->empty()) {
            auto& buyOrder = buyOrders->front();
            auto& sellOrder = sellOrders->front();

            const uint64_t buyClient = buyOrder.getClient();
            const uint64_t sellClient = sellOrder.getClient();
            const uint64_t buyId = buyOrder.getId();
            const uint64_t sellId = sellOrder.getId();

            int buyQty = buyOrder.getQuantity();
            int sellQty = sellOrder.getQuantity();
            const int fillQty = std::min(buyQty, sellQty);

            buyOrder.setQuantity(buyQty - fillQty);
            sellOrder.setQuantity(sellQty - fillQty);

            const int buyQtyLeft = buyOrder.getQuantity();
            const int sellQtyLeft = sellOrder.getQuantity();
            char buyStatus = buyQtyLeft == 0 ? 'F' : 'P';
            char sellStatus = sellQtyLeft == 0 ? 'F' : 'P';

            uint64_t currentTradeId = transactionId.fetch_add(1, std::memory_order_relaxed);

            //const Transaction transaction(buyClient, sellClient, book.getId(), fillQty, sellPriceIt->first);

            sql << "INSERT INTO trades (id, buyer, seller, price, quantity, ticker, timestamp) VALUES (:id, :buyer, :seller, :price, :quantity, :ticker, NOW())",
            soci::use(currentTradeId), soci::use(buyClient), soci::use(sellClient), soci::use(price), soci::use(fillQty), soci::use(stockId);

            sql <<"UPDATE orders SET remaining_quantity = :remaining_quantity, status = :status WHERE id = :id ",
            soci::use(buyQtyLeft), soci::use(buyStatus), soci::use(buyId);

            sql <<"UPDATE orders SET remaining_quantity = :remaining_quantity, status = :status WHERE id = :id ",
            soci::use(buyQtyLeft), soci::use(sellStatus), soci::use(sellId);

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
