// This program requires a C++20 compiler.
// Compile with `-DUSE_FLAT_SET` for a 6x speed improvement.
// Remember to compile with `-DNDEBUG`!
//
//   g++ -std=c++23 -O2 -DNDEBUG -DUSE_FLAT_SET count-lego-arrangements.cpp
//   ./a.out 6
//

// TODO: This program is wrong! We need to track the bricks themselves,
// not just the filled cells, to distinguish 2x4+2x4 (north-south) from
// 4x2+4x2 (east-west). Yuck.

#include <algorithm>
#include <cassert>
#include <compare>
#include <cstdio>
#include <set>
#include <tuple>
#include <utility>

#ifdef USE_FLAT_SET
#include <flat_set>
#endif

struct Coord {
  int x, y, z;
  void operator-=(Coord rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
  }
  Coord rotate90() const { return {y, -x, z}; }
  friend auto operator<=>(Coord a, Coord b) { return std::tie(a.z, a.x, a.y) <=> std::tie(b.z, b.x, b.y); }
  friend bool operator==(Coord a, Coord b) = default;
};

class CoordSet {
public:
  CoordSet() = default;
  CoordSet(std::initializer_list<Coord> il) : set_(il) {}

  friend auto operator<=>(const CoordSet& a, const CoordSet& b) = default;

  Coord smallest() const {
    return {minX(), minY(), minZ()};
  }
  bool is_normalized() const {
    return minX() == 0 && minY() == 0 && minZ() == 0;
  }

  void inplace_normalize() {
    Coord zero = {minX(), minY(), minZ()};
    if (zero != Coord{0,0,0}) {
#if USE_FLAT_SET
      std::vector<Coord> v = std::move(set_).extract();
      for (Coord& c : v) c -= zero;
      set_.replace(std::move(v));
#else
      CoordSet result;
      while (!set_.empty()) {
        auto nh = set_.extract(set_.begin());
        nh.value() -= zero;
        result.set_.insert(std::move(nh));
      }
      *this = std::move(result);
#endif
    }
    assert(is_normalized());
  }

  void operator+=(CoordSet&& rhs) {
#if USE_FLAT_SET
    set_.insert(std::sorted_unique, rhs.set_.begin(), rhs.set_.end());
#else
    while (!rhs.set_.empty()) {
      auto nh = rhs.set_.extract(rhs.set_.begin());
      set_.insert(std::move(nh));
    }
#endif
  }

  bool intersects(const CoordSet& rhs) const {
    bool result = false;
    struct Dummy {
      bool *result_;
      const Dummy& operator*() const { return *this; }
      Dummy operator++() { return *this; }
      void operator=(const Coord&) const { *result_ = true; }
    };
    std::set_intersection(set_.begin(), set_.end(), rhs.set_.begin(), rhs.set_.end(), Dummy(&result));
    return result;
  }

  void inplace_rotate90_and_normalize() {
    assert(is_normalized());
    if (!set_.empty()) {
      Coord zero = {minY(), -maxX(), minZ()};
#if USE_FLAT_SET
      std::vector<Coord> v = std::move(set_).extract();
      for (Coord& c : v) {
        c = c.rotate90();
        c -= zero;
      }
      std::sort(v.begin(), v.end());
      set_.replace(std::move(v));
#else
      CoordSet result;
      while (!set_.empty()) {
        auto nh = set_.extract(set_.begin());
        nh.value() = nh.value().rotate90();
        nh.value() -= zero;
        result.set_.insert(std::move(nh));
      }
      *this = std::move(result);
#endif
    }
    assert(is_normalized());
  }

  void inplace_rotate_until_minimal() {
    assert(is_normalized());
    CoordSet r = *this;
    r.inplace_rotate90_and_normalize();
    if (r < *this) *this = r;
    r.inplace_rotate90_and_normalize();
    if (r < *this) *this = r;
    r.inplace_rotate90_and_normalize();
    if (r < *this) *this = std::move(r);
  }

