// Compile with "g++ -W -Wall -O2 count.cpp"

/*
 a(n) = Count of languages recognizable by a DFA with n states.

 For example, a(1) = 2 (the empty language and the universal language).
 For a(2) = 26, there are thirteen recognizable languages plus their complements:
  1  .*         2  NOT(1)
  3  (a|b.)*    7  NOT(3)
  5  (a|ba*b)*  9  NOT(5)
  6  a*         10 NOT(6)
  8  .*b        4  NOT(8)
  11 (b|a.)*    15 NOT(11)
  12 (b|ab*a)*  16 NOT(12)
  14 b*         18 NOT(14)
  17 .*a        13 NOT(17)
  19 (..)*      23 NOT(19)
  20 (.b*a)*    24 NOT(20)
  21 (.a*b)*    25 NOT(21)
  26 ..*        22 NOT(26)

 To determine whether two superficially different DFAs accept the same language,
 we just run them brute-force on all possible input strings up to some given length `i`.
 (Length `2n-1` suffices to distinguish any two `n`-state DFAs.)

 Run as `./count n`, where `n` is the number of states (e.g. 4).

 a(n) = 2, 26, 1054, 57068, 3762374, ...

 Actually, to save time, we avoid counting any DFA that doesn't accept the
 empty string; and then we can just double the final tally.

 a(n) = 1, 13,  576, 28534, 1881187, ...
*/

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unordered_set>
#include <vector>

#ifndef MAXN
 #define MAXN 10 // "10 states should be enough for anybody"
#endif

using Input = std::string;

Input bits_as_string(int n, int bits) {
  std::string s;
  s.resize(n);
  for (int i=0; i < n; ++i) {
    s[i] = ((bits >> i) & 0x1);
  }
  return s;
}

struct State {
  int next_[2];
};

struct DFA {
  State states_[MAXN];
  int accepting_;
  int nstates_;
};

// A sample one-state DFA accepting the universal language.
DFA universal_language() {
  DFA d;
  d.states_[0].next_[0] = 0;
  d.states_[0].next_[1] = 0;
  d.accepting_ = 1;
  d.nstates_ = 1;
  return d;
}

void print_dfa(const DFA& d) {
  for (int s = 0; s < d.nstates_; ++s) {
    if (s != 0) printf("; ");
    printf(" State %d: a->%d, b->%d", s+1, d.states_[s].next_[0]+1, d.states_[s].next_[1]+1);
    if (s < d.accepting_) printf(", accepting");
  }
  printf("\n");
}

bool dfa_accepts(const DFA& d, const Input& input) {
  int s = 0;
  for (char c : input) {
    s = d.states_[s].next_[c];
    assert(s < d.nstates_);
  }
  return s < d.accepting_;
}

template<int RecursionBound, class F>
void all_dfas(int n, int m, const F& f) {
  // Yield all DFAs with states 1..m inclusive.
  if (m == 0) {
    DFA d;
    d.nstates_ = n;
    f(d);
  } else if constexpr (RecursionBound == 0) {
    assert(false && "Should never happen");
  } else {
    all_dfas<RecursionBound-1>(n, m-1, [&](DFA& d) {
      // Is this DFA already closed, i.e., state m-1 isn't even reachable?
      bool is_closed = (m >= 2);
      for (int i=0; i < m-1; ++i) {
        if (d.states_[i].next_[0] >= m-1 || d.states_[i].next_[1] >= m-1) {
          is_closed = false;
          break;
        }
      }
      if (is_closed) {
        d.states_[m-1].next_[0] = m-1;
        d.states_[m-1].next_[1] = m-1;
        f(d);
      } else {
        for (int i=0; i < n; ++i) {
          for (int j=0; j < n; ++j) {
            d.states_[m-1].next_[0] = i;
            d.states_[m-1].next_[1] = j;
            f(d);
          }
        }
      }
    });
  }
}

using Language = std::string;

Language language_recognized_by(const DFA& d, const Input *inputs, size_t n) {
  std::string lang;
  lang.resize(n);
  for (size_t i = 0; i < n; ++i) {
    lang[i] = dfa_accepts(d, inputs[i]) ? 1 : 0;
  }
  return lang;
}

int main(int argc, char **argv) {
  assert(argc == 2 && "Usage: ./count n, where n is the number of DFA states");
  int n = atoi(argv[1]);
  assert(n <= MAXN && "Increase MAXN and recompile");

  std::vector<Input> all_inputs;
  all_inputs.reserve(1u << (2*n-1));
  for (int i=0; i < 2*n-1; ++i) {
    for (int j=0; j < (1 << i); ++j) {
      all_inputs.push_back(bits_as_string(i, j));
    }
  }

  std::unordered_set<Language> all_langs;
  all_dfas<MAXN>(n, n, [&](DFA& d) {
    for (int i=1; i < (n==1 ? 2 : n); ++i) {
      d.accepting_ = i;
      auto lang = language_recognized_by(d, all_inputs.data(), all_inputs.size());
      if (all_langs.insert(std::move(lang)).second) {
        printf("%zu.  ", all_langs.size());
        print_dfa(d);
      }
    }
  });
}
