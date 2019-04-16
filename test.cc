#include <algorithm>
#include <assert.h>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

using Int = unsigned long long;

Int choose(int n, int k) {
    if (k > n) return Int(0);
    if (k == 0 || k == n) return Int(1);
    if (k == 1 || k == n-1) return Int(n);
    return choose(n-1, k) + choose(n-1, k-1);
}

int ceil_lg(Int value) {
    Int r = 0;
    while (value > (Int(1) << r)) {
        ++r;
    }
    return r;
}

struct Candidate {
    std::vector<signed char> is_wolf;  // size == n, has exactly k nonzero elements
    Int test_results;  // t bits, each representing the result of a test
    explicit Candidate(std::vector<signed char> v) : is_wolf(std::move(v)) {}
};

std::vector<Candidate> make_candidates(int n, int k) {
    assert(n >= 0);
    assert(k >= 0);
    if (k == 0) {
        return std::vector<Candidate>{ Candidate(std::vector<signed char>(n, 0)) };
    } else if (k > n) {
        return std::vector<Candidate>{};
    } else {
        std::vector<Candidate> a = make_candidates(n-1, k);
        std::vector<Candidate> b = make_candidates(n-1, k-1);
        for (auto&& cand : a) cand.is_wolf.push_back(0);
        for (auto&& cand : b) cand.is_wolf.push_back(1);
        a.reserve(a.size() + b.size());
        for (auto&& cand : b) { a.push_back(std::move(cand)); }
        for (auto&& cand : a) { assert(cand.is_wolf.size() == n); }
        return a;
    }
}

void print_solution(const std::vector<Int>& solution, int n, int t, const std::vector<Candidate>& cands)
{
    printf("Awesome, I think I found a solution using %d blood tests!\n", t);
    printf("  My %d tests use blood from the following sheep:\n", t);
    for (int i = 0; i < t; ++i) {
        printf("  %d. ", i+1);
        auto m = solution[i];
        for (int sheep = 0; sheep < n; ++sheep) {
            bool this_sheep_is_used = ((m & (Int(1) << sheep)) != 0);
            printf(" %c", this_sheep_is_used ? 'T' : '.');
        }
        printf("\n");
    }
    printf("The test results for each arrangement of wolves are:\n");
    for (auto&& cand : cands) {
            printf("Candidate wolves:");
            for (signed char x : cand.is_wolf) {
                printf(" %d", x);
            }
            printf("   Test results: ");
            for (int i=0; i < t; ++i) {
                bool test_was_positive = (cand.test_results & (Int(1) << i)) != 0;
                printf(" %c", test_was_positive ? '+' : '-');
            }
            printf("\n");
    }
    throw "done";
}

void attempt_testing(std::vector<Candidate>& cands, std::vector<Int>& solution, int n, int i, int t) {

    if (i >= t) {
        assert(false);
    }
    for (Int m = 1; m < (Int(1) << n); ++m) {
        // Suppose the i'th test (out of t tests total) combines blood from these sheep.
        // What will the result of the test be, for each candidate arrangement of wolves?
        for (auto&& cand : cands) {
            Int test_result = Int(0);
            for (int sheep = 0; sheep < n; ++sheep) {
                bool this_sheep_is_used = ((m & (Int(1) << sheep)) != 0);
                if (this_sheep_is_used) {
                    if (cand.is_wolf[sheep]) {
                        test_result = Int(1);
                        break;
                    }
                }
            }
            cand.test_results &= (Int(1) << i) - 1;   // clear all bits [i..t)
            cand.test_results |= (test_result << i);  // set bit [i] appropriately
        }

        // Having performed this test, we want to make sure that it's still
        // information-theoretically possible to distinguish so-far-identical
        // cases in our remaining (t - i - 1) tests.
        Int limit = Int(1) << (t - i - 1);

        bool this_test_is_workable = true;
        bool these_tests_are_sufficient = true;
        std::map<Int, Int> partial_result_counts;
        for (auto&& cand : cands) {
            Int& count = partial_result_counts[cand.test_results];
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
            solution[i] = m;
            print_solution(solution, n, i+1, cands);
        } else {
            solution[i] = m;
            attempt_testing(cands, solution, n, i+1, t);
        }
    }
}

bool try_it(int n, int k, int t)
{
    // k wolves hiding among n sheep, given t blood tests
    Int nck = choose(n, k);
    if (ceil_lg(nck) > t) {
        printf("Sorry, information theory tells us that distinguishing %s possibilities requires %d > %d tests.\n",
               std::to_string(nck).c_str(),
               ceil_lg(nck), t);
        return false;
    }
    if (t >= n-1) {
        printf("We can obviously test %d sheep one-by-one using %d >= %d-1 blood tests!\n", n, t, n);
        return true;
    } else if (k == 1) {
        assert(ceil_lg(n) <= t);
        printf("We can test %d sheep for a lone wolf using the binary approach, in %d <= %d blood tests.\n", n, ceil_lg(n), t);
        return true;
    } else {
        // Okay, we have to do it for real.
        std::vector<Candidate> cands = make_candidates(n, k);
#if 1
        for (auto&& cand : cands) {
            printf("Candidate wolves:");
            for (signed char x : cand.is_wolf) {
                printf(" %d", x);
            }
            printf("\n");
        }
#endif
        std::vector<Int> solution(t);
        try {
            attempt_testing(cands, solution, n, 0, t);
        } catch (const char *done) {
            return true;
        }
        printf("I believe it's impossible to detect %d wolves among %d sheep in only %d tests.\n", k, n, t);
        return false;
    }
}

int main(int argc, char **argv)
{
    assert(argc == 4);
    int n = atoi(argv[1]);
    int k = atoi(argv[2]);
    int t = atoi(argv[3]);
    assert(n >= k && k >= 0);
    assert(t >= 0);
    assert(t < sizeof(Int)*8);

    try_it(n, k, t);
}
