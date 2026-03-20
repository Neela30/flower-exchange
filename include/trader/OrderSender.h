#pragma once

#include <vector>

#include "model/ExecutionReport.h"
#include "model/Order.h"

namespace flower_exchange {

class ExchangeApplication;

/**
 * Encapsulates trader-to-exchange submission so the trader layer depends on one seam.
 */
class OrderSender {
public:
    explicit OrderSender(ExchangeApplication& exchangeApplication);
    ~OrderSender();

    /**
     * Submits one order and returns the resulting execution reports.
     */
    std::vector<ExecutionReport> sendOrder(Order order);

private:
    ExchangeApplication& exchangeApplication_;
};

}  // namespace flower_exchange
