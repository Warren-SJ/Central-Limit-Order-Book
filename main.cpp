#include <iostream>
#include <fstream>
#include <unordered_map>
#include "book.h"
#include "order.h"
#include <crow.h>

std::unordered_map<int, Book> books;
int orderId = 1;
std::mutex engine_mutex;

int parseSide(const std::string& sideStr) {
    if (sideStr == "BUY" || sideStr == "buy" || sideStr == "0") {
        return 0;
    }
    return 1;
}

void addOrder(const Order& order, const int32_t stockId, const int side) {
    if (!books.contains(stockId)) {
        books.emplace(stockId, Book(stockId));
    }

    Book& book = books.at(stockId);
    if (side == 0) {
        book.addBuy(order);
    } else {
        book.addSell(order);
    }

    book.printBook();
}

int main() {

    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/order").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        const auto json_data = crow::json::load(req.body);

        if (!json_data) {
            return crow::response(400, "Invalid JSON payload");
        }

        try {
            const auto stockId   = static_cast<int32_t>(json_data["stock"].i());
            const auto clientId  = json_data["client"].i();
            const std::string sideStr = json_data["side"].s();
            const int side          = parseSide(sideStr);
            const int price         = static_cast<int>(json_data["price"].i());
            const int quantity      = static_cast<int>(json_data["quantity"].i());

            std::lock_guard<std::mutex> lock(engine_mutex);
            const Order order(orderId++, clientId, stockId, side, price, quantity);
            addOrder(order, stockId, side);

            crow::json::wvalue response_body;
            response_body["status"] = "success";
            response_body["message"] = "Order processed";

            return crow::response(200, response_body);

        } catch (const std::exception& e) {
            return crow::response(400, e.what());
        }
    });

    app.port(8080).multithreaded().run();

}


