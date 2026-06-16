//
// Created by warren on 03-Jun-26.
//
#pragma once

#ifndef CLOB_LOGIC_H
#define CLOB_LOGIC_H

namespace soci {
    class connection_pool;
}

class Book;
class Journal;

void matchBuy(Book &book, int32_t stockId, soci::session& sql, std::atomic<uint64_t>& transactionId);
void matchSell(Book &book, int32_t stockId, soci::session& sql, std::atomic<uint64_t>& transactionId);

#endif //CLOB_LOGIC_H
