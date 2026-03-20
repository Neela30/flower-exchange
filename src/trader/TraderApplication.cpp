#include "trader/TraderApplication.h"

#include <vector>

namespace flower_exchange {

TraderApplication::TraderApplication()
    : reader_(),
      exchangeApplication_(),
      sender_(exchangeApplication_),
      writer_() {}

TraderApplication::~TraderApplication() = default;

void TraderApplication::run(const std::string& inputFile, const std::string& outputFile) {
    const std::vector<Order> orders = reader_.readOrders(inputFile);

    std::vector<ExecutionReport> reports;
    for (const auto& order : orders) {
        auto orderReports = sender_.sendOrder(order);
        reports.insert(reports.end(), orderReports.begin(), orderReports.end());
    }

    writer_.writeReports(outputFile, reports);
}

}  // namespace flower_exchange
