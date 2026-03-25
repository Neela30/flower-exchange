#pragma once

#include <string>
#include <vector>

#include "model/Order.h"

namespace flower_exchange {

// Reads CSV order rows and converts them into Order objects.
class CsvOrderReader {
public:
    CsvOrderReader();
    ~CsvOrderReader();

    // Reads all non-empty data rows after the header.
    std::vector<Order> readOrders(const std::string& filePath) const;

    // Parses one CSV data row into an Order.
    Order parseLine(const std::string& line) const;
};

}  // namespace flower_exchange