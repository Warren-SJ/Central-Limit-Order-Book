cmake_minimum_required(VERSION 4.2)
project(clob)

set(CMAKE_CXX_STANDARD 23)

list(APPEND CMAKE_PREFIX_PATH
        "${CMAKE_BINARY_DIR}/build/Release/build/generators"
)

find_package(Crow CONFIG REQUIRED)
find_package(SOCI REQUIRED COMPONENTS )
find_package(CURL REQUIRED CONFIG)

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
        dbwriter.cpp
        dbwriter.h
        orderLocation.h
)

target_compile_definitions(clob PRIVATE NOMINMAX)

target_link_libraries(clob PRIVATE
        Crow::Crow
        SOCI::soci_core
        SOCI::soci_postgresql
        CURL::libcurl
)

if(WIN32)
    add_custom_command(TARGET clob POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:clob> $<TARGET_RUNTIME_DLLS:clob>
            COMMAND_EXPAND_LISTS
    )
endif()