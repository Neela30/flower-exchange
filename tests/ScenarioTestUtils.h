#pragma once

#include <cctype>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "exchange/ExchangeApplication.h"
#include "io/CsvOrderReader.h"
#include "model/Enums.h"
#include "model/ExecutionReport.h"
#include "model/Order.h"

namespace flower_exchange::test_utils {

struct ComparableReportRow {
    std::string orderId;
    std::string clientOrderId;
    std::string instrument;
    std::string side;
    std::string status;
    int executedQuantity;
    std::string executedPrice;
    std::string reason;
};

inline std::string formatPrice(double price) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << price;
    return stream.str();
}

inline std::string trimCopy(const std::string& value) {
    std::size_t begin = 0;
    while (begin < value.size() && std::isspace(static_cast<unsigned char>(value[begin])) != 0) {
        ++begin;
    }

    std::size_t end = value.size();
    while (end > begin && std::isspace(static_cast<unsigned char>(value[end - 1])) != 0) {
        --end;
    }

    return value.substr(begin, end - begin);
}

inline ComparableReportRow expectedRow(const std::string& orderId,
                                       const std::string& clientOrderId,
                                       const std::string& instrument,
                                       int side,
                                       const std::string& status,
                                       int executedQuantity,
                                       const std::string& executedPrice,
                                       const std::string& reason) {
    return ComparableReportRow{
        orderId,
        clientOrderId,
        instrument,
        std::to_string(side),
        status,
        executedQuantity,
        executedPrice,
        reason};
}

inline std::string rowToString(const ComparableReportRow& row) {
    std::ostringstream stream;
    stream << row.orderId << ','
           << row.clientOrderId << ','
           << row.instrument << ','
           << row.side << ','
           << row.status << ','
           << row.executedQuantity << ','
           << row.executedPrice << ','
           << row.reason;
    return stream.str();
}

inline std::filesystem::path writeScenarioCsv(const std::string& fileName,
                                              const std::vector<std::string>& dataRows) {
    const std::filesystem::path scenarioDir =
        std::filesystem::temp_directory_path() / "flower_exchange_scenario_tests";
    std::filesystem::create_directories(scenarioDir);
    const std::filesystem::path path = scenarioDir / fileName;
    std::ofstream output(path, std::ios::trunc);
    if (!output.is_open()) {
        throw std::runtime_error("Failed to write scenario CSV: " + path.string());
    }

    output << "Client Order ID,Instrument,Side,Quantity,Price\n";
    for (const auto& row : dataRows) {
        output << row << '\n';
    }
    return path;
}

inline std::vector<std::string> splitCsvSimple(const std::string& line) {
    std::vector<std::string> parts;
    std::size_t start = 0;
    while (start <= line.size()) {
        const std::size_t commaPos = line.find(',', start);
        if (commaPos == std::string::npos) {
            parts.push_back(line.substr(start));
            break;
        }
        parts.push_back(line.substr(start, commaPos - start));
        start = commaPos + 1;
    }
    return parts;
}

inline std::vector<ComparableReportRow> normalizeReports(
    const std::vector<ExecutionReport>& reports,
    const std::unordered_map<std::string, std::pair<int, std::string>>& submittedOrderInfo) {
    std::vector<ComparableReportRow> normalized;
    normalized.reserve(reports.size());

    for (const auto& report : reports) {
        const std::string status = toString(report.getStatus());
        int executedQuantity = report.getExecutedQuantity();
        std::string executedPrice = formatPrice(report.getExecutedPrice());
        const std::string reason = report.getReason();

        // Ignore residual synthetic row used by this codebase to represent resting remainders.
        if (status == "Pfill" && executedQuantity == 0 && reason.empty()) {
            continue;
        }

        // For New status, compare meaningful resting details (original order qty/price).
        if (status == "New" && executedQuantity == 0 && reason.empty()) {
            const auto infoIt = submittedOrderInfo.find(report.getClientOrderId());
            if (infoIt == submittedOrderInfo.end()) {
                throw std::runtime_error("Missing submitted-order info for: " + report.getClientOrderId());
            }
            executedQuantity = infoIt->second.first;
            executedPrice = infoIt->second.second;
        }

        normalized.push_back(ComparableReportRow{
            report.getOrderId(),
            report.getClientOrderId(),
            report.getInstrument(),
            std::to_string(static_cast<int>(report.getSide())),
            status,
            executedQuantity,
            executedPrice,
            reason});
    }

    return normalized;
}

inline std::vector<ComparableReportRow> runScenarioThroughReaderAndExchange(
    const std::vector<std::string>& dataRows,
    const std::string& fileName) {
    const std::filesystem::path csvPath = writeScenarioCsv(fileName, dataRows);

    CsvOrderReader reader;
    ExchangeApplication exchangeApplication;

    std::vector<Order> orders = reader.readOrders(csvPath.string());
    std::unordered_map<std::string, std::pair<int, std::string>> submittedOrderInfo;
    submittedOrderInfo.reserve(orders.size());

    std::vector<ExecutionReport> reports;
    reports.reserve(orders.size() * 2);

    for (const auto& order : orders) {
        submittedOrderInfo[order.getClientOrderId()] = {order.getQuantity(), formatPrice(order.getPrice())};
        auto orderReports = exchangeApplication.submitOrder(order);
        reports.insert(reports.end(), orderReports.begin(), orderReports.end());
    }

    return normalizeReports(reports, submittedOrderInfo);
}

inline void compareReportsExact(const std::vector<ComparableReportRow>& actual,
                                const std::vector<ComparableReportRow>& expected,
                                const std::string& scenarioName) {
    if (actual.size() != expected.size()) {
        std::ostringstream stream;
        stream << scenarioName << ": expected " << expected.size()
               << " rows, got " << actual.size();
        throw std::runtime_error(stream.str());
    }

    for (std::size_t index = 0; index < expected.size(); ++index) {
        const std::string expectedRowString = rowToString(expected[index]);
        const std::string actualRowString = rowToString(actual[index]);
        if (expectedRowString != actualRowString) {
            std::ostringstream stream;
            stream << scenarioName << ": row mismatch at index " << index
                   << "\n  expected: " << expectedRowString
                   << "\n  actual:   " << actualRowString;
            throw std::runtime_error(stream.str());
        }
    }
}

}  // namespace flower_exchange::test_utils
