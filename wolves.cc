
#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "wolves.h"

using Int = unsigned long long;

static Int choose(int n, int k) {
    if (k > n) return Int(0);
    if (k == 0 || k == n) return Int(1);
    if (k == 1 || k == n-1) return Int(n);
    return choose(n-1, k) + choose(n-1, k-1);
}

static inline
int ceil_lg(Int value) {
    Int r = 0;
    while (value > (Int(1) << r)) {
        ++r;
    }
    return r;
}

static inline
int popcount(Int value) {
#if 1
    return __builtin_popcountll(value);
#else
    Int bit = 1;
    int result = 0;
    while (bit <= value) {
        if (value & bit) ++result;
        bit <<= 1;
    }
    return result;
#endif
}

static inline
bool is_power_of_2_minus_1(Int x) {
    Int y = x;
    ++y;
    return (y & x) == 0;
}

static std::string format(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char buffer[100];
    int len_without_nul = vsnprintf(buffer, sizeof buffer, fmt, ap);
    std::string result;
    if (len_without_nul < sizeof buffer) {
        result = buffer;
    } else {
        va_end(ap);
        va_start(ap, fmt);
        result.resize(len_without_nul + 1);
        int n = vsnprintf(&result[0], result.size(), fmt, ap);
        assert(n == len_without_nul);
        result.resize(n);
        return result;
    }
    va_end(ap);
    return result;
}

struct Candidate {
    Int is_wolf;  // n bits, has exactly k nonzero bits
    Int test_results;  // t bits, each representing the result of a test
    explicit Candidate(Int v) : is_wolf(v) {}
};

static std::vector<Candidate> make_candidates(int n, int k) {
    assert(n >= 0);
    assert(k >= 0);
    if (k == 0) {
        return std::vector<Candidate>{ Candidate(0) };
    } else if (k > n) {
        return std::vector<Candidate>{};
    } else {
        std::vector<Candidate> a = make_candidates(n-1, k);
        std::vector<Candidate> b = make_candidates(n-1, k-1);
        for (auto&& cand : a) cand.is_wolf <<= 1;
        for (auto&& cand : b) cand.is_wolf = (cand.is_wolf << 1) | 1;
        a.reserve(a.size() + b.size());
        for (auto&& cand : b) { a.push_back(std::move(cand)); }
        return a;
    }
}

static void report_solution(const std::vector<Int>& solution, int n, int t, const std::vector<Candidate>& cands)
{
    std::string message;
    message += format("Awesome, I think I found a solution using %d blood tests!\n", t);
    message += format("  My %d tests use blood from the following sheep:\n", t);
    for (int i = 0; i < t; ++i) {
        message += format("  %d.%s", i+1, (i >= 9) ? "" : " ");
        auto m = solution[i];
        for (int sheep = 0; sheep < n; ++sheep) {
            bool this_sheep_is_used = ((m & (Int(1) << sheep)) != 0);
            message += format(" %c", this_sheep_is_used ? 'T' : '.');
        }
        message += format("\n");
    }
#if 0
    message += format("The test results for each arrangement of wolves are:\n");
    for (auto&& cand : cands) {
            message += format("Candidate wolves:");
            for (int i=0; i < n; ++i) {
                bool sheep_is_wolf = (cand.is_wolf & (Int(1) << i)) != 0;
                message += format(" %d", sheep_is_wolf ? 1 : 0);
            }
            message += format("   Test results: ");
            for (int i=0; i < t; ++i) {
                bool test_was_positive = (cand.test_results & (Int(1) << i)) != 0;
                message += format(" %c", test_was_positive ? '+' : '-');
            }
            message += format("\n");
    }
#endif
    throw NktResult(true, message);
}

static inline
Int increment(Int m, int i) {
    if (i == 0) {
        m <<= 1;
    }
    ++m;
    return m;
}

namespace {
struct TestingState {
    std::vector<Candidate> cands;
    std::vector<Int> solution;
    std::vector<Int> partial_result_counts;
};
} // anonymous namespace

