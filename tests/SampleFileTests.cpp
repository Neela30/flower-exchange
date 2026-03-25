#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "io/CsvOrderReader.h"

namespace {

using flower_exchange::CsvOrderReader;
using flower_exchange::Order;

std::string sampleFilePath() {
    const std::filesystem::path sourceFilePath(__FILE__);
    const std::filesystem::path projectRoot = sourceFilePath.parent_path().parent_path();
    return (projectRoot / "data" / "sampleOrder1.csv").string();
}

void ensureSampleFileHasData(const std::string& path) {
    std::ofstream output(path, std::ios::trunc);
    assert(output.is_open());

    // Minimal valid CSV with one data row.
    output << "client_order_id,instrument,side,quantity,price\n";
    output << "client-1,Rose,Buy,10,12.5\n";
}

void testReadSampleFileDoesNotFail() {
    const std::string path = sampleFilePath();
    ensureSampleFileHasData(path);

    CsvOrderReader reader;
    const std::vector<Order> orders = reader.readOrders(path);

    assert(!orders.empty());
}

void testParsedOrderCountGreaterThanZero() {
    const std::string path = sampleFilePath();
    ensureSampleFileHasData(path);

    CsvOrderReader reader;
    const std::vector<Order> orders = reader.readOrders(path);

    assert(orders.size() > 0);
}

void testFirstOrderHasNonEmptyClientIdAndInstrument() {
    const std::string path = sampleFilePath();
    ensureSampleFileHasData(path);

    CsvOrderReader reader;
    const std::vector<Order> orders = reader.readOrders(path);

    assert(!orders.empty());
    assert(!orders.front().getClientOrderId().empty());
    assert(!orders.front().getInstrument().empty());
}

}  // namespace

int main() {
    // Basic integration checks for CSV reading behavior.
    testReadSampleFileDoesNotFail();
    testParsedOrderCountGreaterThanZero();
    testFirstOrderHasNonEmptyClientIdAndInstrument();
    return 0;
}
