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
    src/common/IdGenerator.cpp
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

add_executable(large_dataset_generator
    src/tools/LargeDatasetGenerator.cpp
)

target_include_directories(large_dataset_generator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(large_dataset_generator PRIVATE /W4 /permissive-)
else()
    target_compile_options(large_dataset_generator PRIVATE -Wall -Wextra -Wpedantic)
endif()

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

    add_executable(scenario_example1_tests tests/ScenarioExample1Tests.cpp)
    target_link_libraries(scenario_example1_tests PRIVATE flower_exchange_core)
    add_test(NAME ScenarioExample1Tests COMMAND scenario_example1_tests)

    add_executable(scenario_example2_tests tests/ScenarioExample2Tests.cpp)
    target_link_libraries(scenario_example2_tests PRIVATE flower_exchange_core)
    add_test(NAME ScenarioExample2Tests COMMAND scenario_example2_tests)

    add_executable(scenario_example3_tests tests/ScenarioExample3Tests.cpp)
    target_link_libraries(scenario_example3_tests PRIVATE flower_exchange_core)
    add_test(NAME ScenarioExample3Tests COMMAND scenario_example3_tests)

    add_executable(scenario_example4_tests tests/ScenarioExample4Tests.cpp)
    target_link_libraries(scenario_example4_tests PRIVATE flower_exchange_core)
    add_test(NAME ScenarioExample4Tests COMMAND scenario_example4_tests)

    add_executable(scenario_example5_tests tests/ScenarioExample5Tests.cpp)
    target_link_libraries(scenario_example5_tests PRIVATE flower_exchange_core)
    add_test(NAME ScenarioExample5Tests COMMAND scenario_example5_tests)

    add_executable(scenario_example6_tests tests/ScenarioExample6Tests.cpp)
    target_link_libraries(scenario_example6_tests PRIVATE flower_exchange_core)
    add_test(NAME ScenarioExample6Tests COMMAND scenario_example6_tests)

    add_executable(scenario_example7_validation_tests tests/ScenarioExample7ValidationTests.cpp)
    target_link_libraries(scenario_example7_validation_tests PRIVATE flower_exchange_core)
    add_test(NAME ScenarioExample7ValidationTests COMMAND scenario_example7_validation_tests)

    set(SCENARIO_TEST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests/include)
    foreach(scenario_target
            scenario_example1_tests
            scenario_example2_tests
            scenario_example3_tests
            scenario_example4_tests
            scenario_example5_tests
            scenario_example6_tests
            scenario_example7_validation_tests)
        target_include_directories(${scenario_target} PRIVATE ${SCENARIO_TEST_INCLUDE_DIR})
    endforeach()

    add_executable(large_dataset_tests tests/LargeDatasetTests.cpp)
    target_link_libraries(large_dataset_tests PRIVATE flower_exchange_core)

    add_test(NAME GenerateLargeDatasets COMMAND large_dataset_generator)
    set_tests_properties(GenerateLargeDatasets PROPERTIES
        FIXTURES_SETUP large_data_ready
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    add_test(NAME LargeDatasetTests COMMAND large_dataset_tests)
    set_tests_properties(LargeDatasetTests PROPERTIES
        FIXTURES_REQUIRED large_data_ready
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()