template<class F>
static void attempt_testing(const F& early_terminate, TestingState& state, int n, int i, int t) {
    assert(i < t);
    if (early_terminate()) {
        throw EarlyTerminateException();
    }

    Int mask_so_far = Int(0);
    for (int j=0; j < i; ++j) mask_so_far |= state.solution[j];

    int remaining_pigeons = (n - 1) - popcount(mask_so_far);
    int remaining_holes = (t - i);
    int min_new_pigeons_in_this_hole = (remaining_pigeons + (remaining_holes - 1)) / remaining_holes;
    // By the pigeonhole principle, at least one of the tests we have left to run must
    // involve AT LEAST this many yet-to-be-tested sheep. Without loss of generality,
    // we can assume that that test is the very next test.

    Int starting_m = (i == 0) ? (Int(1) << min_new_pigeons_in_this_hole) - 1 : state.solution[i-1] + 1;
    for (Int m = starting_m; m < (Int(1) << (n - 1)) - 1; m = increment(m, i)) {

        if (!is_power_of_2_minus_1(mask_so_far | m)) {
            // Testing the 6th animal when we haven't touched the 5th animal yet is pointless.
            // Without loss of generality we can assume the animals are introduced in order.
            continue;
        }
        if (popcount(m & ~mask_so_far) < min_new_pigeons_in_this_hole) {
            continue;
        }

        // Having performed this test, we want to make sure that it's still
        // information-theoretically possible to distinguish so-far-identical
        // cases in our remaining (t - i - 1) tests.
        Int limit = Int(1) << (t - i - 1);

        bool this_test_is_workable = true;
        bool these_tests_are_sufficient = true;
        state.partial_result_counts.resize(Int(1) << (i + 1));
        for (Int& count : state.partial_result_counts) {
            count = 0;
        }

        // Suppose the i'th test (out of t tests total) combines blood from these sheep.
        // What will the result of the test be, for each candidate arrangement of wolves?
        for (auto& cand : state.cands) {
            Int test_result = Int(0);
            if (m & cand.is_wolf) {
                test_result = Int(1) << i;
            }
            cand.test_results &= (Int(1) << i) - 1;  // clear all bits [i..t)
            cand.test_results |= test_result;        // set bit [i] appropriately

            assert(cand.test_results < state.partial_result_counts.size());
            Int& count = state.partial_result_counts[cand.test_results];
            count += 1;
            if (count > limit) {
                this_test_is_workable = false;
#if 0
                printf("- Shortcutting at test #%d because %d remaining tests cannot distinguish %s > %s possibilities...\n",
                       i, (t - i - 1), std::to_string(count).c_str(), std::to_string(limit).c_str());
#endif
                break;
            } else if (count > 1) {
                these_tests_are_sufficient = false;
            }
        }

        if (!this_test_is_workable) {
            // well shoot, reject
        } else if (these_tests_are_sufficient) {
            state.solution[i] = m;
            report_solution(state.solution, n, i+1, state.cands);
        } else {
            state.solution[i] = m;
            attempt_testing(early_terminate, state, n, i+1, t);
        }
    }
}

template<class F>
static NktResult solve_wolves_impl(int n, int k, int t, const F& early_terminate)
{
    // k wolves hiding among n sheep, given t blood tests

    assert(n >= k && k >= 0);
    assert(t >= 0);
    assert(t < sizeof(Int)*8);

    Int nck = choose(n, k);
    if (ceil_lg(nck) > t) {
        return NktResult(false,
            format(
                "Sorry, information theory tells us that distinguishing %s possibilities requires %d > %d tests.\n",
                std::to_string(nck).c_str(),
                ceil_lg(nck), t
            )
        );
    } else if (k == 0 || k == n) {
        return NktResult(true,
            format("We know %s of the sheep are wolves, so we don't need any tests!\n", (k == 0) ? "none" : "all")
        );
    } else if (t >= n-1) {
        return NktResult(true,
            format("We can obviously test %d sheep one-by-one using %d >= %d-1 blood tests!\n", n, t, n)
        );
    } else if (k == n-1) {
        return NktResult(false,
            format("Sorry, finding the one real sheep among %d wolves requires %d-1 > %d tests.\n", n, n, t)
        );
    } else if (k == 1) {
        assert(ceil_lg(n) <= t);
        return NktResult(true,
            format("We can test %d sheep for a lone wolf using the binary approach, in %d <= %d blood tests.\n", n, ceil_lg(n), t)
        );
    } else {
        // Okay, we have to do it for real.
        std::vector<Candidate> cands = make_candidates(n, k);
#if 0
        for (auto&& cand : cands) {
            printf("Candidate wolves:");
            for (int i=0; i < n; ++i) {
                bool sheep_is_wolf = (cand.is_wolf & (Int(1) << i)) != 0;
                printf(" %d", sheep_is_wolf ? 1 : 0);
            }
            printf("\n");
        }
#endif
        TestingState state;
        state.cands = std::move(cands);
        state.solution.resize(t);
        try {
            attempt_testing(early_terminate, state, n, 0, t);
        } catch (const NktResult& result) {
            assert(result.success == true);
            return result;
        }
        return NktResult(false,
            format("I believe it's impossible to detect %d wolves among %d sheep in only %d tests.\n", k, n, t)
        );
    }
}

NktResult solve_wolves(int n, int k, int t)
{
    return solve_wolves_impl(n, k, t, []() { return false; });
}

NktResult solve_wolves(int n, int k, int t, std::function<bool()> early_terminate)
{
    return solve_wolves_impl(n, k, t, early_terminate);
}
