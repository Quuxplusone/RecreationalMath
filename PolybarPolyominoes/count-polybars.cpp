// Compile with:
//   g++ -O2 -W -Wall -std=c++20 count-polybars.cpp -DBARSIZE=$p -DBARSPER=$n -DNDEBUG
// By default we count into a red-black tree of strings, which wastes a lot of memory,
// which means we spend a lot of time swapping and therefore waste a lot of time.
// To count into a trie instead, clone github.com/Tessil/hat-trie and add the compiler flags:
//   -DUSE_TSL_HTRIE -I ~/hat-trie/include

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <set>
#include <utility>

#ifndef BARSIZE
 #define BARSIZE 5    // polybar order (3=I-tromino, 4=I-tetromino, etc.), i.e., "p"
#endif
#ifndef BARSPER
 #define BARSPER 5    // how many bars per polybar, i.e., "n"
#endif
#define GRIDSIZE (BARSIZE*2*BARSPER)
#define SMALLGRIDSIZE (BARSIZE*BARSPER)

using Rotation = int;

namespace Detail {

template<int N>
struct Board {
  unsigned char cells_[N][N] = {};
  int mini_ = N, maxi_ = 0;
  int minj_ = N, maxj_ = 0;

  explicit Board() = default;

  template<int BiggerN>
  explicit Board(const Board<BiggerN>& b) {
    static_assert(BiggerN > N);
    mini_ = b.mini_;
    minj_ = b.minj_;
    maxi_ = b.maxi_;
    maxj_ = b.maxj_;
    int weight = 0;
    for (int j=0; j < N; ++j) {
      for (int i=0; i < N; ++i) {
        cells_[j][i] = b.cells_[j][i];
        weight += cells_[j][i];
      }
    }
    // This should be used only to "shrink" boards for recording in sets.
    // So weight should be a multiple of BARSIZE.
    assert(weight % BARSIZE == 0);
  }

  template<bool CheckTouching>
  bool accrete_polybar(int j, int i, bool horizontal) {
    if (horizontal) {
      if (j < 0 || j + BARSIZE > N) return false;
      if (i < 0 || i >= N) return false;
      bool touching = (i-1 >= 0 && cells_[j][i-1] != 0) || (i+BARSIZE < N && cells_[j][i+BARSIZE] != 0);
      for (int d=0; d < BARSIZE; ++d) {
        if (i+d >= N || cells_[j][i+d] != 0)
          return false;
        if (j+1 < N) touching |= (cells_[j+1][i+d] != 0);
        if (j-1 >= 0) touching |= (cells_[j-1][i+d] != 0);
      }
      if (CheckTouching && !touching) return false;
      for (int d=0; d < BARSIZE; ++d) {
        cells_[j][i+d] = 1;
      }
      mini_ = std::min(mini_, i);
      maxi_ = std::max(maxi_, i+BARSIZE);
      minj_ = std::min(minj_, j);
      maxj_ = std::max(maxj_, j+1);
    } else {
      if (i < 0 || i + BARSIZE > N) return false;
      if (j < 0 || j >= N) return false;
      bool touching = (j-1 >= 0 && cells_[j-1][i] != 0) || (j+BARSIZE < N && cells_[j+BARSIZE][i] != 0);
      for (int d=0; d < BARSIZE; ++d) {
        if (j+d >= N || cells_[j+d][i] != 0)
          return false;
        if (i+1 < N) touching |= (cells_[j+d][i+1] != 0);
        if (i-1 >= 0) touching |= (cells_[j+d][i-1] != 0);
      }
      if (CheckTouching && !touching) return false;
      for (int d=0; d < BARSIZE; ++d) {
        cells_[j+d][i] = 1;
      }
      mini_ = std::min(mini_, i);
      maxi_ = std::max(maxi_, i+1);
      minj_ = std::min(minj_, j);
      maxj_ = std::max(maxj_, j+BARSIZE);
    }
    return true;
  }

  Board shifted() const {
    Board b;
    for (int j=0; j < N; ++j) {
      for (int i=0; i < N; ++i) {
        if (j < minj_) assert(cells_[j][i] == 0);
        if (j >= maxj_) assert(cells_[j][i] == 0);
        if (i < mini_) assert(cells_[j][i] == 0);
        if (i >= maxi_) assert(cells_[j][i] == 0);
        b.cells_[j][i] = (j+minj_ < N && i+mini_ < N) && cells_[j+minj_][i+mini_];
      }
    }
    b.mini_ = 0;
    b.maxi_ = maxi_ - mini_;
    b.minj_ = 0;
    b.maxj_ = maxj_ - minj_;
    return b;
  }

  struct Less {
    bool operator()(const Board& a, const Board& b) const {
      const unsigned char *ap = a.cells_[0];
      const unsigned char *bp = b.cells_[0];
      return std::lexicographical_compare(ap, ap + (N * N), bp, bp + (N * N));
    }
  };

