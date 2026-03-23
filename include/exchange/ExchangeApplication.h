#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "common/IdGenerator.h"
#include "exchange/Exchange.h"
#include "exchange/Validator.h"
#include "io/TimeProvider.h"
#include "model/ExecutionReport.h"
#include "model/Order.h"

namespace flower_exchange {

/**
 * Exchange-side application service that assigns metadata, validates, and delegates processing.
 */
class ExchangeApplication {
public:
    ExchangeApplication();
    ~ExchangeApplication();

    // Assigns exchange metadata, validates, and routes one order.
    std::vector<ExecutionReport> submitOrder(Order order);

    // Builds a rejected execution report for a failed order.
    ExecutionReport createRejectedReport(const Order& order, const std::string& reason) const;

private:
    Validator validator_;
    Exchange exchange_;
    IdGenerator idGenerator_;
    TimeProvider timeProvider;
    std::uint64_t nextSequenceNumber;
};

}  // namespace flower_exchange
