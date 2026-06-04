#include <iostream>
#include "book.h"
#include "order.h"

int main()
{
    Book book(1);
    Order order1(1, 1, 1, 0, 100.0, 10);
    Order order2(2, 2, 1, 0, 101.0, 20);
    Order order3(3, 3, 1, 1, 90.0, 15);
    book.addBuy(order1);
    std::cout <<"Added order 1" << std::endl;
    book.addBuy(order2);
    std::cout <<"Added order 2" << std::endl;
    book.printBook();
    book.addSell(order3);
    std::cout <<"Added order 3" << std::endl;
    book.printBook();
    return 0;
}