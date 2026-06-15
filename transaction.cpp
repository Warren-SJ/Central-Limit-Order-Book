//
// Created by warren on 15-Jun-26.
//

#include "transaction.h"

Transaction::Transaction(const uint64_t buyerId, const uint64_t sellerId, const uint32_t ticker, const int quantity, const int price) :
    buyerId(buyerId), sellerId(sellerId), ticker(ticker), quantity(quantity), price(price) {
    std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();
}

