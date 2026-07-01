//
// Created by warren on 03-Jun-26.
//
#pragma once

#ifndef CLOB_LOGIC_H
#define CLOB_LOGIC_H
#include "book.h"
#include <string>

class DbWriter;

void matchBuy(Book &book, uint32_t stockId, DbWriter& dbWriter, std::atomic<uint64_t>& transactionId, std::shared_mutex& orderMutex, std::unordered_map<uint64_t, OrderLocation>& locations, const std::string& springUrl);
void matchSell(Book &book, uint32_t stockId, DbWriter& dbWriter, std::atomic<uint64_t>& transactionId, std::shared_mutex& orderMutex, std::unordered_map<uint64_t, OrderLocation>& locations, const std::string& springUrl);
void sendMatchNotification(uint64_t buyOrderId, uint64_t sellOrderId, int price, int quantity, const std::string& springUrl);

#endif //CLOB_LOGIC_H
