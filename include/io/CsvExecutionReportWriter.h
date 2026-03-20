#pragma once

#include <string>
#include <vector>

#include "model/ExecutionReport.h"

namespace flower_exchange {

/**
 * Serializes execution reports to CSV output.
 */
class CsvExecutionReportWriter {
public:
    CsvExecutionReportWriter();
    ~CsvExecutionReportWriter();

    void writeReports(const std::string& filePath,
                      const std::vector<ExecutionReport>& reports) const;
};

}  // namespace flower_exchange
