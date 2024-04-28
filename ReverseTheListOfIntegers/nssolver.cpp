// Compile with e.g. `g++ -std=c++17 -O2 -DN=10 -DS=19 nssolver.cpp`

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <ctime>
#include <iterator>
#include <numeric>
#include <set>
#include <vector>
#include "combinations.h" // https://github.com/HowardHinnant/combinations

using Int = signed char;

struct Position {
  Int data_[N];
  int size_;
  struct Less {
    bool operator()(const Position& lhs, const Position& rhs) const {
      if (lhs.size_ != rhs.size_) return lhs.size_ < rhs.size_;
      return std::lexicographical_compare(lhs.data_, lhs.data_ + lhs.size_, rhs.data_, rhs.data_ + rhs.size_);
    }
  };
  friend bool operator==(const Position& lhs, const Position& rhs) {
    return std::equal(lhs.data_, lhs.data_ + lhs.size_, rhs.data_, rhs.data_ + rhs.size_);
  }
  friend bool operator!=(const Position& lhs, const Position& rhs) {
    return !std::equal(lhs.data_, lhs.data_ + lhs.size_, rhs.data_, rhs.data_ + rhs.size_);
  }
  bool contains(Int k) const {
    return std::find(data_, data_ + size_, k) != data_ + size_;
  }
  Position reversed() const {
    auto copy = *this;
    std::reverse(copy.data_, copy.data_ + copy.size_);
    return copy;
  }
  void print() const {
    for (int i=0; i < size_; ++i) {
      printf("%s%d", (i != 0 ? " " : ""), int(data_[i]));
    }
  }
};

template<class Function>
bool for_each_neighbor_position(int constrain_length, const Position& p, const Function& f) {
  if (p.size_ < constrain_length) {
    // Try splitting
    for (int i = 0; i < p.size_; ++i) {
      Position n;
      n.size_ = p.size_ + 1;
      std::copy(p.data_, p.data_ + i, n.data_);
      std::copy(p.data_ + i + 1, p.data_ + p.size_, n.data_ + i + 2);
      for (Int lo = 1; true; ++lo) {
        Int hi = p.data_[i] - lo;
        if (hi <= lo) break;
        if (p.contains(lo) || p.contains(hi)) {
          continue;
        }
        n.data_[i] = lo;
        n.data_[i+1] = hi;
        bool success = f(n);
        if (success) return true;
        n.data_[i] = hi;
        n.data_[i+1] = lo;
        success = f(n);
        if (success) return true;
      }
    }
  }
  // Try merging
  for (int i = 1; i < p.size_; ++i) {
    Int sum = p.data_[i-1] + p.data_[i];
    if (sum > N || p.contains(sum)) {
      continue;
    }
    Position n;
    n.size_ = p.size_ - 1;
    std::copy(p.data_, p.data_ + i - 1, n.data_);
    n.data_[i-1] = sum;
    std::copy(p.data_ + i + 1, p.data_ + p.size_, n.data_ + i);
    bool success = f(n);
    if (success) return true;
  }
  return false;
}

int moves_to_reverse(const Position& originalp, int constrain_length) {
  if (originalp.size_ == 1) return 0;
  Position reversed_originalp = originalp.reversed();
  std::set<Position, Position::Less> wrong_interior;
  std::set<Position, Position::Less> right_interior;
  std::set<Position, Position::Less> frontier = { {originalp} };
  int moves = 1;
  while (true) {
    if (true) {
      auto oldfrontier = std::move(frontier);
      for (const Position& p : oldfrontier) {
        (void)for_each_neighbor_position(constrain_length, p, [&](const Position& n) {
          // n has "wrong" parity.
          if (wrong_interior.find(n) == wrong_interior.end()) {
            frontier.insert(n);
          }
          return false;
        });
      }
      if (frontier.empty()) return -1;
      right_interior.merge(std::move(oldfrontier));
    }
    moves += 1;
    if (true) {
      auto oldfrontier = std::move(frontier);
      for (const Position& p : oldfrontier) {
        bool success = for_each_neighbor_position(constrain_length, p, [&](const Position& n) {
          // n has "right" parity.
          if (right_interior.find(n) == right_interior.end()) {
            if (n == reversed_originalp) return true;
            frontier.insert(n);
          }
          return false;
        });
        if (success) return moves;
      }
      if (frontier.empty()) return -1;
      wrong_interior.merge(std::move(oldfrontier));
    }
    moves += 1;
  }
}

template<class Function>
void for_each_list(int k, const Function& f) {
  int nums[N];
  std::iota(nums, nums + N, 1);
  if (k == 2) {
    assert(S - N < N);
    Position p;
    p.data_[0] = N;
    p.data_[1] = S - N;
    p.size_ = 2;
    f(p);
    return;
  }
  int size = k;
  for_each_permutation(nums, nums + size - 1, nums + N - 1, [&](auto first, auto last) {
    // We only need to check half of all the positions, since the other half are just the first half reversed.
    if (*first > last[-1]) return false;
    if (std::accumulate(first, last, N) != S) {
      return false;
    }
    Position p;
    std::copy(first, last, p.data_);
    p.size_ = size;
    p.data_[size - 1] = N;
    f(p);
    for (int i = size - 1; i > 0; --i) {
      std::swap(p.data_[i], p.data_[i - 1]);
      f(p);
    }
    return false;
  });
}

int main() {
  int C = 0;
  int M = 0;
  int start = time(NULL);
  std::vector<Position> solvable;
  for (int k = 2; k < N; ++k) {
    int minsum = N;
    int maxsum = N;
    for (int i = 1; i < k; ++i) {
      minsum += i;
      maxsum += N-i;
    }
    if (!(minsum <= S && S <= maxsum)) {
      continue;
    }
    for_each_list(k, [&](Position p) {
      int moves = moves_to_reverse(p, N-1);
      if (moves == -1) return;
      // The list is solvable.
      solvable.push_back(p);
      C += 2;
      if (moves > M) {
        M = moves;
      }
    });
  }

  if (C == 0) {
    printf("N=%d, S=%d: count 0 -- in %d seconds\n", N, S, int(time(NULL) - start));
  } else {
    printf("N=%d, S=%d: count %d, max %d -- in %d seconds\n", N, S, C, M, int(time(NULL) - start));
  }
}
