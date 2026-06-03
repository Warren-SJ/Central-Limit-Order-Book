#include <iostream>
#include "book.h"
#include "order.h"

int main()
{
    Book book(1);
    Order order1(1, 1, 1, 0, 100.0, 10);
    Order order2(2, 2, 1, 0, 101.0, 20);
    Order order3(3, 3, 1, 1, 120.0, 15);
    book.addBuy(order1);
    book.addBuy(order2);
    book.addSell(order3);
    book.printBook();
}