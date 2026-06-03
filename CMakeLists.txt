cmake_minimum_required(VERSION 4.2)
project(clob)

set(CMAKE_CXX_STANDARD 23)

add_executable(clob main.cpp
        order.cpp
        order.h
        book.cpp
        book.h
        logic.cpp
        logic.h
        orderList.cpp
        orderList.h)
