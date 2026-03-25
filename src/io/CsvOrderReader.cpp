#include "io/CsvOrderReader.h"

#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>


namespace flower_exchange {

namespace {

constexpr int kInvalidQuantitySentinel = -1;
constexpr double kInvalidPriceSentinel = -1.0;

std::string trim(const std::string& value) {
    std::size_t begin = 0;
    while (begin < value.size() &&
           std::isspace(static_cast<unsigned char>(value[begin])) != 0) {
        ++begin;
    }

    std::size_t end = value.size();
    while (end > begin &&
           std::isspace(static_cast<unsigned char>(value[end - 1])) != 0) {
        --end;
    }

    return value.substr(begin, end - begin);
}

bool isEmptyLine(const std::string& line) {
    for (const char ch : line) {
        if (std::isspace(static_cast<unsigned char>(ch)) == 0) {
            return false;
        }
    }
    return true;
}

int parseIntStrict(const std::string& value, const char* fieldName) {
    (void)fieldName;
    std::size_t parsedCount = 0;
    int parsedValue = 0;

    try {
        parsedValue = std::stoi(value, &parsedCount);
    } catch (const std::exception&) {
        return kInvalidQuantitySentinel;
    }

    if (parsedCount != value.size()) {
        return kInvalidQuantitySentinel;
    }

    return parsedValue;
}

double parseDoubleStrict(const std::string& value, const char* fieldName) {
    (void)fieldName;
    std::size_t parsedCount = 0;
    double parsedValue = 0.0;

    try {
        parsedValue = std::stod(value, &parsedCount);
    } catch (const std::exception&) {
        return kInvalidPriceSentinel;
    }

    if (parsedCount != value.size()) {
        return kInvalidPriceSentinel;
    }

    return parsedValue;
}

Side parseSide(const std::string& value) {
    if (value == "1" || value == "Buy" || value == "buy") {
        return Side::Buy;
    }
    if (value == "2" || value == "Sell" || value == "sell") {
        return Side::Sell;
    }
    return Side::Invalid;
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

        orders.push_back(parseLine(line));
    }

    return orders;
}

Order CsvOrderReader::parseLine(const std::string& line) const {
    std::stringstream stream(line);

    std::string clientOrderId;
    std::string instrument;
    std::string sideToken;
    std::string quantityToken;
    std::string priceToken;

    if (!std::getline(stream, clientOrderId, ',') ||
        !std::getline(stream, instrument, ',') ||
        !std::getline(stream, sideToken, ',') ||
        !std::getline(stream, quantityToken, ',') ||
        !std::getline(stream, priceToken, ',')) {
        throw std::runtime_error("Malformed CSV line");
    }

    clientOrderId = trim(clientOrderId);
    instrument = trim(instrument);
    sideToken = trim(sideToken);
    quantityToken = trim(quantityToken);
    priceToken = trim(priceToken);

    const Side side = parseSide(sideToken);
    const int quantity = parseIntStrict(quantityToken, "quantity");
    const double price = parseDoubleStrict(priceToken, "price");

    return Order(std::move(clientOrderId), std::move(instrument), side, quantity, price);
}

}  // namespace flower_exchange
