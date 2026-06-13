#include <iostream>
#include <fstream>
#include <unordered_map>
#include "book.h"
#include "order.h"
#include <crow.h>
#include "soci/soci.h"
#include "soci/postgresql/soci-postgresql.h"

soci::backend_factory const &backEnd = *soci::factory_postgresql();

std::unordered_map<int, Book> books;
int orderId = 1;
std::mutex engine_mutex;

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

    soci::connection_pool pool(db_pool);

    const std::string conn = "dbname=" + db_name + " user=" + db_username + " password=" + db_password + " host=" + db_hostname + " port=" + db_port;
    for (std::size_t i = 0; i < db_pool; ++i) {
        pool.at(i).open(backEnd,conn);
    }

    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/order").methods(crow::HTTPMethod::POST)([&pool](const crow::request& req) {
        soci::session sql(pool);
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
