#include "exchange/ExchangeApplication.h"
#include "io/CsvOrderReader.h"
#include "model/ExecutionReport.h"
#include "model/Order.h"
#include "model/Enums.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{

    using flower_exchange::CsvOrderReader;
    using flower_exchange::ExchangeApplication;
    using flower_exchange::ExecStatus;
    using flower_exchange::ExecutionReport;
    using flower_exchange::Order;

    constexpr const char *kExpectedHeader = "ClientOrderID,Instrument,Side,Quantity,Price";

    std::filesystem::path projectRoot()
    {
        const std::filesystem::path sourceFilePath(__FILE__);
        return sourceFilePath.parent_path().parent_path();
    }

    std::filesystem::path dataPath(const std::string &fileName)
    {
        return projectRoot() / "generated_data" / fileName;
    }

    void expect(bool condition, const std::string &message)
    {
        if (!condition)
        {
            throw std::runtime_error(message);
        }
    }

    std::string readFirstLine(const std::filesystem::path &path)
    {
        std::ifstream input(path);
        expect(input.is_open(), "Failed to open file: " + path.string());

        std::string firstLine;
        std::getline(input, firstLine);
        return firstLine;
    }

    std::vector<std::string> requiredLargeFiles()
    {
        return {
            "large_balanced_10k.csv",
            "large_balanced_100k.csv",
            "large_matching_100k.csv",
            "large_non_crossing_100k.csv",
            "large_partial_fill_100k.csv",
            "large_invalid_mix_100k.csv",
            "large_hotspot_100k.csv",
            "large_balanced_1m.csv",
        };
    }

    void testLargeDatasetFilesExistAndHaveExpectedHeader()
    {
        for (const auto &fileName : requiredLargeFiles())
        {
            const auto path = dataPath(fileName);
            expect(std::filesystem::exists(path),
                   "Missing generated dataset: " + path.string() +
                       " (run large_dataset_generator first)");
            expect(std::filesystem::is_regular_file(path), "Path is not a regular file: " + path.string());
            expect(std::filesystem::file_size(path) > 0, "Dataset file is empty: " + path.string());
            expect(readFirstLine(path) == kExpectedHeader, "CSV header mismatch in: " + path.string());
        }
    }

    void testCanReadBalanced10kWithCsvOrderReader()
    {
        CsvOrderReader reader;
        const std::vector<Order> orders = reader.readOrders(dataPath("large_balanced_10k.csv").string());

        // Dataset has exactly 10k data rows; parser should read all of them.
        expect(orders.size() == 10000, "Expected 10000 parsed orders for large_balanced_10k.csv");
    }

    void testInvalidMixSubsetValidationBehavior()
    {
        CsvOrderReader reader;
        ExchangeApplication exchangeApplication;
        std::ifstream input(dataPath("large_invalid_mix_100k.csv"));
        expect(input.is_open(), "Failed to open large_invalid_mix_100k.csv");

        std::string line;
        std::getline(input, line); // header

        int rejectedOrders = 0;
        int acceptedOrders = 0;
        int checkedRows = 0;

        // Keep it fast and deterministic: inspect only the first small subset.
        while (checkedRows < 200 && std::getline(input, line))
        {
            if (line.empty())
            {
                continue;
            }

            Order order = reader.parseLine(line);
            const auto reports = exchangeApplication.submitOrder(order);

            bool hasReject = false;
            bool hasNonReject = false;
            for (const auto &report : reports)
            {
                if (report.getStatus() == ExecStatus::Rejected && !report.getReason().empty())
                {
                    hasReject = true;
                }
                else
                {
                    hasNonReject = true;
                }
            }

            if (hasReject)
            {
                ++rejectedOrders;
            }
            if (hasNonReject)
            {
                ++acceptedOrders;
            }

            ++checkedRows;
        }

        expect(checkedRows > 0, "Expected to inspect at least one row in invalid-mix dataset");
        expect(rejectedOrders > 0, "Invalid-mix subset should include validation-rejected rows");
        expect(acceptedOrders > 0, "Invalid-mix subset should also include orders that pass validation");
    }

    void testBalanced10kExchangeApplicationSmoke()
    {
        CsvOrderReader reader;
        ExchangeApplication exchangeApplication;

        const std::vector<Order> orders = reader.readOrders(dataPath("large_balanced_10k.csv").string());
        expect(!orders.empty(), "Smoke test input unexpectedly empty");

        std::vector<ExecutionReport> reports;
        reports.reserve(orders.size());

        for (const auto &order : orders)
        {
            auto orderReports = exchangeApplication.submitOrder(order);
            reports.insert(reports.end(), orderReports.begin(), orderReports.end());
        }

        expect(!reports.empty(), "Smoke test should produce execution reports");
        expect(reports.size() >= orders.size(), "Expected at least one report per submitted order");
    }

} // namespace

int main()
{
    try
    {
        testLargeDatasetFilesExistAndHaveExpectedHeader();
        testCanReadBalanced10kWithCsvOrderReader();
        testInvalidMixSubsetValidationBehavior();
        testBalanced10kExchangeApplicationSmoke();
    }
    catch (const std::exception &exception)
    {
        std::cerr << "LargeDatasetTests failure: " << exception.what() << '\n';
        return 1;
    }

    return 0;
}
