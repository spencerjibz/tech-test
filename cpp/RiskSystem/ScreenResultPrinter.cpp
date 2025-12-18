#include "ScreenResultPrinter.h"
#include "optional"
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

#include <string>
void ScreenResultPrinter::printResults(ScalarResults &results) {
    for (const auto &result : results) {
        auto tradeId = result.getTradeId();
        std::optional<double> expected_result = result.getResult();
        std::optional<std::string> error = result.getError();
        std::stringstream stream;
        stream << tradeId;
        if (expected_result.has_value()) {
            stream << " : ";
            stream << expected_result.value();
        }

        if (error.has_value()) {
            stream << " : ";
            stream << error.value();
        }
        std::cout << stream.str() << std::endl;
    }
}
