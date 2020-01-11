#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "verify_strategy.h"

enum class GuaranteedBest { Yes=1, No=0 };
enum class BelongsInFile { Yes=1, No=0 };

struct Strategy {
    std::function<std::vector<std::string>()> tests;
    int t;
    GuaranteedBest guaranteed_best;
    BelongsInFile belongs_in_file;

    explicit Strategy(std::vector<std::string> testvec, GuaranteedBest gb, BelongsInFile bf) :
        t(testvec.size()), guaranteed_best(gb), belongs_in_file(bf)
    {
        tests = [vec = std::move(testvec)]() { return vec; };
    }

    explicit Strategy(int t, GuaranteedBest gb, BelongsInFile bf, std::function<std::vector<std::string>()> tests) :
        t(t), guaranteed_best(gb), belongs_in_file(bf), tests(std::move(tests))
    {
    }

    bool isBetterThan(const Strategy& rhs) const {
        if (t != rhs.t) return (t < rhs.t);
        if (guaranteed_best != rhs.guaranteed_best) return (guaranteed_best == GuaranteedBest::Yes);
        return false;
    }

    std::string to_string(int n, int d) const {
        std::ostringstream oss;
        oss << "N=" << n << " D=" << d << " T=" << t;
        oss << " guaranteed_best=" << ((guaranteed_best == GuaranteedBest::Yes) ? '1' : '0') << '\n';
        auto concrete_tests = tests();
        for (auto&& test : concrete_tests) {
            oss << test << '\n';
        }
        return std::move(oss).str();
    }
};

std::shared_ptr<Strategy> empty_strategy()
{
    return std::make_shared<Strategy>(std::vector<std::string>{}, GuaranteedBest::Yes, BelongsInFile::No);
}

std::shared_ptr<Strategy> perfect_strategy_for_one_wolf(int n)
{
    std::vector<std::string> tests;
    for (int bit = 1; bit < n; bit <<= 1) {
        tests.push_back(std::string(n, '.'));
        for (int i=0; i < n; ++i) {
            tests.back()[i] = ((i & bit) ? '1' : '.');
        }
    }
    return std::make_shared<Strategy>(std::move(tests), GuaranteedBest::Yes, BelongsInFile::No);
}

std::shared_ptr<Strategy> worst_case_strategy(int n, GuaranteedBest gb)
{
    std::vector<std::string> tests;
    for (int i=0; i < n-1; ++i) {
        tests.push_back(std::string(n, '.'));
        tests.back()[i] = '1';
    }
    return std::make_shared<Strategy>(std::move(tests), gb, BelongsInFile::No);
}

std::shared_ptr<Strategy> chop_last_sheep(std::shared_ptr<Strategy> orig)
{
    auto tests = orig->tests();
    auto& final_test = tests.back();
    const int n = final_test.size();
    auto first_it = std::find(final_test.begin(), final_test.end(), '1');
    assert(first_it != final_test.end());
    int animal_to_remove_idx = (first_it - final_test.begin());
    bool can_lose_this_test = (std::find(first_it + 1, final_test.end(), '1') == final_test.end());

    if (can_lose_this_test) {
        tests.pop_back();
    }
    tests.erase(
        std::remove_if(
            tests.begin(),
            tests.end(),
            [&](const std::string& test) { return std::count(test.begin(), test.end(), '1') == int(test[animal_to_remove_idx] == '1'); }
        ),
        tests.end()
    );

    return std::make_shared<Strategy>(
        tests.size(),
        GuaranteedBest::No,
        BelongsInFile::No,
        [animal_to_remove_idx, captured_tests = std::move(tests)]() {
            auto tests = captured_tests;
            for (auto&& test : tests) {
                test.erase(test.begin() + animal_to_remove_idx);
            }
            return tests;
        }
    );
}

std::shared_ptr<Strategy> test_last_animal_individually(int n, std::shared_ptr<Strategy> orig)
{
    return std::make_shared<Strategy>(
        orig->t + 1,
        GuaranteedBest::No,
        BelongsInFile::No,
        [orig, n]() {
            auto tests = orig->tests();
            for (auto& test : tests) {
                test.push_back('.');
            }
            tests.push_back(std::string(n, '.') + '1');
            return tests;
        }
    );
}

std::shared_ptr<Strategy> remove_most_tested_animal(std::shared_ptr<Strategy> orig)
{
    auto tests = orig->tests();
    const int n = tests[0].size();

    std::vector<int> counts(n);
    for (auto&& test : tests) {
        for (int i=0; i < n; ++i) {
            counts[i] += (test[i] == '1');
        }
    }
    auto most_tested_idx = std::max_element(counts.begin(), counts.end()) - counts.begin();
    int most_tested_count = counts[most_tested_idx];

    return std::make_shared<Strategy>(
        orig->t - most_tested_count,
        GuaranteedBest::No,
        BelongsInFile::No,
        [most_tested_idx, captured_tests = std::move(tests)]() {
            auto tests = captured_tests;
            tests.erase(
                std::remove_if(
                    tests.begin(),
                    tests.end(),
                    [&](const auto& test){ return test[most_tested_idx] == '1'; }
                ),
                tests.end()
            );
            for (auto&& test : tests) {
                test.erase(test.begin() + most_tested_idx);
            }
            return tests;
        }
    );
}


