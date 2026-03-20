#pragma once

#include <string>
#include <vector>

#include "model/Order.h"

namespace flower_exchange {

/**
 * Converts CSV input into order objects while keeping file concerns out of domain classes.
 */
class CsvOrderReader {
public:
    CsvOrderReader();
    ~CsvOrderReader();

    std::vector<Order> readOrders(const std::string& filePath) const;
    Order parseLine(const std::string& line) const;
};

}  // namespace flower_exchange
