//
// Created by warren on 03-Jun-26.
//

#ifndef CLOB_ORDER_H
#define CLOB_ORDER_H
#include <cstdint>

class Order {
public:
    Order(uint64_t id, uint64_t client, uint32_t book, int side, double price, int quantity);
    uint64_t getClient() const;
    uint64_t getId() const;
    double getPrice() const;
    int getQuantity() const;
private:
    uint64_t id;
    uint64_t client;
    uint32_t book;
    int side;
    double price;
    int quantity;
};
#endif //CLOB_ORDER_H
