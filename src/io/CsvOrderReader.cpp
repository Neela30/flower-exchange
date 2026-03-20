#include "io/CsvOrderReader.h"

#include <fstream>
#include <string>
#include <utility>

namespace flower_exchange {

CsvOrderReader::CsvOrderReader() = default;

CsvOrderReader::~CsvOrderReader() = default;

std::vector<Order> CsvOrderReader::readOrders(const std::string& filePath) const {
    std::ifstream input(filePath);
    if (!input.is_open()) {
        // TODO: Decide whether missing input files should raise an exception instead.
        return {};
    }

    std::vector<Order> orders;

    std::string line;
    while (std::getline(input, line)) {
        // TODO: Skip headers, parse fields, and surface malformed rows.
        (void)line;
    }

    return orders;
}

Order CsvOrderReader::parseLine(const std::string& line) const {
    (void)line;
    // TODO: Parse CSV fields into a populated Order instance.
    return {};
}

}  // namespace flower_exchange
