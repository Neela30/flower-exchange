#include <exception>
#include <iostream>
#include <vector>

#include "ScenarioTestUtils.h"

namespace {

void testScenarioExample1() {
    const std::vector<std::string> inputRows{
        "aa13,Rose,2,100,55.00",
    };

    const std::vector<flower_exchange::test_utils::ComparableReportRow> expected{
        flower_exchange::test_utils::expectedRow("ord1", "aa13", "Rose", 2, "New", 100, "55.00", ""),
    };

    const auto actual = flower_exchange::test_utils::runScenarioThroughReaderAndExchange(
        inputRows, "scenario_example1.csv");
    flower_exchange::test_utils::compareReportsExact(actual, expected, "Scenario Example 1");
}

}  // namespace

int main() {
    try {
        testScenarioExample1();
    } catch (const std::exception& exception) {
        std::cerr << "ScenarioExample1Tests failure: " << exception.what() << '\n';
        return 1;
    }

    return 0;
}
