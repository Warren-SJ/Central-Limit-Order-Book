//
// Created by warren on 03-Jun-26.
//

#include "order.h"

Order::Order(const uint64_t id,
             const uint64_t client,
             const uint32_t book,
             const int side,
             const int price,
             const int quantity):
             id(id), client(client), book(book), side(side), price(price), quantity(quantity)
{

}

uint64_t Order::getClient() const {
    return client;
}

uint64_t Order::getId() const {
    return id;
}

int Order::getPrice() const {
    return price;
}

int Order::getQuantity() const {
    return quantity;
}

void Order::setQuantity(const int newQuantity) {
    quantity = newQuantity;
}
