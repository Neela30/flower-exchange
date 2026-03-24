#include "io/CsvExecutionReportWriter.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "model/Constants.h"
#include "model/ExecutionReport.h"
#include "model/Enums.h"

namespace
{

    using flower_exchange::CsvExecutionReportWriter;
    using flower_exchange::ExecStatus;
    using flower_exchange::ExecutionReport;
    using flower_exchange::Side;

    std::vector<std::string> readLines(const std::filesystem::path &path)
    {
        std::vector<std::string> lines;
        std::ifstream input(path);
        std::string line;
        while (std::getline(input, line))
        {
            lines.emplace_back(line);
        }
        return lines;
    }

    std::string toCsvRow(const ExecutionReport &report)
    {
        std::ostringstream oss;
        oss << report.getOrderId() << ','
            << report.getClientOrderId() << ','
            << report.getInstrument() << ','
            << flower_exchange::toString(report.getSide()) << ','
            << flower_exchange::toString(report.getStatus()) << ','
            << report.getExecutedQuantity() << ','
            << report.getExecutedPrice() << ','
            << report.getReason() << ','
            << report.getTransactionTime();
        return oss.str();
    }

    void testSuccessfulWrite()
    {
        CsvExecutionReportWriter writer;
        std::vector<ExecutionReport> reports;
        reports.emplace_back("order-1",
                             "client-1",
                             "Rose",
                             Side::Buy,
                             ExecStatus::Fill,
                             100,
                             12.5,
                             "Matched",
                             "2024-01-01T00:00:00Z");
        reports.emplace_back("order-2",
                             "client-2",
                             "Tulip",
                             Side::Sell,
                             ExecStatus::Pfill,
                             40,
                             10.0,
                             "Partial",
                             "2024-01-01T00:01:00Z");

        const auto tempFile = std::filesystem::temp_directory_path() / "flower_exchange_csv_writer_test.csv";
        if (std::filesystem::exists(tempFile))
        {
            std::filesystem::remove(tempFile);
        }

        writer.writeReports(tempFile.string(), reports);

        const auto lines = readLines(tempFile);
        if (lines.size() != reports.size() + 1)
        {
            throw std::runtime_error("CSV output line count mismatch");
        }
        if (lines.front() != std::string(flower_exchange::kExecutionReportCsvHeader))
        {
            throw std::runtime_error("CSV header mismatch");
        }
        if (lines[1] != toCsvRow(reports[0]) || lines[2] != toCsvRow(reports[1]))
        {
            throw std::runtime_error("CSV body mismatch");
        }

        std::filesystem::remove(tempFile);
    }

    void testThrowsWhenPathIsDirectory()
    {
        CsvExecutionReportWriter writer;
        std::vector<ExecutionReport> reports;
        reports.emplace_back("order-1",
                             "client-1",
                             "Rose",
                             Side::Buy,
                             ExecStatus::Fill,
                             100,
                             12.5,
                             "Matched",
                             "2024-01-01T00:00:00Z");

        bool threw = false;
        try
        {
            writer.writeReports(std::filesystem::temp_directory_path().string(), reports);
        }
        catch (const std::runtime_error &)
        {
            threw = true;
        }

        if (!threw)
        {
            throw std::runtime_error("Expected runtime_error when path is a directory");
        }
    }

} // namespace

int main()
{
    try
    {
        testSuccessfulWrite();
        testThrowsWhenPathIsDirectory();
    }
    catch (...)
    {
        return 1;
    }
    return 0;
}
