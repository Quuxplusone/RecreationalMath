#include <array>
#include <assert.h>
#include <set>
#include <map>
#include <stdio.h>
#include <vector>
#if USE_MEMORY_RESOURCE
#include <memory_resource>
#endif

using Int = unsigned long long;

template<int n, int k>
static constexpr Int choose() {
    Int grid[n+1][k+1] = {};
    auto compute = [&](int ni, int ki) {
        if (ki > ni) return Int(0);
        if (ki == 0 || ki == ni) return Int(1);
        if (ki == 1 || ki == ni-1) return Int(ni);
        if (ni-1 < 0) throw;
        if (ki-1 < 0) throw;
        return grid[ni-1][ki] + grid[ni-1][ki-1];
    };
    for (int i = 0; i < n+k+2; ++i) {
        for (int ni = 0; ni <= i && ni <= n; ++ni) {
            int ki = i - ni;
            if (ki <= k) {
                grid[ni][ki] = compute(ni, ki);
            }
        }
    }
    return grid[n][k];
}

template<class TS, class = void>
struct TestResults {
    std::vector<bool> data_;
    void push_back(bool b) {
        data_.push_back(b);
    }
    friend bool operator<(const TestResults& a, const TestResults& b) {
        return a.data_ < b.data_;
    }
};
template<class TS>
struct TestResults<TS, std::enable_if_t<(TS::t <= 64)>> {
    uint64_t data_;
    void push_back(bool b) {
        data_ <<= 1;
        data_ |= uint64_t(b);
    }
    friend bool operator<(const TestResults& a, const TestResults& b) {
        return a.data_ < b.data_;
    }
};
template<class TS>
struct TestResults<TS, std::enable_if_t<(64 < TS::t && TS::t <= 128)>> {
    unsigned __int128 data_;
    void push_back(bool b) {
        data_ <<= 1;
        data_ |= (unsigned __int128)(b);
    }
    friend bool operator<(const TestResults& a, const TestResults& b) {
        return a.data_ < b.data_;
    }
};

struct WolfArrangement {
    std::vector<int> v_;

    explicit WolfArrangement(int k) {
        for (int i=0; i < k; ++i) {
            v_.push_back(i);
        }
    }

    bool animal_is_wolf(int i) const {
        for (int j : v_) if (i == j) return true;
        return false;
    }

    template<class TS>
    bool test_is_wolfy(int t) const {
        for (int i : v_) {
            if (TS::test_contains_animal(t, i)) {
                return true;
            }
        }
        return false;
    }

    template<class TS>
    void increment() {
        // Increment the first possible animal index,
        // and then reset all the previous ones.
        const int k = v_.size();
        for (int i=0; i < k-1; ++i) {
            if (v_[i] + 1 < v_[i+1]) {
                v_[i] += 1;
                for (int j = 0; j < i; ++j) {
                    v_[j] = j;
                }
                return;
            }
        }
        if (v_[k-1] + 1 < TS::n) {
            v_[k-1] += 1;
            for (int j = 0; j < k-1; ++j) {
                v_[j] = j;
            }
            return;
        }
        // Otherwise, increment is impossible.
        assert(false);
    }
};

template<class TS>
WolfArrangement wolf_arrangement_from_index(int id) {
    assert((0 <= id && id < choose<TS::n, TS::k>()));
    WolfArrangement result(TS::k);
    for (int i=0; i < id; ++i) {
        result.increment<TS>();
    }
    return result;
}

struct T_8_2 {
    static constexpr int n = 8;
    static constexpr int k = 2;
    static constexpr int t = 6;
    static bool test_contains_animal(int t, int i) {
        switch (t) {
            case 0: return "T..TT..."[i] == 'T';
            case 1: return "T....TT."[i] == 'T';
            case 2: return ".T.T.T.."[i] == 'T';
            case 3: return ".T..T.T."[i] == 'T';
            case 4: return "..T.TT.."[i] == 'T';
            case 5: return "..TT..T."[i] == 'T';
        }
        assert(false);
    }
};

