#pragma once

#include <array>
#include <string_view>

namespace flower_exchange {

/**
 * Centralizes static configuration so validation and routing rules stay cohesive.
 */
inline constexpr std::array<std::string_view, 5> kValidInstruments{
    "Rose",
    "Lavender",
    "Lotus",
    "Tulip",
    "Orchid"
};

inline constexpr int kMinQuantity = 10;
inline constexpr int kMaxQuantity = 1000;
inline constexpr int kQuantityStep = 10;
inline constexpr double kMinPrice = 0.0;

inline constexpr std::string_view kOrdersCsvHeader =
    "client_order_id,instrument,side,quantity,price";
inline constexpr std::string_view kExecutionReportCsvHeader =
    "order_id,client_order_id,instrument,side,status,executed_quantity,executed_price,reason,transaction_time";

}  // namespace flower_exchange