  int minX() const {
    if (set_.empty()) return 0;
    return std::min_element(set_.begin(), set_.end(), [](auto&& a, auto&& b) { return a.x < b.x; })->x;
  }
  int minY() const {
    if (set_.empty()) return 0;
    return std::min_element(set_.begin(), set_.end(), [](auto&& a, auto&& b) { return a.y < b.y; })->y;
  }
  int minZ() const {
    if (set_.empty()) return 0;
    return std::min_element(set_.begin(), set_.end(), [](auto&& a, auto&& b) { return a.z < b.z; })->z;
  }
  int maxX() const {
    if (set_.empty()) return 0;
    return std::max_element(set_.begin(), set_.end(), [](auto&& a, auto&& b) { return a.x < b.x; })->x;
  }
  int maxY() const {
    if (set_.empty()) return 0;
    return std::max_element(set_.begin(), set_.end(), [](auto&& a, auto&& b) { return a.y < b.y; })->y;
  }
  int maxZ() const {
    if (set_.empty()) return 0;
    return std::max_element(set_.begin(), set_.end(), [](auto&& a, auto&& b) { return a.z < b.z; })->z;
  }
  int height() const { return maxZ() - minZ() + 1; }

  bool links_to(const CoordSet& rhs) const {
    for (const auto& [x,y,z] : set_) {
      if (rhs.set_.contains({x,y,z+1}) || rhs.set_.contains({x,y,z-1})) {
        return true;
      }
    }
    return false;
  }

#ifdef USE_FLAT_SET
  std::flat_set<Coord> set_;
#else
  std::set<Coord> set_;
#endif
};

CoordSet ns_brick(short x, short y, short z) {
  return {
    {x+0,y+0,z},
    {x+0,y+1,z},  // 1x2
    {x+1,y+0,z},
    {x+1,y+1,z},  // 2x2
    {x+0,y+2,z},
    {x+1,y+2,z},  // 2x3
    {x+0,y+3,z},
    {x+1,y+3,z},  // 2x4
  };
}

CoordSet ew_brick(short x, short y, short z) {
  return {
    {x+0,y+0,z},
    {x+1,y+0,z},  // 1x2
    {x+0,y+1,z},
    {x+1,y+1,z},  // 2x2
    {x+2,y+0,z},
    {x+2,y+1,z},  // 2x3
    {x+3,y+0,z},
    {x+3,y+1,z},  // 2x4
  };
}

std::set<CoordSet> add_a_brick(std::set<CoordSet> inputs) {
  // Now, for all the different places we could put a NS or EW brick,
  // try them. Count the ones that aren't reducible by rotating.
  std::set<CoordSet> outputs;
  for (const CoordSet& input : inputs) {
    for (int x = input.minX() - 3; x <= input.maxX(); ++x) {
      for (int y = input.minY() - 3; y <= input.maxY(); ++y) {
        for (int z = input.minZ(); z <= input.maxZ() + 1; ++z) {
          for (auto makebrick : {ns_brick, ew_brick}) {
            CoordSet newbrick = makebrick(x,y,z);
            if (newbrick.links_to(input) && !newbrick.intersects(input)) {
              CoordSet output = input;
              output += std::move(newbrick);
              output.inplace_normalize();
              output.inplace_rotate_until_minimal();
              outputs.insert(std::move(output));
            }
          }
        }
      }
    }
  }
  return outputs;
}

int main(int argc, char **argv) {
  std::set<CoordSet> prev = {
    ns_brick(0,0,0),
  };
  int MAX = atoi(argv[1]);
  for (int n=2; n <= MAX; ++n) {
    std::set<CoordSet> next = add_a_brick(std::move(prev));
    printf("Count all %d-brick arrangements: %zu\n", n, next.size());
    for (int i=1; i <= n; ++i) {
      printf(" with height=%d: %zu\n", i, (size_t)std::count_if(next.begin(), next.end(), [&](auto&& cs) { return cs.height() == i; }));
    }
    prev = std::move(next);
  }

#if 0
    // Put this inside the loop to print out all the CoordSets.
    for (const CoordSet& cs : next) {
      for (const Coord& c : cs.set_) {
        printf(" (%d,%d,%d)", c.x,c.y,c.z);
      }
      printf("\n");
    }
#endif
}
