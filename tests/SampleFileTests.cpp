#include "exchange/ExchangeApplication.h"
#include "io/CsvOrderReader.h"
#include "model/Enums.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using flower_exchange::CsvOrderReader;
using flower_exchange::ExchangeApplication;
using flower_exchange::ExecStatus;
using flower_exchange::ExecutionReport;
using flower_exchange::Order;

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

std::filesystem::path projectRoot() {
    const std::filesystem::path sourceFilePath(__FILE__);
    return sourceFilePath.parent_path().parent_path();
}

std::string dataFilePath(const std::string& fileName) {
    return (projectRoot() / "data" / fileName).string();
}

std::vector<ExecutionReport> runScenarioFile(const std::string& fileName) {
    CsvOrderReader reader;
    ExchangeApplication exchangeApplication;

    const std::vector<Order> orders = reader.readOrders(dataFilePath(fileName));
    std::vector<ExecutionReport> reports;

    for (const auto& order : orders) {
        auto orderReports = exchangeApplication.submitOrder(order);
        reports.insert(reports.end(), orderReports.begin(), orderReports.end());
    }

    return reports;
}

int countByStatus(const std::vector<ExecutionReport>& reports, ExecStatus status) {
    return static_cast<int>(std::count_if(
        reports.begin(),
        reports.end(),
        [status](const ExecutionReport& report) { return report.getStatus() == status; }));
}

bool hasTradeAtPrice(const std::vector<ExecutionReport>& reports, double price) {
    constexpr double kEpsilon = 1e-9;
    return std::any_of(
        reports.begin(),
        reports.end(),
        [price, kEpsilon](const ExecutionReport& report) {
            return report.getExecutedQuantity() > 0 &&
                   std::abs(report.getExecutedPrice() - price) < kEpsilon;
        });
}

std::vector<std::string> readDataLines(const std::string& fileName) {
    std::ifstream input(dataFilePath(fileName));
    expect(input.is_open(), "Failed to open scenario file: " + fileName);

    std::vector<std::string> lines;
    std::string line;

    bool firstRow = true;
    while (std::getline(input, line)) {
        if (firstRow) {
            firstRow = false;
            continue;
        }

        if (!line.empty()) {
            lines.push_back(line);
        }
    }

    return lines;
}

void testScenarioANonCrossingOrdersRemainNew() {
    const auto reports = runScenarioFile("sampleOrder1.csv");

    expect(!reports.empty(), "Scenario A should produce execution reports");
    expect(reports.size() == 3, "Scenario A should emit one New report per non-crossing order");
    expect(countByStatus(reports, ExecStatus::New) == 3, "Scenario A should only produce New reports");

    const bool hasExecutedQuantity = std::any_of(
        reports.begin(),
        reports.end(),
        [](const ExecutionReport& report) { return report.getExecutedQuantity() > 0; });
    expect(!hasExecutedQuantity, "Scenario A should not execute any quantity");
}

void testScenarioBAndCFullAndPartialFill() {
    const auto reports = runScenarioFile("sampleOrder2.csv");

    const int fillCount = countByStatus(reports, ExecStatus::Fill);
    const int pfillCount = countByStatus(reports, ExecStatus::Pfill);

    expect(fillCount >= 3, "Scenario B/C should include Fill reports from full and partial matches");
    expect(pfillCount >= 2, "Scenario C should include Pfill reports for the incoming partial fill path");

    const bool hasFullFillTrade = std::any_of(
        reports.begin(),
        reports.end(),
        [](const ExecutionReport& report) {
            return report.getStatus() == ExecStatus::Fill && report.getExecutedQuantity() == 30;
        });
    expect(hasFullFillTrade, "Scenario B should include a full fill quantity of 30");

    const bool hasPartialExecution = std::any_of(
        reports.begin(),
        reports.end(),
        [](const ExecutionReport& report) {
            return report.getStatus() == ExecStatus::Pfill && report.getExecutedQuantity() == 20;
        });
    expect(hasPartialExecution, "Scenario C should include a Pfill trade quantity of 20");

    const bool hasResidualPfill = std::any_of(
        reports.begin(),
        reports.end(),
        [](const ExecutionReport& report) {
            return report.getStatus() == ExecStatus::Pfill && report.getExecutedQuantity() == 0;
        });
    expect(hasResidualPfill, "Scenario C should keep remainder resting and emit residual Pfill status");
}

void testScenarioDRestingPriceExecution() {
    const auto reports = runScenarioFile("sampleOrder3.csv");

    const bool hasAnyTrade = std::any_of(
        reports.begin(),
        reports.end(),
        [](const ExecutionReport& report) { return report.getExecutedQuantity() > 0; });
    expect(hasAnyTrade, "Scenario D should produce trade reports");

    expect(hasTradeAtPrice(reports, 11.0), "Scenario D should trade at the first resting buy price (11.0)");
    expect(hasTradeAtPrice(reports, 10.5), "Scenario D should trade at the second resting buy price (10.5)");

    const bool tradedAtAggressivePrice = hasTradeAtPrice(reports, 8.0);
    expect(!tradedAtAggressivePrice, "Scenario D should not trade at the aggressive incoming sell price");
}

void testScenarioEValidationFailuresProduceRejectedReports() {
    CsvOrderReader reader;
    ExchangeApplication exchangeApplication;

    const auto lines = readDataLines("sampleOrder4.csv");
    expect(lines.size() == 4, "Scenario E dataset should contain four invalid rows");

    int parsedOrderCount = 0;
    int parseFailureCount = 0;
    int rejectedCount = 0;

    for (const auto& line : lines) {
        try {
            Order order = reader.parseLine(line);
            ++parsedOrderCount;

            const auto reports = exchangeApplication.submitOrder(order);
            expect(!reports.empty(), "Validation path should return at least one report");

            for (const auto& report : reports) {
                if (report.getStatus() == ExecStatus::Rejected && !report.getReason().empty()) {
                    ++rejectedCount;
                }
            }
        } catch (const std::runtime_error&) {
            ++parseFailureCount;
        }
    }

    expect(parsedOrderCount == 3, "Scenario E should parse three rows and reject them via validation");
    expect(rejectedCount == 3, "Scenario E should produce three Rejected reports with reasons");
    expect(parseFailureCount == 1, "Scenario E should contain one invalid side encoding row");
}

}  // namespace

int main() {
    try {
        testScenarioANonCrossingOrdersRemainNew();
        testScenarioBAndCFullAndPartialFill();
        testScenarioDRestingPriceExecution();
        testScenarioEValidationFailuresProduceRejectedReports();
    } catch (const std::exception& exception) {
        std::cerr << "SampleFileTests failure: " << exception.what() << '\n';
        return 1;
    }

    return 0;
}
