// Compile with "g++ -std=c++17 -W -Wall -O2 count-dfas-up-to-alphabet-recoloring.cpp"

/*
 a(n) = Count of languages over an alphabet of size L
        which are recognizable by a DFA with n states,
        up to recoloring of the alphabet.

 For example, a(1) = 2 (the empty language and the universal language).
 For a(2) = 12, there are six recognizable languages, plus their complements,
 plus the languages you get by recoloring the alphabet (e.g. swapping 'a' and 'b',
 or recoloring all 'b's as 'a's):
  19 (..)*      23 NOT(19)
  3  (a|b.)*    7  NOT(3)    11 (b|a.)*    15 NOT(11)
  5  (a|ba*b)*  9  NOT(5)    12 (b|ab*a)*  16 NOT(12)
  6  a*         10 NOT(6)    14 b*         18 NOT(14)   1  .*         2  NOT(1)
  17 .*a        13 NOT(17)   8  .*b        4  NOT(8)    26 ..*        22 NOT(26)
  21 (.a*b)*    25 NOT(21)   20 (.b*a)*    24 NOT(20)

 To determine whether two superficially different DFAs accept the same language,
 we just run them brute-force on all possible input strings up to some given length `i`.
 (Length `2n-1` suffices to distinguish any two `n`-state DFAs.)

 Run as `./count-dfas-up-to-alphabet-recoloring n`, where `n` is the number of states (e.g. 4).

          L=1   L=2      L=3     L=4     L=5
    n=1   2     ...
    n=2   6     ...
    n=3   18    64       ...
    n=4   48    3900     16314   ?       ?
    n=5   126   273072   ?       ?       ?
    n=6   306   ?        ?       ?       ?
    n=7   738   ?        ?       ?       ?
*/

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <numeric>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#ifndef MAXN
 #define MAXN 10 // "10 states should be enough for anybody"
#endif

using Input = std::string;

struct State {
  int next_[ALPHABET_SIZE];
};

struct DFA {
  State states_[MAXN];
  int accepting_;
  int nstates_;
};

void print_dfa(const DFA& d) {
  for (int s = 0; s < d.nstates_; ++s) {
    if (s != 0) printf(";");
    printf(" State %d: ", s+1);
    for (int c = 0; c < ALPHABET_SIZE; ++c) {
      if (c != 0) printf(", ");
      printf("%c->%d", 'a'+c, d.states_[s].next_[c]+1);
    }
    if (s < d.accepting_) printf(", accepting");
  }
  printf("\n");
}

bool dfa_accepts(const DFA& d, const Input& input, const int *permutation) {
  int s = 0;
  for (char cx : input) {
    char c = permutation[int(cx)];
    s = d.states_[s].next_[int(c)];
    assert(s < d.nstates_);
  }
  return s < d.accepting_;
}

template<class State, class F>
static void populate(State& s, int nstates, int c, const F& f) {
  if (c == ALPHABET_SIZE) {
    f();
  } else {
    // Set the c'th transition to any legal value (from 0 up to nstates-1).
    for (int i=0; i < nstates; ++i) {
      s.next_[c] = i;
      populate(s, nstates, c+1, f);
    }
  }
}

template<int RecursionBound, class F>
void all_dfas(int n, int m, int accepting, const F& f) {
  // Yield all DFAs with states 1..m inclusive.
  if (m == 0) {
    DFA d;
    d.nstates_ = n;
    d.accepting_ = accepting;
    f(d);
  } else if constexpr (RecursionBound == 0) {
    assert(false && "Should never happen");
  } else {
    all_dfas<RecursionBound-1>(n, m-1, accepting, [&](DFA& d) {
      // Is this DFA already closed, i.e., state m-1 isn't even reachable from any lower state?
      // Note that we could reach m-1 by jumping up to m+k and then back down.
      bool is_unreachable = (m-1 != 0);
      for (int i=0; i < m-1; ++i) {
        for (int c = 0; c < ALPHABET_SIZE; ++c) {
          int cand = d.states_[i].next_[c];
          if (cand >= m-1) {
            is_unreachable = false;
            break;
          }
        }
      }
      if (is_unreachable) {
        // State m-1 isn't even reachable.
        for (int c = 0; c < ALPHABET_SIZE; ++c) {
          d.states_[m-1].next_[c] = 0;
        }
        f(d);
      } else {
        // State m-1 is reachable. Populate it.
        populate(d.states_[m-1], n, 0, [&]() { f(d); });
      }
    });
  }
}

