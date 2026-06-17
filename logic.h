//
// Created by warren on 03-Jun-26.
//
#pragma once

#ifndef CLOB_LOGIC_H
#define CLOB_LOGIC_H
#include "book.h"

class DbWriter;

void matchBuy(Book &book, int32_t stockId, DbWriter& dbWriter, std::atomic<uint64_t>& transactionId);
void matchSell(Book &book, int32_t stockId, DbWriter& dbWriter, std::atomic<uint64_t>& transactionId);

#endif //CLOB_LOGIC_H