struct ND {
    int n, d;
    bool operator<(const ND& rhs) const { return std::tie(d, n) < std::tie(rhs.d, rhs.n); }
};

bool overwrite_if_better(std::map<ND, std::shared_ptr<Strategy>>& m, int n, int d, std::shared_ptr<Strategy> strategy)
{
    assert(0 <= n);
    assert(0 <= d && d <= n);
    auto it = m.find(ND{n,d});
    if (it == m.end()) {
        return false;
    } else if (strategy->isBetterThan(*it->second)) {
        assert((it->second->guaranteed_best == GuaranteedBest::No) || !"found something better than the guaranteed best");
        if (it->second->belongs_in_file == BelongsInFile::Yes) {
            // If this solution came from the file, we don't want to completely vanish it.
            // Replace it in the file with this better solution.
            strategy->belongs_in_file = BelongsInFile::Yes;
        }
        it->second = strategy;
        return true;
    }
    return false;
}

void preserve_from_file(std::map<ND, std::shared_ptr<Strategy>>& m, int n, int d, std::shared_ptr<Strategy> strategy)
{
    m.insert(std::make_pair(ND{n,d}, worst_case_strategy(n, GuaranteedBest::No)));
    bool overwritten = overwrite_if_better(m, n, d, strategy);
    assert(overwritten);
}

void read_solutions_from_file(const char *filename, std::map<ND, std::shared_ptr<Strategy>>& m)
{
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        throw std::runtime_error("Failed to open solution file");
    }
    std::string line;
    bool seen_a_grid = false;
    while (std::getline(infile, line)) {
        if (line.compare(0, 2, "N=") == 0) {
            int n, d, t, gb;
            int rc = std::sscanf(line.c_str(), "N=%d D=%d T=%d guaranteed_best=%d", &n, &d, &t, &gb);
            assert(rc == 4 || !"input file contained malformed lines");
            std::vector<std::string> tests;
            for (int i=0; i < t; ++i) {
                std::getline(infile, line);
                assert(line.size() == n || !"input file contained malformed solution");
                tests.push_back(line);
            }
            auto strategy = std::make_shared<Strategy>(tests, gb ? GuaranteedBest::Yes : GuaranteedBest::No, BelongsInFile::Yes);
            preserve_from_file(m, n, d, std::move(strategy));
            seen_a_grid = true;
        } else if (seen_a_grid && line != "") {
            assert(!"input file contained malformed lines after the first grid");
        }
    }
}

void write_solutions_to_file(const char *filename, const std::map<ND, std::shared_ptr<Strategy>>& m)
{
    std::ofstream outfile(filename);

    // Write out the triangle, up to n=30.
    const int max_n_to_print = 30;

    outfile << "    d=       1  2  3  4  5  6  ...\n";
    outfile << "          .\n";
    outfile << "    n=1   .  0\n";
    outfile << "    n=2   .  1  0\n";
    outfile << "    n=3   .  2  2\n";

    for (int n = 4; n <= max_n_to_print; ++n) {
        outfile << "    n=" << std::setw(2) << std::left << n << "   ";
        for (int d = 1; d < n; ++d) {
            auto it = m.find(ND{n,d});
            if (it != m.end()) {
                int value = it->second->t;
                outfile << ' ' << std::setw(2) << std::right << value;
                if (value == n-1) {
                    // Don't bother filling out the rest of this line.
                    break;
                }
            } else {
                outfile << std::setw(3) << std::right << '?';
            }
        }
        outfile << "\n";
    }
    outfile << "\n\n";

    for (const auto& kv : m) {
        const auto& strategy = kv.second;
        if (strategy->belongs_in_file == BelongsInFile::Yes) {
            outfile << strategy->to_string(kv.first.n, kv.first.d) << '\n';
        }
    }
}

void add_easy_solutions(std::map<ND, std::shared_ptr<Strategy>>& m, int max_n)
{
    for (int n=0; n <= max_n; ++n) {
        for (int d=0; d <= n; ++d) {
            auto strategy =
                (d == 0 || d == n) ? empty_strategy() :
                (d == 1) ? perfect_strategy_for_one_wolf(n) :
                (d >= n/2) ? worst_case_strategy(n, GuaranteedBest::Yes) :
                worst_case_strategy(n, GuaranteedBest::No);
            m.insert(std::make_pair(ND{n,d}, strategy));
        }
    }
}