  Board rotated(Rotation rotation) const {
    Board b = *this;
    if (rotation == 0) {
      // do nothing
    } else if (rotation == 1) {
      for (int j=0; j < N; ++j) {
        for (int i=0; i < N; ++i) {
          b.cells_[j][i] = cells_[i][N-j-1];
        }
      }
      b.minj_ = N-maxi_;
      b.maxj_ = N-mini_;
      b.mini_ = minj_;
      b.maxi_ = maxj_;
    } else if (rotation == 2) {
      for (int j=0; j < N; ++j) {
        for (int i=0; i < N; ++i) {
          b.cells_[j][i] = cells_[N-j-1][N-i-1];
        }
      }
      b.minj_ = N-maxj_;
      b.maxj_ = N-minj_;
      b.mini_ = N-maxi_;
      b.maxi_ = N-mini_;
    } else if (rotation == 3) {
      for (int j=0; j < N; ++j) {
        for (int i=0; i < N; ++i) {
          b.cells_[j][i] = cells_[N-i-1][j];
        }
      }
      b.minj_ = mini_;
      b.maxj_ = maxi_;
      b.mini_ = N-maxj_;
      b.maxi_ = N-minj_;
    } else if (rotation == 4) {
      for (int j=0; j < N; ++j) {
        for (int i=0; i < N; ++i) {
          b.cells_[j][i] = cells_[j][N-i-1];
        }
      }
      b.mini_ = N-maxi_;
      b.maxi_ = N-mini_;
      b.minj_ = minj_;
      b.maxj_ = maxj_;
    } else if (rotation == 5) {
      for (int j=0; j < N; ++j) {
        for (int i=0; i < N; ++i) {
          b.cells_[j][i] = cells_[i][j];
        }
      }
      b.minj_ = mini_;
      b.maxj_ = maxi_;
      b.mini_ = minj_;
      b.maxi_ = maxj_;
    } else if (rotation == 6) {
      for (int j=0; j < N; ++j) {
        for (int i=0; i < N; ++i) {
          b.cells_[j][i] = cells_[N-j-1][i];
        }
      }
      b.mini_ = mini_;
      b.maxi_ = maxi_;
      b.minj_ = N-maxj_;
      b.maxj_ = N-minj_;
    } else {
      for (int j=0; j < N; ++j) {
        for (int i=0; i < N; ++i) {
          b.cells_[j][i] = cells_[N-i-1][N-j-1];
        }
      }
      b.minj_ = N-maxi_;
      b.maxj_ = N-mini_;
      b.mini_ = N-maxj_;
      b.maxi_ = N-minj_;
    }
    return b;
  }

  Board<SMALLGRIDSIZE> normalized() const {
    auto smallb = Board<SMALLGRIDSIZE>(this->shifted());
    auto minb = smallb;
    for (Rotation rot = 1; rot < 8; ++rot) {
      auto b = smallb.rotated(rot).shifted();
      if (Board<SMALLGRIDSIZE>::Less()(b, minb)) {
        minb = b;
      }
    }
    return minb;
  }

  std::string to_string() const {
    std::string s;
    int bitcount = 4;
    unsigned char value = 0;
    auto output = [&](unsigned char digit) {
      value <<= 2;
      value |= digit;
      bitcount -= 1;
      if (bitcount == 0) {
        s.push_back(value);
        bitcount = 4;
        value = 0;
        return true;
      }
      return false;
    };
    for (int j=minj_; j < maxj_; ++j) {
      for (int i=mini_; i < maxi_; ++i) {
        output(cells_[j][i] != 0);
      }
      output(2); // special "end of row" marker
    }
    while (!output(3)) {}  // special "end of polyomino" padding
    return s;
  }

  void print() const {
    for (int j=0; j < N; ++j) {
      for (int i=0; i < N; ++i) {
        printf("%c", cells_[j][i] ? 'X' : '.');
      }
      if (j == 0) {
        printf("    mini=%d maxi=%d\n", mini_, maxi_);
      } else if (j == 1) {
        printf("    minj=%d maxj=%d\n", minj_, maxj_);
      } else {
        printf("\n");
      }
    }
    printf("\n");
  }
};

} // namespace Detail

using Board = Detail::Board<GRIDSIZE>;

#ifdef PRINT_PIECES
std::set<Detail::Board<SMALLGRIDSIZE>, Detail::Board<SMALLGRIDSIZE>::Less> g_seen[BARSPER];
#elif defined(USE_TSL_HTRIE)
#include <tsl/htrie_set.h>
tsl::htrie_set<char> g_seen[BARSPER];
#else
std::set<std::string> g_seen[BARSPER];
#endif

void accrete(const Board& b, int pieces_to_place)
{
#ifdef PRINT_PIECES
  auto [it, inserted] = g_seen[pieces_to_place].insert( b.normalized() );
#else
  auto [it, inserted] = g_seen[pieces_to_place].insert( b.normalized().to_string() );
#endif

  if (inserted && pieces_to_place >= 1) {
    Board b2 = b;
    for (int j=b2.minj_ - BARSIZE; j <= b2.maxj_; ++j) {
      for (int i=b2.mini_ - BARSIZE; i <= b2.maxi_; ++i) {
        if (b2.accrete_polybar<true>(j, i, true)) {
          accrete(b2, pieces_to_place - 1);
          b2 = b;
        }
        if (b2.accrete_polybar<true>(j, i, false)) {
          accrete(b2, pieces_to_place - 1);
          b2 = b;
        }
      }
    }
  }
}

int main() {
  Board b;
  b.accrete_polybar<false>(GRIDSIZE/2, GRIDSIZE/2, true);
  accrete(b, BARSPER-1);
  for (int i=0; i < BARSPER; ++i) {
    printf("Accreting %d %d-bars: found %zu free polybars\n", BARSPER-i, BARSIZE, g_seen[i].size());
#ifdef PRINT_PIECES
    for (const auto& elt : g_seen[i]) elt.print();
#endif
  }
}
