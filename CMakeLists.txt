cmake_minimum_required(VERSION 4.2)
project(clob)

set(CMAKE_CXX_STANDARD 23)

list(APPEND CMAKE_PREFIX_PATH
        "${CMAKE_BINARY_DIR}/build/Debug/generators"
)

find_package(Crow CONFIG REQUIRED)
find_package(SOCI REQUIRED COMPONENTS )

add_executable(clob main.cpp
        order.cpp
        order.h
        book.cpp
        book.h
        logic.cpp
        logic.h
        orderList.cpp
        orderList.h
        transaction.cpp
        transaction.h
        journal.cpp
        journal.h
)

target_link_libraries(clob PRIVATE
        Crow::Crow
        SOCI::soci_core
        SOCI::soci_postgresql
)