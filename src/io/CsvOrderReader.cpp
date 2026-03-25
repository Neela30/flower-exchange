#include "io/CsvOrderReader.h"

#include <charconv>
#include <cctype>
#include <fstream>
#include <string_view>
#include <stdexcept>
#include <string>
#include <utility>

namespace flower_exchange {

namespace {

std::string_view trimView(std::string_view value) {
    while (!value.empty() &&
           std::isspace(static_cast<unsigned char>(value.front())) != 0) {
        value.remove_prefix(1);
    }

    while (!value.empty() &&
           std::isspace(static_cast<unsigned char>(value.back())) != 0) {
        value.remove_suffix(1);
    }

    return value;
}

bool isEmptyLine(const std::string& line) {
    for (const char ch : line) {
        if (std::isspace(static_cast<unsigned char>(ch)) == 0) {
            return false;
        }
    }
    return true;
}

int parseIntStrict(std::string_view value, const char* fieldName) {
    value = trimView(value);
    if (value.empty()) {
        throw std::runtime_error(std::string("Invalid integer for ") + fieldName);
    }

    int parsedValue = 0;
    const auto result = std::from_chars(value.data(), value.data() + value.size(), parsedValue);
    if (result.ec != std::errc() || result.ptr != value.data() + value.size()) {
        throw std::runtime_error(std::string("Invalid integer for ") + fieldName);
    }

    return parsedValue;
}

double parseDoubleStrict(std::string_view value, const char* fieldName) {
    value = trimView(value);
    if (value.empty()) {
        throw std::runtime_error(std::string("Invalid decimal for ") + fieldName);
    }

    const std::string valueBuffer(value);
    std::size_t parsedCount = 0;
    double parsedValue = 0.0;

    try {
        parsedValue = std::stod(valueBuffer, &parsedCount);
    } catch (const std::exception&) {
        throw std::runtime_error(std::string("Invalid decimal for ") + fieldName);
    }

    if (parsedCount != valueBuffer.size()) {
        throw std::runtime_error(std::string("Invalid decimal for ") + fieldName);
    }

    return parsedValue;
}

Side parseSide(std::string_view value) {
    value = trimView(value);

    if (value == "1" || value == "Buy" || value == "buy") {
        return Side::Buy;
    }
    if (value == "2" || value == "Sell" || value == "sell") {
        return Side::Sell;
    }
    throw std::runtime_error("Invalid side value");
}

}  // namespace

CsvOrderReader::CsvOrderReader() = default;

CsvOrderReader::~CsvOrderReader() = default;

std::vector<Order> CsvOrderReader::readOrders(const std::string& filePath) const {
    std::ifstream input(filePath);
    if (!input.is_open()) {
        throw std::runtime_error("Unable to open file: " + filePath);
    }

    std::vector<Order> orders;
    input.seekg(0, std::ios::end);
    const std::streamoff fileSize = input.tellg();
    if (fileSize > 0) {
        orders.reserve(static_cast<std::size_t>(fileSize / 32));
    }
    input.seekg(0, std::ios::beg);

    std::string line;
    bool isFirstRow = true;
    while (std::getline(input, line)) {
        // Skip the header row.
        if (isFirstRow) {
            isFirstRow = false;
            continue;
        }

        // Ignore empty rows.
        if (isEmptyLine(line)) {
            continue;
        }

        orders.emplace_back(parseLine(line));
    }

    return orders;
}

Order CsvOrderReader::parseLine(const std::string& line) const {
    std::string_view remaining(line);
    std::string_view fields[5];

    for (int index = 0; index < 4; ++index) {
        const std::size_t commaPosition = remaining.find(',');
        if (commaPosition == std::string_view::npos) {
            throw std::runtime_error("Malformed CSV line");
        }

        fields[index] = remaining.substr(0, commaPosition);
        remaining.remove_prefix(commaPosition + 1);
    }

    const std::size_t priceCommaPosition = remaining.find(',');
    fields[4] = priceCommaPosition == std::string_view::npos
                    ? remaining
                    : remaining.substr(0, priceCommaPosition);

    const std::string_view clientOrderIdView = trimView(fields[0]);
    const std::string_view instrumentView = trimView(fields[1]);
    const std::string_view sideTokenView = trimView(fields[2]);
    const std::string_view quantityTokenView = trimView(fields[3]);
    const std::string_view priceTokenView = trimView(fields[4]);

    const Side side = parseSide(sideTokenView);
    const int quantity = parseIntStrict(quantityTokenView, "quantity");
    const double price = parseDoubleStrict(priceTokenView, "price");

    return Order(std::string(clientOrderIdView),
                 std::string(instrumentView),
                 side,
                 quantity,
                 price);
}

}  // namespace flower_exchange
