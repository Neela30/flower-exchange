#pragma once

#include <string>

#include "exchange/ExchangeApplication.h"
#include "io/CsvExecutionReportWriter.h"
#include "io/CsvOrderReader.h"
#include "trader/OrderSender.h"

namespace flower_exchange {

/**
 * Coordinates the end-to-end trader workflow from input file to execution report output.
 */
class TraderApplication {
public:
    TraderApplication();
    ~TraderApplication();

    /**
     * Runs the trader-side orchestration for a single input/output pair.
     */
    void run(const std::string& inputFile, const std::string& outputFile);

private:
    CsvOrderReader reader_;
    ExchangeApplication exchangeApplication_;
    OrderSender sender_;
    CsvExecutionReportWriter writer_;
};

}  // namespace flower_exchange
