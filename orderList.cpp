//
// Created by warren on 03-Jun-26.
//

#include "orderList.h"

OrderList::OrderList() {
    orders.clear();
}

void OrderList::addOrder(const Order &order) {
    orders.push_back(order);
}

void OrderList::deleteOrder(const uint64_t id) {
    for (auto it = orders.begin(); it != orders.end(); ++it) {
        if (it->getId() == id) {
            orders.erase(it);
            break;
        }
    }
}

std::list<Order>* OrderList::getOrders(){
    return &orders;
}


