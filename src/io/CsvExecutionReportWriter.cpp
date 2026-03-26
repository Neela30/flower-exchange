#include "io/CsvExecutionReportWriter.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include "model/Constants.h"
#include "model/Enums.h"

namespace flower_exchange
{

    CsvExecutionReportWriter::CsvExecutionReportWriter() = default;

    CsvExecutionReportWriter::~CsvExecutionReportWriter() = default;

    void CsvExecutionReportWriter::writeReports(
        const std::string &filePath,
        const std::vector<ExecutionReport> &reports) const
    {
        std::ofstream output(filePath);
        if (!output.is_open())
        {
            throw std::runtime_error("Failed to open execution report file: " + filePath);
        }

        std::ostringstream buffer;
        buffer << kExecutionReportCsvHeader << '\n';

        for (const auto &report : reports)
        {
            buffer << report.getOrderId() << ','
                   << report.getClientOrderId() << ','
                   << report.getInstrument() << ','
                   << toString(report.getSide()) << ','
                   << toString(report.getStatus()) << ','
                   << report.getExecutedQuantity() << ','
                   << report.getExecutedPrice() << ','
                   << report.getReason() << ','
                   << report.getTransactionTime() << '\n';
        }

        output << buffer.str();
    }

} // namespace flower_exchange
