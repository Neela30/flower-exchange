#pragma once

#include <cstdint>
#include <string>

#include "model/Enums.h"

namespace flower_exchange {

/**
 * Represents a single client order as it moves through the exchange layers.
 */
class Order {
public:
    Order();
    Order(std::string clientOrderId,
          std::string instrument,
          Side side,
          int quantity,
          double price);
    ~Order();

    const std::string& getOrderId() const;
    const std::string& getClientOrderId() const;
    const std::string& getInstrument() const;
    Side getSide() const;
    int getQuantity() const;
    double getPrice() const;
    int getRemainingQuantity() const;
    std::uint64_t getSequenceNumber() const;

    void setOrderId(std::string orderId);
    void setSequenceNumber(std::uint64_t sequenceNumber);

    bool isBuy() const;
    bool isSell() const;
    bool isFilled() const;
    bool isPartiallyFilled() const;

    /**
     * Reduces open quantity after a future match event.
     */
    void reduceRemainingQuantity(int matchedQty);

private:
    std::string orderId_;
    std::string clientOrderId_;
    std::string instrument_;
    Side side_;
    int quantity_;
    double price_;
    int remainingQuantity_;
    std::uint64_t sequenceNumber_;
};

}  // namespace flower_exchange
