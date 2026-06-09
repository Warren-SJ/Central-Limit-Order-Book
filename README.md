# Central Limit Order Book in C++

This repository contains a C++ implementation of a central limit order book (CLOB). The CLOB is a fundamental component of modern financial markets, where it serves as a mechanism for matching buy and sell orders for various financial instruments.

## Features

- **Order Types**: Supports limit orders, market orders, and cancel orders.
- **Order Matching**: Implements a matching engine that efficiently matches incoming orders against existing orders in the order book.
- **Data Structures**: Utilizes efficient data structures to maintain the order book and ensure fast order processing.

## File Structure

- `book.h`: Contains the definition of the order book class and its associated data structures.
- `book.cpp`: Contains the implementation of the order book class and its methods.
- `order.h`: Contains the definition of the order class, which represents individual orders in the order book.
- `order.cpp`: Contains the implementation of the order class and its methods.
- `logic.h`: Contains the definition of the logic class, which implements the core logic for order matching and processing.
- `logic.cpp`: Contains the implementation of the logic class and its methods.
- `main.cpp`: Contains the main function, which serves as the entry point for the application and demonstrates how to use the order book.

## Usage

To compile and run the application, follow these steps:
1. Ensure you have a C++ compiler installed (e.g., g++).
2. Install Conan, a C++ package manager, if you haven't already. You can find installation instructions on the [Conan website](https://conan.io/).
3. Clone the repository to your local machine:
```bash
git clone https://github.com/Warren-SJ/CLOB.git
cd CLOB
```
4. Run the following command to install the required dependencies:
```bash
mkdir build && cd build
conan install .. --output-folder=. --build=missing
```
5. Compile the application using the following command:
```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build .
```
6. Run the compiled application:
```bash
./clob
```
The server will start listening on port 8080.