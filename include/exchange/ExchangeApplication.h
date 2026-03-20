#pragma once

#include <cstdint>
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

    std::vector<ExecutionReport> submitOrder(Order order);
    ExecutionReport createRejectedReport(const Order& order, const std::string& reason) const;

private:
    Validator validator_;
    Exchange exchange_;
    IdGenerator idGenerator_;
    TimeProvider timeProvider_;
    std::uint64_t nextSequenceNumber_;
};

}  // namespace flower_exchange