template<class KeyValue>
void add_solutions_derived_from(std::map<ND, std::shared_ptr<Strategy>>& m, const KeyValue& kv)
{
    int n = kv.first.n;
    int d = kv.first.d;
    const std::shared_ptr<Strategy>& strategy = kv.second;

    if (2 < n && d < n) {
        // A solution to t(n-1,d) can be constructed from t(n,d): simply introduce an innocent sheep.
        if (overwrite_if_better(m, n-1, d, chop_last_sheep(strategy))) {
            add_solutions_derived_from(m, *m.find(ND{n-1, d}));
        }
    }
    if (2 < d && d < n-1) {
        // A solution for (n,d) also works for (n,d-1) except when d >= n-1.
        std::shared_ptr<Strategy> r = std::make_shared<Strategy>(
            strategy->t,
            GuaranteedBest::No,
            BelongsInFile::No,
            [strategy]() { return strategy->tests(); }
        );
        if (overwrite_if_better(m, n, d-1, r)) {
            add_solutions_derived_from(m, *m.find(ND{n, d-1}));
        }
    }
    if (d < n) {
        if (overwrite_if_better(m, n+1, d, test_last_animal_individually(n, strategy))) {
            add_solutions_derived_from(m, *m.find(ND{n+1, d}));
        }
    }
    if (2 < d && d < n) {
        if (overwrite_if_better(m, n-1, d-1, remove_most_tested_animal(strategy))) {
            add_solutions_derived_from(m, *m.find(ND{n-1, d-1}));
        }
    }
    if (strategy->t == n-1) {
        if (overwrite_if_better(m, n+2, d+1, worst_case_strategy(n+2, strategy->guaranteed_best))) {
            add_solutions_derived_from(m, *m.find(ND{n+2, d+1}));
        }
    }
}

int main(int argc, char **argv)
{
    const char *filename = "wolfy-out.txt";
    bool verify = false;
    bool verify_all = false;
    int i = 1;
    for (; argv[i] != nullptr && argv[i][0] == '-'; ++i) {
        if (strcmp(argv[i], "--help") == 0) {
            puts("./wolfy [--file f.txt] [--verify] N D");
            puts("");
            puts("Print the smallest known D-separable matrix with N columns.");
            puts("  --file f.txt    Read best known solutions from this file");
            puts("  --verify        Verbosely verify the solution that is printed");
            puts("  --verify-all    Verify every solution in the input file");
            exit(0);
        } else if (strcmp(argv[i], "--file") == 0) {
            filename = argv[++i];
        } else if (strcmp(argv[i], "--verify") == 0) {
            verify = true;
        } else if (strcmp(argv[i], "--verify-all") == 0) {
            verify_all = true;
        } else {
            printf("Unrecognized option '%s'; --help for help\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }
    if (i + 2 != argc) {
        printf("Usage: ./wolfy N D; --help for help\n");
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[i]);
    int d = atoi(argv[i+1]);

    if (n < d || d < 0) {
        printf("Usage: ./wolfy N D; --help for help\n");
        exit(EXIT_FAILURE);
    }

    std::map<ND, std::shared_ptr<Strategy>> solutions_from_file;
    read_solutions_from_file(filename, solutions_from_file);

    int max_n = n + 10;
    for (auto&& kv : solutions_from_file) {
        if (verify_all) {
            VerifyStrategyResult r = verify_strategy(kv.first.n, kv.first.d, kv.second->tests());
            if (!r.success) {
                printf("INVALID! (This should never happen unless the solution file is bad.)\n");
                printf("%s\n", kv.second->to_string(n, d).c_str());
                printf("These two wolf arrangements cannot be distinguished:\n");
                printf("%s\n", r.w1.c_str());
                printf("%s\n", r.w2.c_str());
            }
        }
        max_n = std::max(max_n, kv.first.n);
    }

    std::map<ND, std::shared_ptr<Strategy>> all_solutions;
    add_easy_solutions(all_solutions, max_n);
    for (auto&& kv : solutions_from_file) {
        if (overwrite_if_better(all_solutions, kv.first.n, kv.first.d, kv.second)) {
            add_solutions_derived_from(all_solutions, kv);
        } else {
            preserve_from_file(all_solutions, kv.first.n, kv.first.d, kv.second);
        }
    }

    write_solutions_to_file("wolfy-out.txt", all_solutions);

    std::shared_ptr<Strategy> strategy = all_solutions.at(ND{n, d});
    auto tests = strategy->tests();

    if (verify) {
        printf("Candidate is\n");
        printf("%s\n", strategy->to_string(n, d).c_str());
        VerifyStrategyResult r = verify_strategy(n, d, tests);
        if (r.success) {
            printf("Verified. This is a solution for t(%d, %d) <= %zu.\n", n, d, tests.size());
        } else {
            printf("INVALID! (This should never happen unless the solution file is bad.)\n");
            printf("These two wolf arrangements cannot be distinguished:\n");
            printf("%s\n", r.w1.c_str());
            printf("%s\n", r.w2.c_str());
        }
    } else {
        for (auto&& line : tests) printf("%s\n", line.c_str());
    }
}
