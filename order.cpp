//
// Created by warren on 03-Jun-26.
//

#include "order.h"

Order::Order(const uint64_t id,
             const uint64_t client,
             const int side,
             const double price,
             const int quantity):
             id(id), client(client), side(side), price(price), quantity(quantity)
{

}

uint64_t Order::getId() const {
    return id;
}
