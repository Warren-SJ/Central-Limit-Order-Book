//
// Created by warren on 21-Jun-26.
//


#ifndef CLOB_ORDERLOCATION_H
#define CLOB_ORDERLOCATION_H

#include <cstdint>

struct OrderLocation {
    uint32_t stockId;
    uint64_t clientId;
    char side;
    char status;

    OrderLocation(uint32_t stockId, uint64_t clientId, char side, char status)
        : stockId(stockId), clientId(clientId), side(side), status(status) {}
};

#endif // CLOB_ORDERLOCATION_H

