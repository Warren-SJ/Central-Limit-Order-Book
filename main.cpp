#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <atomic>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <crow.h>
#include <soci/soci.h>
#include <soci/postgresql/soci-postgresql.h>
#include <curl/curl.h>

#include "book.h"
#include "order.h"
#include "dbwriter.h"
#include "orderLocation.h"

soci::backend_factory const &backEnd = *soci::factory_postgresql();

std::unordered_map<uint32_t, std::unique_ptr<Book>> books;
std::shared_mutex books_map_mutex;
std::shared_mutex order_locations_mutex;
std::atomic<uint64_t> globalOrderId;
std::atomic<uint64_t> globalTransactionId;
std::unordered_map<uint64_t, OrderLocation> orderLocations;

std::unordered_map<std::string, std::string> loadEnvFile(const std::string& filename) {

     std::unordered_map<std::string, std::string> envVars;
     std::ifstream file(filename);

     if (!file.is_open()) {
         std::cerr << "Error: Could not open environment file: " << filename << std::endl;
         return envVars;
     }

     std::string line;
     while (std::getline(file, line)) {
         if (line.empty() || line[0] == '#') {
             continue;
         }

         if (const std::size_t delimiterPos = line.find('='); delimiterPos != std::string::npos) {
             std::string key = line.substr(0, delimiterPos);
             const std::string value = line.substr(delimiterPos + 1);
             envVars[key] = value;
         }
     }
     return envVars;
 }

char parseSide(const std::string& sideStr) {
     if (sideStr == "b" || sideStr == "B" || sideStr == "BUY" || sideStr == "buy") {
        return 'B';
    }

    if (sideStr == "s" || sideStr == "S" || sideStr == "SELL" || sideStr == "sell") {
        return 'S';
    }

    return '\0';
 }

void addOrder(const Order& order, const uint32_t stockId, const int side, DbWriter& dbWriter, std::atomic<uint64_t>& transactionId, std::shared_mutex& orderMutex, std::unordered_map<uint64_t, OrderLocation>& locations, const std::string& springUrl) {
     Book* book_ptr = nullptr;
     {
         std::shared_lock lock(books_map_mutex);
         if (const auto it = books.find(stockId); it != books.end()) {
             book_ptr = it->second.get();
         }
     }

     if (!book_ptr) {
         std::lock_guard lock(books_map_mutex);
         if (!books.contains(stockId)) {
             books.emplace(stockId, std::make_unique<Book>(stockId));
         }
         book_ptr = books.at(stockId).get();
     }

     std::lock_guard book_lock(book_ptr->getMutex());
     if (side == 'B') {
         book_ptr->addBuy(order, stockId, dbWriter, transactionId, orderMutex, locations, springUrl);
     } else {
         book_ptr->addSell(order, stockId, dbWriter, transactionId, orderMutex, locations, springUrl);
     }
 }

bool deleteOrder(const uint64_t orderId, const uint32_t stockId, const char side) {
    Book* book_ptr = nullptr;
    {
        std::shared_lock lock(books_map_mutex);
        if (const auto it = books.find(stockId); it != books.end()) {
            book_ptr = it->second.get();
        }
    }
    if (!book_ptr) return false;
    std::lock_guard lock(book_ptr->getMutex());
    if (side == 'B') {
        return book_ptr->deleteBuy(orderId) != 0;
    } else {
        return book_ptr->deleteSell(orderId) != 0;
    }
}

