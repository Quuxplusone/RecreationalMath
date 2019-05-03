
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
int ceil_div(int x, int y) {
    return (x + y - 1) / y;
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
template<class A, class B>
struct TestingState {
    std::vector<Candidate> cands;
    std::vector<Int> solution;
    std::vector<Int> partial_result_counts;
    A early_terminate;
    B test_is_acceptable;

    explicit TestingState(A a, B b) :
        early_terminate(std::move(a)), test_is_acceptable(std::move(b)) {}

    Int groups_for_sheep(int sheep, int t) const {
        Int result = Int(0);
        for (int i=0; i < t; ++i) {
            if (solution[i] & (Int(1) << sheep)) {
                result |= Int(1);
            }
            result <<= 1;
        }
        return result;
    }
};
} // anonymous namespace

template<class A, class B>
static void attempt_testing(TestingState<A, B>& state, int n, int i, int t) {
    assert(i < t);
    if (state.early_terminate()) {
        throw EarlyTerminateException();
    }

    Int mask_so_far = Int(0);
    for (int j=0; j < i; ++j) mask_so_far |= state.solution[j];

    // Without loss of generality, we can assume that the tests are performed
    // in order of "weight" -- no test involves more animals than its predecessor.
    int max_population = (i == 0) ? INT_MAX : popcount(state.solution[i-1]);

    // By the pigeonhole principle, at least one of the tests we have left to run must
    // involve AT LEAST this many yet-to-be-tested animals.
    int animals_yet_to_test = (n - 1) - popcount(mask_so_far);
    int remaining_tests = (t - i);
    if (i != 0 && animals_yet_to_test > max_population * remaining_tests) {
        return;
    }

    Int starting_m = (i == 0) ? 1 : state.solution[i-1] + 1;

    // Information theory tells us that, after this test is performed, if our tests
    // thus far have given identical results for more than 2^(remaining tests)
    // distinct candidate sets of wolves, then it's hopeless; we'll never distinguish
    // all of those sets in just (remaining tests) tests.
    const Int permissible_indistinguishable_cases = Int(1) << (remaining_tests - 1);

    for (Int m = starting_m; m < (Int(1) << (n - 1)) - 1; m = increment(m, i)) {

        if (!state.test_is_acceptable(m)) {
            continue;
        }

        if (!is_power_of_2_minus_1(mask_so_far | m)) {
            // Testing the 6th animal when we haven't touched the 5th animal yet is pointless.
            // Without loss of generality we can assume the animals are introduced in order.
            continue;
        }
        if (popcount(m) > max_population) {
            continue;
        }

        // Without loss of generality, we can keep the columns decreasing to the right.
        // That is, if Sheeps 1 and 2 have been in the same test groups all the time
        // up to this point, we should not introduce Sheep 2 into a new group unless Sheep 1
        // is already there (but we might introduce Sheep 1 without Sheep 2).
        for (int s2 = 1; s2 < n; ++s2) {
            int s1 = s2 - 1;
            bool sheep2_in_group = (m & (Int(1) << s2)) != 0;
            bool sheep1_in_group = (m & (Int(1) << s1)) != 0;
            if (sheep2_in_group && !sheep1_in_group) {
                if (state.groups_for_sheep(s1, i) == state.groups_for_sheep(s2, i)) {
                    goto abandon_this_line;
                }
            }
        }
        if (false) {
            abandon_this_line: continue;
        }

        // Having performed this test, we want to make sure that it's still
        // information-theoretically possible to distinguish so-far-identical
        // cases in our remaining (t - i - 1) tests.

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
            if (count > permissible_indistinguishable_cases) {
                goto abandon_this_line;
            } else if (count > 1) {
                these_tests_are_sufficient = false;
            }
        }

        if (these_tests_are_sufficient) {
            state.solution[i] = m;
            report_solution(state.solution, n, i+1, state.cands);
        } else {
            state.solution[i] = m;
            attempt_testing(state, n, i+1, t);
        }
    }
}

template<class A, class B>
static NktResult solve_wolves_impl(int n, int k, int t, const A& early_terminate, const B& test_is_acceptable)
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
        TestingState<A, B> state(early_terminate, test_is_acceptable);
        state.cands = std::move(cands);
        state.solution.resize(t);
        try {
            attempt_testing(state, n, 0, t);
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
    auto early_terminate = []() { return false; };
    auto test_is_acceptable = [](Int) { return true; };
    return solve_wolves_impl(n, k, t, early_terminate, test_is_acceptable);
}

NktResult solve_wolves(int n, int k, int t, int s)
{
    auto early_terminate = []() { return false; };
    auto test_is_acceptable = [s](Int m) { return popcount(m) == s; };
    return solve_wolves_impl(n, k, t, early_terminate, test_is_acceptable);
}

NktResult solve_wolves(int n, int k, int t, std::function<bool()> early_terminate)
{
    auto test_is_acceptable = [](Int) { return true; };
    return solve_wolves_impl(n, k, t, early_terminate, test_is_acceptable);
}
