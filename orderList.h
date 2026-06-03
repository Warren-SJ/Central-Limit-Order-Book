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
    void addOrder(const Order &order);
    void deleteOrder(const Order& order);
private:
    std::list<Order> orders;
};



#endif //CLOB_ORDERLIST_H
