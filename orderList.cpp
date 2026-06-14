//
// Created by warren on 03-Jun-26.
//

#include "orderList.h"

OrderList::OrderList() {
    orders.clear();
}

OrderList::OrderIterator OrderList::addOrder(const Order &order) {
    orders.push_back(order);
    return std::prev(orders.end());
}

void OrderList::removeOrder(const OrderIterator &it) {
    orders.erase(it);
}

std::list<Order>* OrderList::getOrders(){
    return &orders;
}


