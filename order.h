//
// Created by warren on 03-Jun-26.
//
#ifndef CLOB_ORDER_H
#define CLOB_ORDER_H

#include <cstdint>

class Order {
public:
    Order(uint64_t id, uint64_t client, uint32_t book, char side, int price, int quantity);
    [[nodiscard]] uint64_t getClient() const;
    [[nodiscard]] uint64_t getId() const;
    [[nodiscard]] int getPrice() const;
    [[nodiscard]] int getQuantity() const;
    void setQuantity(int newQuantity);
private:
    uint64_t id;
    uint64_t client;
    uint32_t book;
    char side;
    int price;
    int quantity;
};
#endif //CLOB_ORDER_H
