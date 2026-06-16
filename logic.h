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

void matchBuy(Book &book, Journal &journal, soci::connection_pool& pool);
void matchSell(Book &book, Journal &journal, soci::connection_pool& pool);

#endif //CLOB_LOGIC_H
