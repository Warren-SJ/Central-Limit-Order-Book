//
// Created by warren on 03-Jun-26.
//

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <algorithm>
#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <thread>

#include "logic.h"
#include "book.h"
#include "transaction.h"
#include "dbwriter.h"

void matchBuy(Book &book, const uint32_t stockId, DbWriter& dbWriter, std::atomic<uint64_t>& transactionId, std::shared_mutex& orderMutex, std::unordered_map<uint64_t, OrderLocation>& locations, const std::string& springUrl) {
    while (!book.getBuyBook()->empty() && !book.getSellBook()->empty()) {
        const auto buyPriceIt = book.getBuyBook()->begin();
        const auto sellPriceIt = book.getSellBook()->begin();

        if (buyPriceIt->first < sellPriceIt->first) break;

        const int price = buyPriceIt->first;

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
            const char buyStatus = buyQtyLeft == 0 ? 'F' : 'P';
            const char sellStatus = sellQtyLeft == 0 ? 'F' : 'P';

            const uint64_t currentTradeId = transactionId.fetch_add(1, std::memory_order_relaxed);
            {
                std::unique_lock<std::shared_mutex> lock(orderMutex);
                locations.at(buyId).status = buyStatus;
                locations.at(sellId).status = sellStatus;
            }
            dbWriter.push({DbTask::INSERT_TRADE, 0, currentTradeId, buyClient, sellClient, price, fillQty, static_cast<uint32_t>(stockId)});
            dbWriter.push({DbTask::UPDATE_ORDER_QUANTITY, buyId, 0, 0, 0, 0, buyQtyLeft, 0, 0, buyStatus});
            dbWriter.push({DbTask::UPDATE_ORDER_QUANTITY, sellId, 0, 0, 0, 0, sellQtyLeft, 0, 0, sellStatus});

            sendMatchNotification(buyId, sellId, price, fillQty, springUrl);

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

void matchSell(Book &book, const uint32_t stockId, DbWriter& dbWriter, std::atomic<uint64_t>& transactionId, std::shared_mutex& orderMutex, std::unordered_map<uint64_t, OrderLocation>& locations, const std::string& springUrl) {
    while (!book.getBuyBook()->empty() && !book.getSellBook()->empty()) {
        const auto buyPriceIt = book.getBuyBook()->begin();
        const auto sellPriceIt = book.getSellBook()->begin();

        if (buyPriceIt->first < sellPriceIt->first) break;

        const int price = buyPriceIt->first;

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
            const char buyStatus = buyQtyLeft == 0 ? 'F' : 'P';
            const char sellStatus = sellQtyLeft == 0 ? 'F' : 'P';

            const uint64_t currentTradeId = transactionId.fetch_add(1, std::memory_order_relaxed);
            {
                std::unique_lock<std::shared_mutex> lock(orderMutex);
                locations.at(buyId).status = buyStatus;
                locations.at(sellId).status = sellStatus;
            }

            dbWriter.push({DbTask::INSERT_TRADE, 0, currentTradeId, buyClient, sellClient, price, fillQty, static_cast<uint32_t>(stockId)});
            dbWriter.push({DbTask::UPDATE_ORDER_QUANTITY, buyId, 0, 0, 0, 0, buyQtyLeft, 0, 0, buyStatus});
            dbWriter.push({DbTask::UPDATE_ORDER_QUANTITY, sellId, 0, 0, 0, 0, sellQtyLeft, 0, 0, sellStatus});

            sendMatchNotification(buyId, sellId, price, fillQty, springUrl);

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

void sendMatchNotification(const uint64_t buyOrderId, const uint64_t sellOrderId, const int price, const int quantity, const std::string& springUrl) {
    if (springUrl.empty()) {
        return;
    }

    std::thread([buyOrderId, sellOrderId, price, quantity, springUrl]() {
        CURL* curl = curl_easy_init();
        if (!curl) {
            std::cerr << "Error: Failed to initialize CURL" << std::endl;
            return;
        }

        std::stringstream jsonPayload;
        jsonPayload << "{"
                    << "\"buyOrderId\":" << buyOrderId << ","
                    << "\"sellOrderId\":" << sellOrderId << ","
                    << "\"price\":" << price << ","
                    << "\"quantity\":" << quantity
                    << "}";

        const std::string payload = jsonPayload.str();
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, springUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Error: Failed to send notification to Spring URL: " << curl_easy_strerror(res) << std::endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }).detach();
}
