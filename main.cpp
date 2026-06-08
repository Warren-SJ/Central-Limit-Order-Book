#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "book.h"
#include "order.h"
#include <crow.h>

int parseSide(const std::string& sideStr) {
    if (sideStr == "BUY" || sideStr == "buy" || sideStr == "0") {
        return 0;
    }
    return 1;
}

int main()
{
    std::ifstream file("../../orders.csv");
    if (!file.is_open()) {
        std::cerr << "Failed to open orders.csv" << std::endl;
        return 1;
    }
    std::string line;
    std::unordered_map<int, Book> books;
    int orderId = 1;

    while (std::getline(file, line)) {

        std::istringstream iss(line);
        std::string stockStr;
        std::string clientStr;
        std::string sideStr;
        std::string priceStr;
        std::string quantityStr;

        std::getline(iss, stockStr,',');
        std::getline(iss, clientStr,',');
        std::getline(iss, sideStr,',');
        std::getline(iss, priceStr,',');
        std::getline(iss, quantityStr,',');

        int32_t stockId = std::stoi(stockStr);
        int64_t clientId = std::stoll(clientStr);
        int side = parseSide(sideStr);
        int price = std::stoi(priceStr);
        int quantity = std::stoi(quantityStr);

        if (!books.contains(stockId)) {
            books.emplace(stockId, Book(stockId));
        }

        Book& book = books.at(stockId);

        Order order(orderId++, clientId, stockId, side, price, quantity);
        if (side == 0) {
            book.addBuy(order);
        } else {
            book.addSell(order);
        }
    }
    file.close();

    for (auto&[fst, snd] : books) {
        std::cout << fst << std::endl;
        snd.printBook();
    }
    return 0;
}