struct T_14_3 {
    // Dmitry Kamenetsky, https://oeis.org/A290492
    static constexpr int n = 14;
    static constexpr int k = 3;
    static constexpr int t = 12;
    static bool test_contains_animal(int t, int i) {
        switch (i) {
            case  0: return "100000001100"[t] == '1';
            case  1: return "000001010001"[t] == '1';
            case  2: return "100101100000"[t] == '1';
            case  3: return "010000110100"[t] == '1';
            case  4: return "000110000101"[t] == '1';
            case  5: return "011100000000"[t] == '1';
            case  6: return "001000101001"[t] == '1';
            case  7: return "000000000000"[t] == '1';
            case  8: return "101010010000"[t] == '1';
            case  9: return "001001000110"[t] == '1';
            case 10: return "000100011010"[t] == '1';
            case 11: return "000010100010"[t] == '1';
            case 12: return "110000000011"[t] == '1';
            case 13: return "010011001000"[t] == '1';
        }
        assert(false);
    }
};

static constexpr bool T_100_5_test_contains_animal_(int t, int i) {
    if (i == 99) return false;
    switch (t / 11) {
        case 0: return (t % 11) == ((i+0*(i/11)) % 11);
        case 1: return (t % 11) == ((i+1*(i/11)) % 11);
        case 2: return (t % 11) == ((i+2*(i/11)) % 11);
        case 3: return (t % 11) == ((i+3*(i/11)) % 11);
        case 4: return (t % 11) == ((i+4*(i/11)) % 11);
        case 5: return ((t - 55) % 9) == (i / 11);
    }
    assert(false);
}
struct T_100_5_cache {
    char data_[63][100];
};
static constexpr T_100_5_cache T_100_5_cacheit_() {
    T_100_5_cache result {};
    for (int j = 0; j < 63; ++j) {
        for (int i = 0; i < 100; ++i) {
            result.data_[j][i] = T_100_5_test_contains_animal_(j, i);
        }
    }
    return result;
}
struct T_100_5 {
    static constexpr int n = 100;
    static constexpr int k = 5;
    static constexpr int t = 63;

    static constexpr const T_100_5_cache cached_ = T_100_5_cacheit_();

    static bool test_contains_animal(int t, int i) {
        return cached_.data_[t][i];
    }
};
const T_100_5_cache T_100_5::cached_;

struct T_111_3 {
    static constexpr int n = 111;
    static constexpr int k = 3;
    static constexpr int t = 37;

    static bool test_contains_animal(int t, int n) {
        // Each animal n is tested exactly 4 times;
        // no pair of animals is tested twice together.
        // Thanks to @Elaqqad for this example!
        // https://math.stackexchange.com/questions/3195281#comment7183199_3488985
        int i = n % 3;
        int j = n / 3;
        assert(0 <= j && j < 37);
        int D[3][4] = {
            {0, 1, 3, 24},
            {0, 10, 18, 30},
            {0, 4, 26, 32},
        };
        if (t == (D[i][0] + j) % 37) return true;
        if (t == (D[i][1] + j) % 37) return true;
        if (t == (D[i][2] + j) % 37) return true;
        if (t == (D[i][3] + j) % 37) return true;
        return false;
    }
};

struct T_273_5_helper {
    static constexpr bool test_contains_animal(int t, int n) {
        // Each animal n is tested exactly 6 times;
        // no pair of animals is tested twice together.
        // Table 1 in "Parallel Filter-Based Feature Selection Based on Balanced Incomplete Block Designs"
        // (Salmerón, Madsen, et al., ECAI 2016).
        // https://books.google.com/books?id=xfboDAAAQBAJ&pg=PA747
        int i = n % 3;
        int j = n / 3;
        assert(0 <= j && j < 91);
        int D[3][6] = {
            {0, 1, 3, 7, 25, 38},
            {0, 5, 20, 32, 46, 75},
            {0, 8, 17, 47, 57, 80},
        };
        if (t == (D[i][0] + j) % 91) return true;
        if (t == (D[i][1] + j) % 91) return true;
        if (t == (D[i][2] + j) % 91) return true;
        if (t == (D[i][3] + j) % 91) return true;
        if (t == (D[i][4] + j) % 91) return true;
        if (t == (D[i][5] + j) % 91) return true;
        return false;
    }

    static constexpr bool test_should_be_run(int t, int n) {
        for (int i=0; i < n; ++i) {
            if (test_contains_animal(t, i)) return true;
        }
        return false;
    }

    static constexpr int count_useful_tests(int n) {
        int count = 0;
        for (int t=0; t < 91; ++t) {
            count += test_should_be_run(t, n);
        }
        return count;
    };
};

