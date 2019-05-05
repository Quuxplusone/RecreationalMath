#include <array>
#include <assert.h>
#include <set>
#include <map>
#include <stdio.h>
#include <vector>

using Int = unsigned long long;

static constexpr Int choose(int n, int k) {
    if (k > n) return Int(0);
    if (k == 0 || k == n) return Int(1);
    if (k == 1 || k == n-1) return Int(n);
    return choose(n-1, k) + choose(n-1, k-1);
}

struct TestResults {
    std::vector<bool> data_;
    void push_back(bool b) {
        data_.push_back(b);
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

struct T_8_2 {
    static constexpr int nCk = choose(8,2);
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
    static constexpr int nCk = 75287520;
    static constexpr int n = 100;
    static constexpr int k = 5;
    static constexpr int t = 63;

    static constexpr const T_100_5_cache cached_ = T_100_5_cacheit_();

    static bool test_contains_animal(int t, int i) {
        return cached_.data_[t][i];
    }
};
const T_100_5_cache T_100_5::cached_;

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
TestResults run_tests(const WolfArrangement& w) {
    TestResults r;
    for (int i=0; i < TS::t; ++i) {
        r.push_back(w.test_is_wolfy<TS>(i));
    }
    return r;
}

template<class TS>
void do_it() {
    WolfArrangement wolves(TS::k);
    std::map<TestResults, WolfArrangement> tr;
    tr.insert(std::make_pair(run_tests<TS>(wolves), wolves));
    for (Int i=1; i < TS::nCk; ++i) {
        wolves.increment<TS>();
        auto ii = tr.insert(std::make_pair(run_tests<TS>(wolves), wolves));
        if (ii.second == false) {
            printf("Failure! These wolf arrangements cannot be distinguished:\n");
            print_wolves<TS>(ii.first->second);
            print_wolves<TS>(wolves);
            return;
        }
    }
    printf("Success!\n");
}

int main() {
    do_it<T_100_5>();
}
