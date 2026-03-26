#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ScenarioTestUtils.h"

namespace {

flower_exchange::Order parseWithSideFallback(flower_exchange::CsvOrderReader& reader,
                                             const std::string& line) {
    try {
        return reader.parseLine(line);
    } catch (const std::runtime_error& exception) {
        const std::string message = exception.what();
        if (message.find("Invalid side value") == std::string::npos) {
            throw;
        }

        // For this validation scenario we still want to test invalid side handling
        // through ExchangeApplication even if CsvOrderReader rejects side=3.
        const auto fields = flower_exchange::test_utils::splitCsvSimple(line);
        if (fields.size() != 5) {
            throw std::runtime_error("Malformed CSV line in validation scenario");
        }

        const std::string clientOrderId = flower_exchange::test_utils::trimCopy(fields[0]);
        const std::string instrument = flower_exchange::test_utils::trimCopy(fields[1]);
        const int quantity = std::stoi(flower_exchange::test_utils::trimCopy(fields[3]));
        const double price = std::stod(flower_exchange::test_utils::trimCopy(fields[4]));

        return flower_exchange::Order(
            clientOrderId,
            instrument,
            static_cast<flower_exchange::Side>(3),
            quantity,
            price);
    }
}

void testScenarioExample7Validation() {
    const std::vector<std::string> inputRows{
        "aa13,,1,100,55.00",
        "aa14,Rose,3,100,65.00",
        "aa15,Lavender,2,101,1.00",
        "aa16,Tulip,1,100,-1.00",
        "aa17,Orchid,1,1000,-1.00",
    };

    const std::vector<flower_exchange::test_utils::ComparableReportRow> expected{
        flower_exchange::test_utils::expectedRow("ord1", "aa13", "", 1, "Rejected", 100, "55.00", "Invalid instrument"),
        flower_exchange::test_utils::expectedRow("ord2", "aa14", "Rose", 3, "Rejected", 100, "65.00", "Invalid side"),
        flower_exchange::test_utils::expectedRow("ord3", "aa15", "Lavender", 2, "Rejected", 101, "1.00", "Invalid quantity"),
        flower_exchange::test_utils::expectedRow("ord4", "aa16", "Tulip", 1, "Rejected", 100, "-1.00", "Invalid price"),
        flower_exchange::test_utils::expectedRow("ord5", "aa17", "Orchid", 1, "Rejected", 1000, "-1.00", "Invalid price"),
    };

    const std::filesystem::path csvPath =
        flower_exchange::test_utils::writeScenarioCsv("scenario_example7_validation.csv", inputRows);

    flower_exchange::CsvOrderReader reader;
    flower_exchange::ExchangeApplication exchangeApplication;

    std::unordered_map<std::string, std::pair<int, std::string>> submittedOrderInfo;
    submittedOrderInfo.reserve(inputRows.size());

    std::vector<flower_exchange::ExecutionReport> reports;
    reports.reserve(inputRows.size());

    std::ifstream input(csvPath);
    if (!input.is_open()) {
        throw std::runtime_error("Failed to open validation scenario file");
    }

    std::string line;
    std::getline(input, line);  // header
    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        flower_exchange::Order order = parseWithSideFallback(reader, line);
        submittedOrderInfo[order.getClientOrderId()] = {
            order.getQuantity(),
            flower_exchange::test_utils::formatPrice(order.getPrice())};

        auto orderReports = exchangeApplication.submitOrder(order);
        reports.insert(reports.end(), orderReports.begin(), orderReports.end());
    }

    const auto actual = flower_exchange::test_utils::normalizeReports(reports, submittedOrderInfo);
    flower_exchange::test_utils::compareReportsExact(actual, expected, "Scenario Example 7 Validation");
}

}  // namespace

int main() {
    try {
        testScenarioExample7Validation();
    } catch (const std::exception& exception) {
        std::cerr << "ScenarioExample7ValidationTests failure: " << exception.what() << '\n';
        return 1;
    }

    return 0;
}