struct T_273_5 : T_273_5_helper {
    static constexpr int n = 100;  // works for up to 273
    static constexpr int k = 5;
    static constexpr int t = count_useful_tests(n);

    static bool test_contains_animal(int t, int i) {
        for (int tt = 0; tt < 91; ++tt) {
            if (test_should_be_run(tt, n)) {
                if (t-- == 0) {
                    return T_273_5_helper::test_contains_animal(tt, i);
                }
            }
        }
        assert(false);
    }
};

struct T_26_3 {
    static constexpr int n = 26;
    static constexpr int k = 3;
    static constexpr int t = 19;

    static bool test_contains_animal(int t, int i) {
        if (i == 25) return false;
        switch (t / 5) {
            case 0: return (t % 5) == ((i+0*(i/5)) % 5);
            case 1: return (t % 5) == ((i+1*(i/5)) % 5);
            case 2: return (t % 5) == ((i+2*(i/5)) % 5);
            case 3: return (t % 5) == ((i+3*(i/5)) % 5);
        }
        assert(false);
    }
};

struct T_21_3 {
    static constexpr int n = 21;
    static constexpr int k = 3;
    static constexpr int t = 18;

    static bool test_contains_animal(int t, int i) {
        if (i == 20) return false;
        switch (t / 5) {
            case 0: return (t % 5) == ((i+0*(i/5)) % 5);
            case 1: return (t % 5) == ((i+1*(i/5)) % 5);
            case 2: return (t % 5) == ((i+2*(i/5)) % 5);
            case 3: return (t % 5) == (i / 5);
        }
        assert(false);
    }
};

struct T_17_3 {
    static constexpr int n = 17;
    static constexpr int k = 3;
    static constexpr int t = 15;

    static bool test_contains_animal(int t, int i) {
        if (i == 16) return false;
        switch (t / 4) {
            case 0: return (t % 4) == "aaaabbbbccccdddd"[i] - 'a';
            case 1: return (t % 4) == "abcdabcdabcdabcd"[i] - 'a';
            case 2: return (t % 4) == "abcdbadccdabdcba"[i] - 'a';
            case 3: return (t % 4) == "abcddcbabadccdab"[i] - 'a';
        }
        assert(false);
    }
};

template<class TS>
void print_wolves(const WolfArrangement& w) {
    for (int i=0; i < TS::n; ++i) {
        if (w.animal_is_wolf(i)) {
            printf("W");
        } else {
            printf(".");
        }
    }
    printf("\n");
}

template<class TS>
TestResults<TS> run_tests(const WolfArrangement& w) {
    TestResults<TS> r;
    for (int i=0; i < TS::t; ++i) {
        r.push_back(w.test_is_wolfy<TS>(i));
    }
    return r;
}

template<class TS>
bool verify_strategy() {
    WolfArrangement wolves(TS::k);
#if USE_MEMORY_RESOURCE
    std::pmr::map<TestResults<TS>, int> tr;
#else
    std::map<TestResults<TS>, int> tr;
#endif
    tr.insert(std::make_pair(run_tests<TS>(wolves), 0));
    for (Int id=1; id < choose<TS::n, TS::k>(); ++id) {
        wolves.increment<TS>();
        auto ii = tr.insert(std::make_pair(run_tests<TS>(wolves), id));
        if (ii.second == false) {
            printf("Failure! These wolf arrangements cannot be distinguished:\n");
            print_wolves<TS>(wolf_arrangement_from_index<TS>(ii.first->second));
            print_wolves<TS>(wolves);
            return false;
        }
    }
    printf("Success!\n");
    return true;
}

template<class TS>
void print_strategy() {
    for (int i = 0; i < TS::t; ++i) {
        printf("  %d.%s", i+1, (i >= 9) ? "" : " ");
        for (int sheep = 0; sheep < TS::n; ++sheep) {
            bool this_sheep_is_used = TS::test_contains_animal(i, sheep);
            printf(" %c", this_sheep_is_used ? 'T' : '.');
        }
        printf("\n");
    }
}

int main() {
#if USE_MEMORY_RESOURCE
    std::pmr::monotonic_buffer_resource mr(1'000'000);
    std::pmr::set_default_resource(&mr);
#endif
    using T = T_111_3;
    if (verify_strategy<T>()) {
        print_strategy<T>();
    }
}
