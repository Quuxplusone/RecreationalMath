#pragma once

#include <string>

struct NktResult {
    bool success;
    std::string message;
    explicit NktResult(bool success, std::string msg) : success(success), message(std::move(msg)) {}
};

NktResult solve_wolves(int n, int k, int t);
