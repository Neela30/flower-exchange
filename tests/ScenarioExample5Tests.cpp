#include <exception>
#include <iostream>
#include <vector>

#include "test_utils/ScenarioTestUtils.h"

namespace {

void testScenarioExample5() {
    const std::vector<std::string> inputRows{
        "aa13,Rose,1,100,55.00",
        "aa14,Rose,1,100,65.00",
        "aa15,Rose,2,300,1.00",
    };

    const std::vector<flower_exchange::test_utils::ComparableReportRow> expected{
        flower_exchange::test_utils::expectedRow("ord1", "aa13", "Rose", 1, "New", 100, "55.00", ""),
        flower_exchange::test_utils::expectedRow("ord2", "aa14", "Rose", 1, "New", 100, "65.00", ""),
        flower_exchange::test_utils::expectedRow("ord3", "aa15", "Rose", 2, "Pfill", 100, "65.00", ""),
        flower_exchange::test_utils::expectedRow("ord2", "aa14", "Rose", 1, "Fill", 100, "65.00", ""),
        flower_exchange::test_utils::expectedRow("ord3", "aa15", "Rose", 2, "Pfill", 100, "55.00", ""),
        flower_exchange::test_utils::expectedRow("ord1", "aa13", "Rose", 1, "Fill", 100, "55.00", ""),
    };

    const auto actual = flower_exchange::test_utils::runScenarioThroughReaderAndExchange(
        inputRows, "scenario_example5.csv");
    flower_exchange::test_utils::compareReportsExact(actual, expected, "Scenario Example 5");
}

}  // namespace

int main() {
    try {
        testScenarioExample5();
    } catch (const std::exception& exception) {
        std::cerr << "ScenarioExample5Tests failure: " << exception.what() << '\n';
        return 1;
    }

    return 0;
}
