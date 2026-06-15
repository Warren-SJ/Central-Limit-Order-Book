//
// Created by warren on 15-Jun-26.
//

#include "journal.h"
#include <iostream>

Journal::Journal() {
    id = 0;
}

uint64_t Journal::logTransaction(Transaction transaction) {
    transactions.emplace_back(transaction);
    id++;
    for (auto& transaction : transactions) {
        // Log transaction details to console for demonstration
        // In a real implementation, this would be written to a durable log
        std::cout << "Logged Transaction - Buyer: " << transaction.buyerId
                  << ", Seller: " << transaction.sellerId
                  << ", Ticker: " << transaction.ticker
                  << ", Quantity: " << transaction.quantity
                  << ", Price: " << transaction.price
                  << std::endl;
    }
    return id;
}
