
#include <algorithm>
#include <cassert>
#include <chrono>
#include <map>
#include <print>
#include <ranges>
#include <set>
#include <vector>

class SetOfInt {
public:
  using const_iterator = std::vector<int>::const_iterator;

  explicit SetOfInt() = default;
  void add_back(int x) {
    assert(v_.empty() || v_.back() < x);
    v_.push_back(x);
  }
  void add(int x) {
    auto it = std::lower_bound(v_.begin(), v_.end(), x);
    if (it == v_.end() || *it != x) {
      v_.insert(it, x);
    }
  }
  void add_all(const SetOfInt& yes) {
    auto it = v_.begin();
    for (int x : yes) {
      it = std::lower_bound(it, v_.end(), x);
      if (it == v_.end() || *it != x) {
        it = v_.insert(it, x);
        ++it;
      }
    }
  }
  void discard(int x) {
    auto it = std::lower_bound(v_.begin(), v_.end(), x);
    assert(it != v_.end() && *it == x);
    v_.erase(it);
  }
  void discard_all(const SetOfInt& no) {
    auto it = v_.end();
    for (int x : no | std::views::reverse) {
      it = std::lower_bound(v_.begin(), it, x);
      if (it != v_.end() && *it == x) {
        it = v_.erase(it);
      }
    }
  }
  void union_without(const SetOfInt& yes, const SetOfInt& no) {
    // std::println("{:}.union_without({:}, {:})", *this, yes, no);
    auto yit = yes.begin();
    auto nit = no.begin();
    auto it = v_.begin();
    while (yit != yes.end()) {
      if (nit != no.end() && *nit < *yit) {
        nit = std::lower_bound(nit, no.end(), *yit);
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
  int pop() {
    assert(!v_.empty());
    int r = v_.back();
    v_.pop_back();
    return r;
  }
  const_iterator begin() const { return v_.begin(); }
  const_iterator end() const { return v_.end(); }
  bool empty() const { return v_.empty(); }
  bool has(int x) const { return std::binary_search(v_.begin(), v_.end(), x); }
private:
  std::vector<int> v_;
};

class MapToFactors {
public:
  explicit MapToFactors() = default;
  void addfactor(int k, int v) {
    d_[k].add(v);
  }
  const SetOfInt& at(int k) const {
    static const SetOfInt dummy;
    auto it = d_.find(k);
    return (it == d_.end()) ? dummy : it->second;
  }
  auto begin() const { return d_.begin(); }
  auto end() const { return d_.end(); }
private:
  std::map<int, SetOfInt> d_;
};

std::vector<SetOfInt> sieve(int n)
{
  n /= 2;
  auto r = std::vector<SetOfInt>(n);
  for (int x = 1; x < n; ++x) {
    if (!r[x].empty()) {
      continue;
    }
    int y = 3*x+1;
    while (y < n) {
      r[y].add_back(2*x+1);
      y += 2*x+1;
    }
  }
  return r;
}

SetOfInt check(int n, const std::vector<SetOfInt>& r) {
  auto s = MapToFactors();

  for (int i = 3; i < n / 2; i += 2) {
    if (!r[i/2].empty() || (n % i) == 0) {
      continue;
    }
    if (r[(n-i)/2].empty()) {
      s.addfactor(n - i, i);
    } else {
      for (int f : r[(n-i)/2]) {
        s.addfactor(f, i);
      }
    }
  }
  auto bad = SetOfInt();
  for (auto&& [x, _] : s) {
    if (x >= n/2) {
      bad.add_back(x);
    }
  }
  auto bad_seen = bad;
  auto ok = SetOfInt();
  for (int i = 3; i < n / 2; i += 2) {
    if (r[i/2].empty() && (n % i) != 0) {
      ok.add_back(i);
    }
  }
  while (!bad.empty()) {
    int q = bad.pop();
    const SetOfInt& factors = s.at(q);
#if 1
    for (auto&& p : factors) {
      if (!bad_seen.has(p)) {
        bad.add(p);
        bad_seen.add(p);
        ok.discard(p);
      }
    }
#else
    bad.union_without(factors, bad_seen);
    bad_seen.add_all(factors);
    ok.discard_all(factors);
#endif
  }
  return ok;
}

int main(int, char **argv) {
  int lo = atoi(argv[1]);
  int hi = atoi(argv[2]);
  auto start_time = std::chrono::steady_clock::now();
  auto r = sieve(hi);
  std::println("Setup in {} seconds", int(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count() / 1000.0));
  for (int n = lo; n < hi; n += 2) {
    SetOfInt x = check(n, r);
    if (!x.empty()) {
      std::println("{} {:}", n, x);
    }
    if (n % 1000 == 0) {
      std::println("{} in {} seconds", n, int(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count() / 1000.0));
    }
  }
}
