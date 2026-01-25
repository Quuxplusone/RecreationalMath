
#include <algorithm>
#include <print>
#include <ranges>
#include <vector>

#include "primefactor.cpp"

class Sieve {
public:
  explicit Sieve() = default;
  void emplace(int k, int v) {
    d_.emplace_back(k, v);
  }
  size_t size() const { return d_.size(); }
  auto begin() { return d_.begin(); }
  auto end() { return d_.end(); }
private:
  std::vector<std::pair<int, int>> d_;
};

class SetOfInt {
public:
  explicit SetOfInt(int cap) {
    v_.reserve(cap);
  }

  size_t size() const { return v_.size(); }
  bool empty() const { return v_.empty(); }
  void clear() { v_.resize(0); }
  auto begin() const { return v_.begin(); }
  auto end() const { return v_.end(); }
  const auto& sorted_for_output() const { return v_; }

  using const_iterator = std::vector<int>::const_iterator;

  void erase(const_iterator it) {
    v_.erase(it);
  }

  void add(int x) {
    auto it = std::lower_bound(v_.begin(), v_.end(), x);
    if (it != v_.end() && *it == x) {
      // do nothing
    } else {
      v_.insert(it, x);
    }
  }
  void add_back(int x) {
    assert(v_.empty() || (v_.back() < x));
    v_.push_back(x);
  }
  int pop() {
    assert(!v_.empty());
    int result = v_.back();
    v_.pop_back();
    return result;
  }
  bool has(int x) const {
    return std::binary_search(v_.begin(), v_.end(), x);
  }
  void union_without(const SetOfInt& yes, const SetOfInt& no) {
    // std::println("{:}.union_without({:}, {:})", *this, yes, no);
    auto yit = yes.begin();
    auto nit = no.begin();
    auto it = v_.begin();
    while (yit != yes.end()) {
      if (nit != no.end() && *nit < *yit) {
        ++nit;
      } else if (nit != no.end() && *yit == *nit) {
        ++yit;
        ++nit;
      } else {
        it = std::lower_bound(it, v_.end(), *yit);
        if (it != v_.end() && *it == *yit) {
          // do nothing
        } else {
          it = v_.insert(it, *yit);
        }
        ++it;
        ++yit;
      }
    }
  }
  bool is_subset_of(const SetOfInt& bigger) const {
    return std::includes(bigger.begin(), bigger.end(), v_.begin(), v_.end());
  }
  bool is_subset_of(const std::vector<int>& bigger) const {
    return std::includes(bigger.begin(), bigger.end(), v_.begin(), v_.end());
  }
private:
  std::vector<int> v_;
};

class MapToFactors {
public:
  explicit MapToFactors() = default;
  size_t size() const { return k_.size(); }
  void add_back(int k, SetOfInt v) {
    assert(k_.empty() || k_.back() < k);
    k_.push_back(k);
    v_.push_back(std::move(v));
  }
  const SetOfInt& at(int k) {
    auto it = std::lower_bound(k_.begin(), k_.end(), k);
    assert(*it == k);
    return v_[it - k_.begin()];
  }
  void prune() {
    // Any values of k_ whose corresponding v_ would take us
    // outside of the set k_ can be eliminated right away;
    // they'd never be safe to put into our transitive closure.
    bool shrunk = false;
    do {
      shrunk = false;
      for (size_t i=0; i < k_.size(); ) {
        if (!v_[i].is_subset_of(k_)) {
          k_.erase(k_.begin() + i);
          v_.erase(v_.begin() + i);
          shrunk = true;
        } else {
          i += 1;
        }
      }
    } while (shrunk);
  }

  auto begin() const { return std::views::zip(k_, v_).begin(); }
  auto end() const { return std::views::zip(k_, v_).end(); }

private:
  std::vector<int> k_;
  std::vector<SetOfInt> v_;
};

SetOfInt primefactors(int n) {
  SetOfInt result(10);
  constexpr_factors_of(n, &result);
  return result;
}

bool isprime(int n) {
  return (primefactors(n).size() == 1);
}

void check_m(int m, Sieve& sieve) {
  int n = 2*m;

  // We have it on good authority there are no Goldbug numbers in this range.
  if (7000 <= n && n <= 100'000) {
    if (m % 2 == 0) {
      return;
    }
    // m might be prime; check and add it to our sieve
    for (auto&& [p, kp] : sieve) {
      while (kp < m) {
        kp += p;
      }
      if (kp == m) {
        return;
      }
    }
    // m is prime; add it to our sieve
    assert(isprime(m));
    sieve.emplace(m, n);
    return;
  }

  auto ndps = SetOfInt(100);
  bool m_is_prime = true;
  for (auto&& [p, kp] : sieve) {
    while (kp < m) {
      kp += p;
    }
    if (kp == m) {
      m_is_prime = false;
    } else {
      ndps.add_back(p);
    }
  }
  if (m_is_prime && (m % 2) != 0) {
    // m is prime; add it to our sieve
    assert(isprime(m));
    sieve.emplace(m, n);
    // and, since m is prime, 2m is not a Goldbug;
    // but "1718" is in A306746, so let's check it anyway.
  }

#ifndef NDEBUG
  for (int p : ndps) {
    assert(n % p != 0);
    assert(isprime(p));
  }
  // std::println("n={}: ndps={:}", n, ndps);
#endif

  // Now for each NDP, figure out what NDPs it will cause to be added to our set "P".
  MapToFactors ndp_cascade;
  for (int q : ndps) {
    ndp_cascade.add_back(q, primefactors(n - q));
  }
  // NDPs that cause non-NDPs to be added can just be eliminated right now.
  // And so on, iteratively.
  ndp_cascade.prune();

  int lo = (m + 1) >> 1;
  int hi = (m & ~1);
  for (auto&& [p, pfactors] : ndp_cascade) {
    if (p < lo || hi < p) {
      continue;
    }
#if 0
    // Compute our set of NDPs "P", starting with P={p} and expanding by adding
    // the factors of (n - p_i) until either we reach the transitive closure
    // (in which case n is a Goldbug) or attempt to add a factor that's not in ndps.
    auto interior = SetOfInt(ndp_cascade.size());
    auto frontier = pfactors;
    interior.add_back(p);
    while (!frontier.empty()) {
      // Shuffle a factor from frontier to interior, and expand the frontier.
      int q = frontier.pop();
      interior.add(q);
      const auto& margin = ndp_cascade.at(q);
      assert(margin.is_subset_of(ndps));
      frontier.union_without(margin, interior);
      // std::println("frontier is now {:}", frontier);
    }
#ifndef NDEBUG
    assert(frontier.empty());
    for (int f : interior) {
      assert(ndps.has(f));
    }
#endif
#endif
    auto interior = SetOfInt(10);
    for (auto&& [q, qfactors] : ndp_cascade) {
      interior.add_back(q);
    }
    std::println("GOLDBUG {} = 2*{} => {:}", n, n/2, interior);
    return;
  }
  // Dropping out of the for-loop means we failed to find any workable set "P";
  // therefore n is not a Goldbug.
  return;
}

int main() {
  Sieve sieve;
  auto start_time = std::chrono::steady_clock::now();
  for (int m = 3; true; ++m) {
    check_m(m, sieve);
    if (m % 1000 == 0) {
      std::println("{} in {} seconds", m, int(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count() / 1000.0));
    }
  }
}
