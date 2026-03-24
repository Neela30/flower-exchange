cmake_minimum_required(VERSION 3.16)

project(FlowerExchange VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

add_library(flower_exchange_core
    src/model/Order.cpp
    src/model/ExecutionReport.cpp
    src/trader/TraderApplication.cpp
    src/trader/OrderSender.cpp
    src/exchange/ExchangeApplication.cpp
    src/exchange/Exchange.cpp
    src/exchange/OrderBook.cpp
    src/exchange/OrderBookSide.cpp
    src/exchange/MatchingEngine.cpp
    src/exchange/Validator.cpp
    src/io/CsvOrderReader.cpp
    src/io/CsvExecutionReportWriter.cpp
    src/io/TimeProvider.cpp
)

target_include_directories(flower_exchange_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(flower_exchange_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(flower_exchange_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(flower_exchange
    src/main.cpp
)

target_link_libraries(flower_exchange
    PRIVATE
        flower_exchange_core
)

if(BUILD_TESTING)
    add_executable(validator_tests tests/ValidatorTests.cpp)
    target_link_libraries(validator_tests PRIVATE flower_exchange_core)
    add_test(NAME ValidatorTests COMMAND validator_tests)

    add_executable(order_book_tests tests/OrderBookTests.cpp)
    target_link_libraries(order_book_tests PRIVATE flower_exchange_core)
    add_test(NAME OrderBookTests COMMAND order_book_tests)

    add_executable(matching_tests tests/MatchingTests.cpp)
    target_link_libraries(matching_tests PRIVATE flower_exchange_core)
    add_test(NAME MatchingTests COMMAND matching_tests)

    add_executable(exchange_tests tests/ExchangeTests.cpp)
    target_link_libraries(exchange_tests PRIVATE flower_exchange_core)
    add_test(NAME ExchangeTests COMMAND exchange_tests)

    add_executable(sample_file_tests tests/SampleFileTests.cpp)
    target_link_libraries(sample_file_tests PRIVATE flower_exchange_core)
    add_test(NAME SampleFileTests COMMAND sample_file_tests)

    add_executable(csv_writer_tests tests/CsvExecutionReportWriterTests.cpp)
    target_link_libraries(csv_writer_tests PRIVATE flower_exchange_core)
    add_test(NAME CsvExecutionReportWriterTests COMMAND csv_writer_tests)

    add_executable(order_book_side_tests tests/OrderBookSideTests.cpp)
    target_link_libraries(order_book_side_tests PRIVATE flower_exchange_core)
    add_test(NAME OrderBookSideTests COMMAND order_book_side_tests)
endif()
