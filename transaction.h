//
// Created by warren on 15-Jun-26.
//

#ifndef CLOB_TRANSACTION_H
#define CLOB_TRANSACTION_H

#include <chrono>
#include <cstdint>
#include <map>

class Transaction {
public:
    Transaction(uint64_t buyerId, uint64_t sellerId, uint32_t ticker, int quantity, int price);
    uint64_t buyerId;
    uint64_t sellerId;
    uint32_t ticker;
    int quantity;
    int price;
    std::chrono::system_clock::time_point timestamp;
// private:

};



#endif //CLOB_TRANSACTION_H
