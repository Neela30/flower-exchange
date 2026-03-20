#pragma once

#include <string>

#include "model/Enums.h"

namespace flower_exchange {

/**
 * Represents one execution report row emitted by the exchange.
 */
class ExecutionReport {
public:
    ExecutionReport();
    ExecutionReport(std::string orderId,
                    std::string clientOrderId,
                    std::string instrument,
                    Side side,
                    ExecStatus status,
                    int executedQuantity,
                    double executedPrice,
                    std::string reason,
                    std::string transactionTime);
    ~ExecutionReport();

    const std::string& getOrderId() const;
    const std::string& getClientOrderId() const;
    const std::string& getInstrument() const;
    Side getSide() const;
    ExecStatus getStatus() const;
    int getExecutedQuantity() const;
    double getExecutedPrice() const;
    const std::string& getReason() const;
    const std::string& getTransactionTime() const;

private:
    std::string orderId_;
    std::string clientOrderId_;
    std::string instrument_;
    Side side_;
    ExecStatus status_;
    int executedQuantity_;
    double executedPrice_;
    std::string reason_;
    std::string transactionTime_;
};

}  // namespace flower_exchange
