// Count free animals buildable from a complete set of trominoes or a complete set of tetrominoes.
// The 11 hexominoes buildable from a complete set of trominoes are:
//
//  XXX XXX XXX XXX   XXX XXX XXX XXXY XXXY  XXXYY XXXYY
// YY   YY   YY   YY  Y    Y    Y   YY    YY     Y    Y
// Y    Y    Y    Y  YY   YY   YY
//
// 11 is 31.4% of the 35 total possible hexominoes.
//
// There are 3721336 icosominoes buildable from a complete set of tetrominoes.
// That's 1.3% of the 2870671950 total possible icosominoes.
//
// As for how many hexacosominoes are buildable from a complete set of pentominoes,
// good luck computing that! The answer is presumably some astronomically small
// percentage of the astronomically large (>10^28) number of hexacosominoes.
//
#include <algorithm>
#include <cassert>
#include <compare>
#include <cstdio>
#include <set>
#include <utility>

#ifndef BARSIZE
 #define BARSIZE 3  // or 4
 #define M 2        // or 5
#endif

#define GRIDSIZE (BARSIZE*2*M) // grid size
#define SMALLGRIDSIZE (BARSIZE*M)

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
  bool accrete_polyomino(int j, int i, const Board<BARSIZE>& piece) {
    if (j < 0 || j >= N) return false;
    if (i < 0 || i >= N) return false;
    assert(piece.mini_ == 0);
    assert(piece.minj_ == 0);
    bool touching = false;
    for (int dj=0; dj < piece.maxj_; ++dj) {
      for (int di=0; di < piece.maxi_; ++di) {
        if (piece.cells_[dj][di] != 0) {
          if (j+dj >= N || i+di >= N || cells_[j+dj][i+di] != 0) return false;
          touching |= (j+dj+1 < N && cells_[j+dj+1][i+di] != 0);
          touching |= (j+dj-1 >= 0 && cells_[j+dj-1][i+di] != 0);
          touching |= (i+di+1 < N && cells_[j+dj][i+di+1] != 0);
          touching |= (i+di-1 >= 0 && cells_[j+dj][i+di-1] != 0);
        }
      }
    }
    if (CheckTouching && !touching) return false;
    for (int dj=0; dj < piece.maxj_; ++dj) {
      for (int di=0; di < piece.maxi_; ++di) {
        if (piece.cells_[dj][di] != 0) {
          cells_[j+dj][i+di] = 1;
          mini_ = std::min(mini_, i+di);
          maxi_ = std::max(maxi_, i+di+1);
          minj_ = std::min(minj_, j+dj);
          maxj_ = std::max(maxj_, j+dj+1);
        }
      }
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

  std::string serialized() const {
    std::string s;
    for (int j=minj_; j < maxj_; ++j) {
      std::string row;
      int blanks = 0;
      for (int i=mini_; i < maxi_; ++i) {
        if (cells_[j][i] == 0) {
          blanks += 1;
        } else {
          row.append(blanks, '.');
          row.push_back('X');
          blanks = 0;
        }
      }
      s += row;
      s.push_back('/');
    }
    s.pop_back();
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

struct Piece {
  int j_, i_;
  std::string cells_;
  std::vector<int> rotations_;
  friend auto operator<=>(const Piece&, const Piece&) = default;

  Detail::Board<BARSIZE> make(Rotation rot) const {
    Detail::Board<BARSIZE> b2;
    for (int j=0; j < this->j_; ++j) {
      for (int i=0; i < this->i_; ++i) {
        if (cells_[j * i_ + i] == 'X') {
          b2.cells_[j][i] = 1;
          b2.mini_ = std::min(b2.mini_, i);
          b2.maxi_ = std::max(b2.maxi_, i+1);
          b2.minj_ = std::min(b2.minj_, j);
          b2.maxj_ = std::max(b2.maxj_, j+1);
        }
      }
    }
    return b2.rotated(rot).shifted();
  }
};

#if BARSIZE == 3
Piece g_pieces[] = {  // the trominoes
  {2, 2, "XXX.", {0, 1, 2, 3}},
  {1, 3, "XXX", {0, 1}},
};
#elif BARSIZE == 4
Piece g_pieces[] = {  // the tetrominoes
  {2, 3, "X.."
         "XXX", {0, 1, 2, 3, 4, 5, 6, 7}},
  {1, 4, "XXXX", {0, 1}},
  {2, 3, "XX."
         ".XX", {0, 1, 4, 5}},
  {2, 3, ".X."
         "XXX", {0, 1, 2, 3}},
  {2, 2, "XX"
         "XX", {0}},
};
#else
 #error "Need a list of polyominoes for this BARSIZE"
#endif

using Board = Detail::Board<GRIDSIZE>;

#ifdef PRINT_PIECES
std::set<Detail::Board<SMALLGRIDSIZE>, Detail::Board<SMALLGRIDSIZE>::Less> g_seen[M];
#else
std::set<std::string> g_seen[M];
#endif

void accrete(const Board& b, int pieces_to_place)
{
#ifdef PRINT_PIECES
  auto [it, inserted] = g_seen[pieces_to_place].insert( b.normalized() );
#else
  auto [it, inserted] = g_seen[pieces_to_place].insert( b.normalized().serialized() );
#endif

  if (inserted && pieces_to_place >= 1) {
    Board b2 = b;
    for (int rot : g_pieces[pieces_to_place].rotations_) {
      auto pieceboard = g_pieces[pieces_to_place].make(rot);
      for (int j=b2.minj_ - pieceboard.maxj_; j <= b2.maxj_; ++j) {
        for (int i=b2.mini_ - pieceboard.maxi_; i <= b2.maxi_; ++i) {
          if (b2.accrete_polyomino<true>(j, i, pieceboard)) {
            accrete(b2, pieces_to_place - 1);
            b2 = b;
          }
        }
      }
    }
  }
}

int main() {
  static_assert(M == (std::end(g_pieces) - std::begin(g_pieces)));
  std::sort(g_pieces + 1, g_pieces + M);
  do {
    Board b;
    Detail::Board<BARSIZE> pieceboard = g_pieces[0].make(0);
    b.accrete_polyomino<false>(GRIDSIZE/2, GRIDSIZE/2, pieceboard);
    accrete(b, M-1);
  } while (std::next_permutation(g_pieces + 1, g_pieces + M));

  for (int i=0; i < M; ++i) {
    printf("Accreting %d %d-ominoes from the set: found %zu free animals\n", M-i, BARSIZE, g_seen[i].size());
#ifdef PRINT_PIECES
    for (const auto& elt : g_seen[i]) elt.print();
#endif
  }
}
