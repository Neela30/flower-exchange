#include <cassert>
#include <string>
#include <vector>

#include "common/IdGenerator.h"
#include "exchange/ExchangeApplication.h"

namespace {

using flower_exchange::ExecStatus;
using flower_exchange::ExchangeApplication;
using flower_exchange::ExecutionReport;
using flower_exchange::IdGenerator;
using flower_exchange::Order;
using flower_exchange::Side;

void testIdGeneratorReturnsDifferentValues() {
    IdGenerator generator;
    const std::string first = generator.nextOrderId();
    const std::string second = generator.nextOrderId();

    assert(first != second);
    assert(!first.empty());
    assert(!second.empty());
}

void testExchangeApplicationRejectsInvalidOrder() {
    ExchangeApplication exchangeApplication;
    const Order invalidOrder("client-1", "InvalidFlower", Side::Buy, 10, 10.0);

    const std::vector<ExecutionReport> reports = exchangeApplication.submitOrder(invalidOrder);

    assert(reports.size() == 1);
    assert(reports.front().getStatus() == ExecStatus::Rejected);
    assert(!reports.front().getReason().empty());
}

void testExchangeApplicationAcceptsValidOrderWithoutCrash() {
    ExchangeApplication exchangeApplication;
    const Order validOrder("client-2", "Rose", Side::Sell, 10, 15.0);

    // Main check: this path should execute without throwing/crashing.
    const std::vector<ExecutionReport> reports = exchangeApplication.submitOrder(validOrder);
    (void)reports;
}

}  // namespace

int main() {
    // Small orchestration checks around order submission flow.
    testIdGeneratorReturnsDifferentValues();
    testExchangeApplicationRejectsInvalidOrder();
    testExchangeApplicationAcceptsValidOrderWithoutCrash();
    return 0;
}
