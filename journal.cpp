//
// Created by warren on 15-Jun-26.
// This code is no longer in use
//

#include "journal.h"

Journal::Journal() {
    id = 0;
}

uint64_t Journal::logTransaction(Transaction transaction) {
    transactions.emplace_back(transaction);
    id++;
    return id;
}
