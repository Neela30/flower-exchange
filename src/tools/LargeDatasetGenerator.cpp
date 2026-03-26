#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>

#include "model/Constants.h"

namespace {

constexpr const char* kHeader = "ClientOrderID,Instrument,Side,Quantity,Price";
constexpr int kBuySide = 1;
constexpr int kSellSide = 2;

std::filesystem::path findProjectRoot(std::filesystem::path startPath) {
    startPath = std::filesystem::absolute(startPath);
    if (std::filesystem::is_regular_file(startPath)) {
        startPath = startPath.parent_path();
    }

    for (std::filesystem::path current = startPath; !current.empty(); current = current.parent_path()) {
        if (std::filesystem::exists(current / "CMakeLists.txt")) {
            return current;
        }
    }

    throw std::runtime_error("Failed to locate project root from: " + startPath.string());
}

std::filesystem::path projectRoot() {
    // Source file lives at <project_root>/src/tools/LargeDatasetGenerator.cpp
    const std::filesystem::path sourceFilePath(__FILE__);
    return findProjectRoot(sourceFilePath);
}

std::ofstream openOutputFile(const std::filesystem::path& outputPath) {
    std::ofstream output(outputPath);
    if (!output.is_open()) {
        throw std::runtime_error("Failed to open output file: " + outputPath.string());
    }
    output << std::fixed << std::setprecision(2);
    output << kHeader << '\n';
    return output;
}

int randomQuantity(std::mt19937& rng) {
    // Valid quantity range: 10..1000 in steps of 10.
    std::uniform_int_distribution<int> steps(1, 100);
    return steps(rng) * 10;
}

int randomSide(std::mt19937& rng) {
    std::uniform_int_distribution<int> sideDist(0, 1);
    return sideDist(rng) == 0 ? kBuySide : kSellSide;
}

double randomPrice(std::mt19937& rng, double lower, double upper) {
    std::uniform_real_distribution<double> dist(lower, upper);
    return dist(rng);
}

void writeRow(std::ofstream& output,
              std::string_view idPrefix,
              std::size_t index,
              std::string_view instrument,
              int side,
              int quantity,
              double price) {
    output << idPrefix << '-' << (index + 1)
           << ',' << instrument
           << ',' << side
           << ',' << quantity
           << ',' << price
           << '\n';
}

void writeRowWithSideToken(std::ofstream& output,
                           std::string_view idPrefix,
                           std::size_t index,
                           std::string_view instrument,
                           std::string_view sideToken,
                           int quantity,
                           double price) {
    output << idPrefix << '-' << (index + 1)
           << ',' << instrument
           << ',' << sideToken
           << ',' << quantity
           << ',' << price
           << '\n';
}

void generateBalanced(const std::filesystem::path& outputPath,
                      std::size_t orderCount,
                      std::uint32_t seed) {
    // Balanced workload with moderate crossing probability.
    std::ofstream output = openOutputFile(outputPath);
    std::mt19937 rng(seed);

    for (std::size_t index = 0; index < orderCount; ++index) {
        const std::string_view instrument =
            flower_exchange::kValidInstruments[index % flower_exchange::kValidInstruments.size()];
        const int side = randomSide(rng);
        const int quantity = randomQuantity(rng);
        const double price = side == kBuySide
                                 ? randomPrice(rng, 95.0, 105.0)
                                 : randomPrice(rng, 100.0, 110.0);
        writeRow(output, "bal", index, instrument, side, quantity, price);
    }
}

void generateMatching(const std::filesystem::path& outputPath,
                      std::size_t orderCount,
                      std::uint32_t seed) {
    // High overlap between buy/sell prices to force frequent matching.
    std::ofstream output = openOutputFile(outputPath);
    std::mt19937 rng(seed);

    for (std::size_t index = 0; index < orderCount; ++index) {
        const std::string_view instrument =
            flower_exchange::kValidInstruments[index % flower_exchange::kValidInstruments.size()];
        const int side = randomSide(rng);
        const int quantity = randomQuantity(rng);
        const double price = side == kBuySide
                                 ? randomPrice(rng, 105.0, 120.0)
                                 : randomPrice(rng, 80.0, 95.0);
        writeRow(output, "match", index, instrument, side, quantity, price);
    }
}

void generateNonCrossing(const std::filesystem::path& outputPath,
                         std::size_t orderCount,
                         std::uint32_t seed) {
    // Separated price bands to keep most orders resting as New.
    std::ofstream output = openOutputFile(outputPath);
    std::mt19937 rng(seed);

    for (std::size_t index = 0; index < orderCount; ++index) {
        const std::string_view instrument =
            flower_exchange::kValidInstruments[index % flower_exchange::kValidInstruments.size()];
        const int side = randomSide(rng);
        const int quantity = randomQuantity(rng);
        const double price = side == kBuySide
                                 ? randomPrice(rng, 80.0, 90.0)
                                 : randomPrice(rng, 110.0, 120.0);
        writeRow(output, "nocross", index, instrument, side, quantity, price);
    }
}

void generatePartialFill(const std::filesystem::path& outputPath,
                         std::size_t orderCount) {
    // Repeating pattern: small resting liquidity then one large aggressive order.
    std::ofstream output = openOutputFile(outputPath);

    for (std::size_t index = 0; index < orderCount; ++index) {
        const std::string_view instrument =
            flower_exchange::kValidInstruments[index % flower_exchange::kValidInstruments.size()];
        const std::size_t slot = index % 6;

        if (slot == 0) {
            writeRow(output, "partial", index, instrument, kSellSide, 10, 100.00);
        } else if (slot == 1) {
            writeRow(output, "partial", index, instrument, kSellSide, 20, 100.50);
        } else if (slot == 2) {
            writeRow(output, "partial", index, instrument, kBuySide, 100, 105.00);
        } else if (slot == 3) {
            writeRow(output, "partial", index, instrument, kBuySide, 10, 100.00);
        } else if (slot == 4) {
            writeRow(output, "partial", index, instrument, kBuySide, 20, 99.50);
        } else {
            writeRow(output, "partial", index, instrument, kSellSide, 100, 95.00);
        }
    }
}

void generateInvalidMix(const std::filesystem::path& outputPath,
                        std::size_t orderCount,
                        std::uint32_t seed) {
    // Mostly valid rows, with controlled invalid rows for parser/validator stress.
    std::ofstream output = openOutputFile(outputPath);
    std::mt19937 rng(seed);

    for (std::size_t index = 0; index < orderCount; ++index) {
        const std::string_view instrument =
            flower_exchange::kValidInstruments[index % flower_exchange::kValidInstruments.size()];
        const bool shouldBeInvalid = (index % 10) >= 8;  // 20% invalid rows

        if (!shouldBeInvalid) {
            const int side = randomSide(rng);
            const int quantity = randomQuantity(rng);
            const double price = side == kBuySide
                                     ? randomPrice(rng, 95.0, 105.0)
                                     : randomPrice(rng, 100.0, 110.0);
            writeRow(output, "mix", index, instrument, side, quantity, price);
            continue;
        }

        switch ((index / 10) % 4) {
            case 0:
                writeRow(output, "mix", index, "InvalidFlower", kBuySide, 100, 100.00);
                break;
            case 1:
                writeRowWithSideToken(output, "mix", index, instrument, "3", 100, 100.00);
                break;
            case 2:
                writeRow(output, "mix", index, instrument, kSellSide, 15, 100.00);
                break;
            default:
                writeRow(output, "mix", index, instrument, kBuySide, 100, 0.00);
                break;
        }
    }
}

void generateHotspot(const std::filesystem::path& outputPath,
                     std::size_t orderCount,
                     std::uint32_t seed) {
    // 85% of the load targets Rose to stress a single order book heavily.
    std::ofstream output = openOutputFile(outputPath);
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> hotspotDist(1, 100);

    for (std::size_t index = 0; index < orderCount; ++index) {
        std::string_view instrument = "Rose";
        if (hotspotDist(rng) > 85) {
            const std::size_t otherIndex =
                1 + ((index / flower_exchange::kValidInstruments.size()) %
                     (flower_exchange::kValidInstruments.size() - 1));
            instrument = flower_exchange::kValidInstruments[otherIndex];
        }

        const int side = randomSide(rng);
        const int quantity = randomQuantity(rng);
        const double price = side == kBuySide
                                 ? randomPrice(rng, 95.0, 105.0)
                                 : randomPrice(rng, 100.0, 110.0);
        writeRow(output, "hot", index, instrument, side, quantity, price);
    }
}

}  // namespace

int main() {
    try {
        const std::filesystem::path outputDirectory = projectRoot() / "generated_data";
        std::filesystem::create_directories(outputDirectory);

        generateBalanced(outputDirectory / "large_balanced_10k.csv", 10000, 1337);
        generateBalanced(outputDirectory / "large_balanced_100k.csv", 100000, 1338);
        generateMatching(outputDirectory / "large_matching_100k.csv", 100000, 2001);
        generateNonCrossing(outputDirectory / "large_non_crossing_100k.csv", 100000, 2002);
        generatePartialFill(outputDirectory / "large_partial_fill_100k.csv", 100000);
        generateInvalidMix(outputDirectory / "large_invalid_mix_100k.csv", 100000, 3001);
        generateHotspot(outputDirectory / "large_hotspot_100k.csv", 100000, 4001);
        generateBalanced(outputDirectory / "large_balanced_1m.csv", 1000000, 5001);

        std::cout << "Large datasets generated under: " << outputDirectory.string() << '\n';
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Dataset generation failed: " << exception.what() << '\n';
    } catch (...) {
        std::cerr << "Dataset generation failed with an unknown error.\n";
    }

    return 1;
}