bool fetchOrderInfo(const uint64_t orderId, uint32_t& stockId, char& side, uint64_t& client, char& status) {
    std::shared_lock lock(order_locations_mutex);
    try {
        const OrderLocation location = orderLocations.at(orderId);
        stockId = location.stockId;
        side = location.side;
        client = location.clientId;
        status = location.status;
        return true;
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return false;
    }
}

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    auto env = loadEnvFile("../../.env");
    if (env.empty() || !env.contains("DB_USERNAME") || !env.contains("DB_PASSWORD") || !env.contains("DB_HOST") || !env.contains("DB_PORT")) {
        std::cerr << "CRITICAL ERROR: Failed to load environment variables from '../../.env'!" << std::endl;
        std::cerr << "Please verify your working directory and that the file exists." << std::endl;
        return 1;
    }
    const std::string db_name = env["DB_NAME"];
    const std::string db_username = env["DB_USERNAME"];
    const std::string db_password = env["DB_PASSWORD"];
    const std::string db_hostname = env["DB_HOST"];
    const std::string db_port = env["DB_PORT"];
    const int db_pool = std::stoi(env["DB_POOL"]);
    const int crow_port = std::stoi(env["CROW_PORT"]);
    const std::string spring_url = env["SPRING_URL"];

    soci::connection_pool pool(db_pool);

    const std::string conn = "dbname=" + db_name + " user=" + db_username + " password=" + db_password + " host=" + db_hostname + " port=" + db_port;
    for (std::size_t i = 0; i < db_pool; ++i) {
        pool.at(i).open(backEnd,conn);
    }

    uint64_t tempOrderId = 0;
    uint64_t tempTransactionId = 0;
    soci::session sql_session(pool);
    sql_session << "SELECT id FROM orders ORDER BY id DESC LIMIT 1", soci::into(tempOrderId);
    sql_session << "SELECT id FROM trades ORDER BY id DESC LIMIT 1", soci::into(tempTransactionId);
    globalOrderId.store(tempOrderId + 1);
    globalTransactionId.store(tempTransactionId + 1);
    std::cout << "Starting server with globalOrderId: " << globalOrderId.load() << " and globalTransactionId: " << globalTransactionId.load() << std::endl;
    sql_session.close();

    DbWriter dbWriter(pool);

    crow::SimpleApp app;
    // app.loglevel(crow::LogLevel::Warning);

    CROW_ROUTE(app, "/api/order/add").methods(crow::HTTPMethod::POST)([&dbWriter, spring_url](const crow::request& req) {
        const auto json_data = crow::json::load(req.body);

        if (!json_data) {
            return crow::response(400, "Invalid JSON payload");
        }

        try {
            const auto stockIdValue = json_data["ticker"].i();
            const auto clientIdValue = json_data["clientId"].i();
            const std::string sideStr = json_data["side"].s();
            const char side          = parseSide(sideStr);
            const auto priceValue = json_data["price"].i();
            const auto quantityValue = json_data["originalQuantity"].i();

            if (stockIdValue == 0 || clientIdValue == 0 || priceValue == 0 || quantityValue == 0 || side == '\0') {
                throw std::invalid_argument("Missing or invalid order fields");
            }

            const auto stockId   = static_cast<uint32_t>(stockIdValue);
            const auto clientId  = static_cast<uint64_t>(clientIdValue);
            const int price         = static_cast<int>(priceValue);
            const int quantity      = static_cast<int>(quantityValue);

            const uint64_t newOrderId = globalOrderId.fetch_add(1, std::memory_order_relaxed);

            try {
                const Order order(newOrderId, clientId, stockId, side, price, quantity);
                {
                    const OrderLocation newLocation(stockId, clientId, side, 'N');
                    std::unique_lock lock(order_locations_mutex);
                    orderLocations.insert({newOrderId, newLocation});
                }
                addOrder(order, stockId, side, dbWriter, globalTransactionId, order_locations_mutex, orderLocations, spring_url);
                dbWriter.push({DbTask::INSERT_ORDER, newOrderId, 0, clientId, 0, price, quantity, static_cast<uint32_t>(stockId), side, 'N'});
                return crow::response(200, "Order processed!");
            }
            catch (const std::exception& e) {
                return crow::response(400, e.what());
            }

        }
        catch (const std::exception& e) {
            return crow::response(400, e.what());
        }
    });

    CROW_ROUTE(app, "/api/order/edit/<uint>").methods(crow::HTTPMethod::PATCH)([&dbWriter, spring_url](const crow::request& req, const uint64_t orderId) {

        const auto json_data = crow::json::load(req.body);

        if (!json_data) {
            return crow::response(400, "Invalid JSON payload");
        }

        try {
            const int price         = static_cast<int>(json_data["price"].i());
            const int quantity      = static_cast<int>(json_data["originalQuantity"].i());
            try {
                uint64_t clientId;
                char side;
                uint32_t stockId;
                char status;
                if (!fetchOrderInfo(orderId, stockId, side, clientId, status)) {
                    return crow::response(404, "Order not found");
                }
                if (status == 'F' || status == 'C') {
                    return crow::response(400, "Cannot edit a filled or cancelled order");
                }
                if (!deleteOrder(orderId, stockId, side)) {
                    return crow::response(400, "Order already filled or cancelled");
                }
                {
                    std::unique_lock lock(order_locations_mutex);
                    orderLocations.erase(orderId);
                }
                dbWriter.push({DbTask::UPDATE_ORDER_STATUS, orderId, 0, 0, 0, 0, 0, 0, 0, 'C'});

                const uint64_t newOrderId = globalOrderId.fetch_add(1, std::memory_order_relaxed);
                const Order order(newOrderId, clientId, stockId, side, price, quantity);
                {
                    const OrderLocation newLocation(stockId, clientId, side, 'N');
                    std::unique_lock lock(order_locations_mutex);
                    orderLocations.insert({newOrderId, newLocation});
                }
                addOrder(order, stockId, side, dbWriter, globalTransactionId, order_locations_mutex, orderLocations, spring_url);
                dbWriter.push({DbTask::INSERT_ORDER, newOrderId, 0, clientId, 0, price, quantity, static_cast<uint32_t>(stockId), side, 'N'});
                return crow::response(200, "Order processed!");
            }
            catch (const std::exception& e) {
                return crow::response(400, e.what());
            }

        }
        catch (const std::exception& e) {
            return crow::response(400, e.what());
        }
    });

    CROW_ROUTE(app, "/api/order/delete/<uint>").methods(crow::HTTPMethod::Delete)([&dbWriter](const uint64_t orderId) {
    try {
        char side;
        uint32_t stockId;
        char status;
        if (uint64_t client; !fetchOrderInfo(orderId, stockId, side, client, status)) {
            return crow::response(404, "Order not found");
        }
        if (status == 'F' || status == 'C') {
            return crow::response(400, "Cannot delete a filled or cancelled order");
        }
        if (!deleteOrder(orderId, stockId, side)) {
            return crow::response(400, "Order already filled or cancelled");
        }
        {
            std::unique_lock lock(order_locations_mutex);
            orderLocations.erase(orderId);
        }
        dbWriter.push({DbTask::UPDATE_ORDER_STATUS, orderId, 0, 0, 0, 0, 0, 0, 0, 'C'});

        return crow::response(200, "Order processed!");
    }
    catch (const std::exception& e) {
        return crow::response(400, e.what());
    }

});

    app.port(crow_port).multithreaded().run();
    curl_global_cleanup();
}
