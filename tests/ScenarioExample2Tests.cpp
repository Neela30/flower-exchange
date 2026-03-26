#include <exception>
#include <iostream>
#include <vector>

#include "test_utils/ScenarioTestUtils.h"

namespace {

void testScenarioExample2() {
    const std::vector<std::string> inputRows{
        "aa13,Rose,2,100,55.00",
        "aa14,Rose,2,100,45.00",
        "aa15,Rose,1,100,35.00",
    };

    const std::vector<flower_exchange::test_utils::ComparableReportRow> expected{
        flower_exchange::test_utils::expectedRow("ord1", "aa13", "Rose", 2, "New", 100, "55.00", ""),
        flower_exchange::test_utils::expectedRow("ord2", "aa14", "Rose", 2, "New", 100, "45.00", ""),
        flower_exchange::test_utils::expectedRow("ord3", "aa15", "Rose", 1, "New", 100, "35.00", ""),
    };

    const auto actual = flower_exchange::test_utils::runScenarioThroughReaderAndExchange(
        inputRows, "scenario_example2.csv");
    flower_exchange::test_utils::compareReportsExact(actual, expected, "Scenario Example 2");
}

}  // namespace

int main() {
    try {
        testScenarioExample2();
    } catch (const std::exception& exception) {
        std::cerr << "ScenarioExample2Tests failure: " << exception.what() << '\n';
        return 1;
    }

    return 0;
}
