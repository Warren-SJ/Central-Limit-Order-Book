//
// Created by warren on 03-Jun-26.
//

#ifndef CLOB_ORDERLIST_H
#define CLOB_ORDERLIST_H

#include <list>
#include <memory_resource>

#include "order.h"

class OrderList {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;

    explicit OrderList(allocator_type alloc = allocator_type{});
    OrderList(const OrderList& other) = default;
    OrderList(OrderList&& other) = default;
    OrderList(const OrderList& other, allocator_type alloc);
    OrderList(OrderList&& other, allocator_type alloc);

    typedef std::pmr::list<Order>::iterator OrderIterator;
    OrderIterator addOrder(const Order &order);
    void removeOrder(const OrderIterator &it);
    std::pmr::list<Order>* getOrders();
private:
    std::pmr::list<Order> orders;
};



#endif //CLOB_ORDERLIST_H
