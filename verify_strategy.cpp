
#include "verify_strategy.h"

#include <cassert>
#include <limits>
#include <map>
#include <string>
#include <vector>

using Int = unsigned long long;

struct TestResults64 {
    uint64_t data_;
    void push_back(bool b) { data_ <<= 1; data_ |= uint64_t(b); }
    friend bool operator<(const TestResults64& a, const TestResults64& b) { return a.data_ < b.data_; }
};

struct TestResults128 {
    unsigned __int128 data_;
    void push_back(bool b) { data_ <<= 1; data_ |= (unsigned __int128)(b); }
    friend bool operator<(const TestResults128& a, const TestResults128& b) { return a.data_ < b.data_; }
};

struct TestResultsBig {
    std::vector<bool> data_;
    void push_back(bool b) { data_.push_back(b); }
    friend bool operator<(const TestResultsBig& a, const TestResultsBig& b) { return a.data_ < b.data_; }
};

struct WolfArrangement {
    std::vector<int> v_;

    static WolfArrangement from_index(int n, int d, int id) {
        WolfArrangement result;
        for (int i=0; i < d; ++i) result.v_.push_back(i);
        for (int i=0; i < id; ++i) result.increment(n);
        return result;
    }

    bool animal_is_wolf(int i) const {
        for (int j : v_) if (i == j) return true;
        return false;
    }

    template<class F>
    bool test_is_wolfy(const F& test_contains_animal, int t) const {
        for (int i : v_) {
            if (test_contains_animal(t, i)) return true;
        }
        return false;
    }

    void increment(int n) {
        // Increment the first possible animal index,
        // and then reset all the previous ones.
        const int d = v_.size();
        for (int i=0; i < d-1; ++i) {
            if (v_[i] + 1 < v_[i+1]) {
                v_[i] += 1;
                for (int j = 0; j < i; ++j) {
                    v_[j] = j;
                }
                return;
            }
        }
        if (v_[d-1] + 1 < n) {
            v_[d-1] += 1;
            for (int j = 0; j < d-1; ++j) {
                v_[j] = j;
            }
            return;
        }
        // Otherwise, increment is impossible.
        assert(false);
    }

    std::string to_string(int n) const {
        std::string result;
        for (int i=0; i < n; ++i) {
            result += (this->animal_is_wolf(i) ? '1' : '.');
        }
        return result;
    }
};

static Int add_check(Int a, Int b) {
    assert(0 <= a);
    assert(0 <= b && b <= std::numeric_limits<Int>::max() - a);
    return a + b;
}

static Int choose(int n, int k) {
    if (k > n) return Int(0);
    if (k == 0 || k == n) return Int(1);
    if (k == 1 || k == n-1) return Int(n);
    return add_check(choose(n-1, k), choose(n-1, k-1));
}

template<class TestResults>
VerifyStrategyResult verify_strategy_impl(int n, int d, const std::vector<std::string>& tests)
{
    const Int n_choose_d = choose(n, d);
    const int t = tests.size();
    for (auto&& test : tests) {
        assert(test.size() == n);
    }

    WolfArrangement wolves = WolfArrangement::from_index(n, d, 0);
    std::map<TestResults, int> test_results_map;

    auto run_tests = [&]() {
        auto test_contains_animal = [&](int ti, int ni) {
            return tests[ti][ni] == '1';
        };
        TestResults r;
        for (int ti=0; ti < t; ++ti) {
            r.push_back(wolves.test_is_wolfy(test_contains_animal, ti));
        }
        return r;
    };

    test_results_map.insert(std::make_pair(run_tests(), 0));
    for (Int id=1; id < n_choose_d; ++id) {
        wolves.increment(n);
        auto ii = test_results_map.insert(std::make_pair(run_tests(), id));
        if (ii.second == false) {
            int old_id = ii.first->second;
            VerifyStrategyResult result;
            result.success = false;
            result.w1 = WolfArrangement::from_index(n, d, old_id).to_string(n);
            result.w2 = wolves.to_string(n);
            return result;
        }
    }

    VerifyStrategyResult result;
    result.success = true;
    return result;
}

VerifyStrategyResult verify_strategy(int n, int d, const std::vector<std::string>& tests)
{
    if (tests.size() <= 64) {
        return verify_strategy_impl<TestResults64>(n, d, tests);
    } else if (tests.size() <= 128) {
        return verify_strategy_impl<TestResults128>(n, d, tests);
    } else {
        return verify_strategy_impl<TestResultsBig>(n, d, tests);
    }
}
