#pragma once

#include <functional>
#include <string>

struct EarlyTerminateException {};

struct NktResult {
    bool success;
    std::string message;
    explicit NktResult(bool success, std::string msg) : success(success), message(std::move(msg)) {}
};

NktResult solve_wolves(int n, int k, int t);
NktResult solve_wolves(int n, int k, int t, std::function<bool()> early_terminate);

NktResult solve_wolves(int n, int k, int t, int s);
