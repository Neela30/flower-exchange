#pragma once

#include <string>

namespace flower_exchange {

/**
 * Defines the supported order sides for the exchange domain.
 */
enum class Side {
    Buy = 1,
    Sell = 2,
    Invalid = 3
};

/**
 * Defines the supported execution outcomes for generated reports.
 */
enum class ExecStatus {
    New,
    Rejected,
    Fill,
    Pfill
};

/**
 * Converts a side enum into a CSV-friendly string token.
 */
inline std::string toString(Side side) {
    switch (side) {
        case Side::Buy:
            return "Buy";
        case Side::Sell:
            return "Sell";
        default:
            return "Unknown";
    }
}

/**
 * Converts an execution status enum into a CSV-friendly string token.
 */
inline std::string toString(ExecStatus status) {
    switch (status) {
        case ExecStatus::New:
            return "New";
        case ExecStatus::Rejected:
            return "Rejected";
        case ExecStatus::Fill:
            return "Fill";
        case ExecStatus::Pfill:
            return "Pfill";
        default:
            return "Unknown";
    }
}

}  // namespace flower_exchange
