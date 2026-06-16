//
// Created by warren on 15-Jun-26.
// This code is no longer in use
//

#ifndef CLOB_JOURNAL_H
#define CLOB_JOURNAL_H

#include <vector>
#include <cstdint>
#include "transaction.h"

class Journal {
public:
    Journal();
    uint64_t logTransaction(Transaction transaction);
private:
    std::vector<Transaction> transactions;
    uint64_t id;
};



#endif //CLOB_JOURNAL_H
