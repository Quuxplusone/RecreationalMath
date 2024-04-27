#pragma once

#include <string>
#include <vector>

struct VerifyStrategyResult {
    bool success;
    // If not success, then w1 and w2 will be indistinguishable wolf arrangements.
    std::string w1;
    std::string w2;
};

VerifyStrategyResult verify_strategy(int n, int d, const std::vector<std::string>& tests);
