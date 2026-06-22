//
// Created by warren on 03-Jun-26.
//

#include "orderList.h"

OrderList::OrderList(const allocator_type alloc) : orders(alloc) {}

OrderList::OrderList(const OrderList& other, const allocator_type alloc) : orders(other.orders, alloc) {}

OrderList::OrderList(OrderList&& other, const allocator_type alloc) : orders(std::move(other.orders), alloc) {}

OrderList::OrderIterator OrderList::addOrder(const Order &order) {
    orders.push_back(order);
    return std::prev(orders.end());
}

void OrderList::removeOrder(const OrderIterator &it) {
    orders.erase(it);
}

std::pmr::list<Order>* OrderList::getOrders(){
    return &orders;
}
