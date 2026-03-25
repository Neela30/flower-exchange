#include "trader/TraderApplication.h"

#include <iterator>
#include <utility>
#include <vector>

namespace flower_exchange {

TraderApplication::TraderApplication()
    : reader_(),
      exchangeApplication_(),
      sender_(exchangeApplication_),
      writer_() {}

TraderApplication::~TraderApplication() = default;

void TraderApplication::run(const std::string& inputFile, const std::string& outputFile) {
    std::vector<Order> orders = reader_.readOrders(inputFile);

    std::vector<ExecutionReport> reports;
    reports.reserve(orders.size());

    for (auto& order : orders) {
        auto orderReports = sender_.sendOrder(std::move(order));
        reports.insert(
            reports.end(),
            std::make_move_iterator(orderReports.begin()),
            std::make_move_iterator(orderReports.end()));
    }

    writer_.writeReports(outputFile, reports);
}

}  // namespace flower_exchange