using Language = std::string;

Language language_recognized_by(const DFA& d, const std::vector<Input>& inputs, const int *permutation) {
  size_t n = inputs.size();
  std::string lang;
  lang.resize(n);
  for (size_t i = 0; i < n; ++i) {
    lang[i] = dfa_accepts(d, inputs[i], permutation) ? 1 : 0;
  }
  return lang;
}

Language lexically_first_language_recognized_by(const DFA& d, const std::vector<Input>& inputs, const std::vector<std::vector<int>>& permutations) {
  std::string smallest = "x";
  for (auto&& p : permutations) {
    std::string lang = language_recognized_by(d, inputs, p.data());
    if (lang < smallest) {
      smallest = std::move(lang);
    }
  }
  assert(smallest.size() == inputs.size());
  return smallest;
}

std::vector<Input> generate_inputs(int len) {
  if (len == 0) {
    return {""};
  } else {
    std::vector<Input> v = generate_inputs(len - 1);
    std::vector<Input> r = v;
    for (auto&& input : v) {
      if (input.size() == size_t(len-1)) {
        Input temp = input;
        temp.push_back(0);
        for (int i=0; i < ALPHABET_SIZE; ++i) {
          temp.back() = i;
          r.push_back(temp);
        }
      }
    }
    return r;
  }
}

std::vector<std::vector<int>> generate_permutations(int len) {
#ifdef NO_RECOLORING
  std::vector<int> p(len);
  std::iota(p.begin(), p.end(), 0);
  return { std::move(p) };
#else
  if (len == 0) {
    return { std::vector<int>() };
  } else {
    std::vector<std::vector<int>> v = generate_permutations(len - 1);
    std::vector<std::vector<int>> r;
    for (auto&& p : v) {
      auto temp = p;
      temp.push_back(0);
      for (int i=0; i < ALPHABET_SIZE; ++i) {
        temp.back() = i;
        r.push_back(temp);
      }
    }
    return r;
  }
#endif
}

int main(int argc, char **argv) {
  assert(argc == 2 && "Usage: ./count-dfas n, where n is the number of DFA states");
  int n = atoi(argv[1]);
  assert(n <= MAXN && "Increase MAXN and recompile");

  std::vector<Input> all_inputs = generate_inputs(2*n - 1);
  printf("Number of inputs: %zu\n", all_inputs.size());

  std::vector<std::vector<int>> all_alphabet_permutations = generate_permutations(ALPHABET_SIZE);
  printf("Number of permutations: %zu\n", all_alphabet_permutations.size());

  std::mutex m;
  std::unordered_set<Language> all_langs;
  std::vector<std::thread> workers;
  for (int i=1; i < (n==1 ? 2 : n); ++i) {
    workers.emplace_back([&, i]() {
      std::unordered_set<Language> local_langs;
      all_dfas<MAXN>(n, n, i, [&](DFA& d) {
        assert(d.nstates_ == n);
        assert(d.accepting_ == i);
        auto lang = lexically_first_language_recognized_by(d, all_inputs, all_alphabet_permutations);
        local_langs.insert(std::move(lang));
      });
      auto lk = std::lock_guard<std::mutex>(m);
      all_langs.merge(std::move(local_langs));
    });
  }
  for (auto&& t : workers) t.join();
  printf("%zu\n", 2*all_langs.size());
}
