#include "trader/TraderApplication.h"

#include <future>
#include <iterator>
#include <map>
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

void TraderApplication::runParallel(const std::string& inputFile, const std::string& outputFile) {
    std::vector<Order> orders = reader_.readOrders(inputFile);
    std::map<std::string, std::vector<Order>> ordersByInstrument;

    for (auto& order : orders) {

        ordersByInstrument[order.getInstrument()].push_back(std::move(order));
    }

    std::vector<std::future<std::vector<ExecutionReport>>> futures;
    futures.reserve(ordersByInstrument.size());

    for (auto& bucketEntry : ordersByInstrument) {
        auto* bucket = &bucketEntry.second;
        futures.emplace_back(std::async(std::launch::async, [this, bucket]() {
            std::vector<ExecutionReport> localReports;
            localReports.reserve(bucket->size());

            // Each worker remains sequential for one instrument to keep price-time priority.
            for (auto& order : *bucket) {
                auto orderReports = exchangeApplication_.submitOrder(std::move(order));
                localReports.insert(
                    localReports.end(),
                    std::make_move_iterator(orderReports.begin()),
                    std::make_move_iterator(orderReports.end()));
            }

            return localReports;
        }));
    }

    // Merge on the main thread. CSV writing remains single-threaded for safety/simplicity.
    std::vector<ExecutionReport> reports;
    reports.reserve(orders.size());
    for (auto& future : futures) {
        auto workerReports = future.get();
        reports.insert(
            reports.end(),
            std::make_move_iterator(workerReports.begin()),
            std::make_move_iterator(workerReports.end()));
    }

    writer_.writeReports(outputFile, reports);
}

}  // namespace flower_exchange
