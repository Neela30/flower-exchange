#include "io/CsvExecutionReportWriter.h"

#include <fstream>

#include "model/Constants.h"
#include "model/Enums.h"

namespace flower_exchange {

CsvExecutionReportWriter::CsvExecutionReportWriter() = default;

CsvExecutionReportWriter::~CsvExecutionReportWriter() = default;

void CsvExecutionReportWriter::writeReports(
    const std::string& filePath,
    const std::vector<ExecutionReport>& reports) const {
    std::ofstream output(filePath);
    if (!output.is_open()) {
        // TODO: Promote this to an exception if output failures should stop the application.
        return;
    }

    output << kExecutionReportCsvHeader << '\n';

    for (const auto& report : reports) {
        output << report.getOrderId() << ','
               << report.getClientOrderId() << ','
               << report.getInstrument() << ','
               << toString(report.getSide()) << ','
               << toString(report.getStatus()) << ','
               << report.getExecutedQuantity() << ','
               << report.getExecutedPrice() << ','
               << report.getReason() << ','
               << report.getTransactionTime() << '\n';
    }
}

}  // namespace flower_exchange
