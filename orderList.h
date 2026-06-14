//
// Created by warren on 03-Jun-26.
//

#ifndef CLOB_ORDERLIST_H
#define CLOB_ORDERLIST_H

#include <list>

#include "order.h"

class OrderList {
public:
    OrderList();
    typedef std::list<Order>::iterator OrderIterator;
    OrderIterator addOrder(const Order &order);
    void removeOrder(const OrderIterator &it);
    std::list<Order>* getOrders();
private:
    std::list<Order> orders;
};



#endif //CLOB_ORDERLIST_H
