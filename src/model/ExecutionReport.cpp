#include "model/ExecutionReport.h"

#include <utility>

namespace flower_exchange {

ExecutionReport::ExecutionReport()
    : orderId_(),
      clientOrderId_(),
      instrument_(),
      side_(Side::Buy),
      status_(ExecStatus::New),
      executedQuantity_(0),
      executedPrice_(0.0),
      reason_(),
      transactionTime_() {}

ExecutionReport::ExecutionReport(std::string orderId,
                                 std::string clientOrderId,
                                 std::string instrument,
                                 Side side,
                                 ExecStatus status,
                                 int executedQuantity,
                                 double executedPrice,
                                 std::string reason,
                                 std::string transactionTime)
    : orderId_(std::move(orderId)),
      clientOrderId_(std::move(clientOrderId)),
      instrument_(std::move(instrument)),
      side_(side),
      status_(status),
      executedQuantity_(executedQuantity),
      executedPrice_(executedPrice),
      reason_(std::move(reason)),
      transactionTime_(std::move(transactionTime)) {}

ExecutionReport::~ExecutionReport() = default;

const std::string& ExecutionReport::getOrderId() const {
    return orderId_;
}

const std::string& ExecutionReport::getClientOrderId() const {
    return clientOrderId_;
}

const std::string& ExecutionReport::getInstrument() const {
    return instrument_;
}

Side ExecutionReport::getSide() const {
    return side_;
}

ExecStatus ExecutionReport::getStatus() const {
    return status_;
}

int ExecutionReport::getExecutedQuantity() const {
    return executedQuantity_;
}

double ExecutionReport::getExecutedPrice() const {
    return executedPrice_;
}

const std::string& ExecutionReport::getReason() const {
    return reason_;
}

const std::string& ExecutionReport::getTransactionTime() const {
    return transactionTime_;
}

}  // namespace flower_exchange
