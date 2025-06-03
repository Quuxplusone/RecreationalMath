#pragma once

#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <unordered_map>

using Move = int;
using Rotation = int;
// 0 = identity
// 1,2,3 = rotate left (1,2,3) times
// 4 = flip horizontal
// 5,6,7 = flip horizontal then rotate left (1,2,3) times

inline Rotation compose(Rotation a, Rotation b) {
  int m[8][8] = {
    {0,1,2,3,4,5,6,7}, // a * 0
    {1,2,3,0,5,6,7,4}, // a * 1
    {2,3,0,1,6,7,4,5}, // a * 2
    {3,0,1,2,7,4,5,6}, // a * 3
    {4,7,6,5,0,3,2,1}, // a * 4
    {5,4,7,6,1,0,3,2}, // a * 5
    {6,5,4,7,2,1,0,3}, // a * 6
    {7,6,5,4,3,2,1,0}, // a * 7
  };
  return m[b][a];
}

inline Rotation invert(Rotation a) {
  int m[8] = {0,3,2,1,4,5,6,7};
  return m[a];
}

struct Board {
  char cells_[B][B] = {};

  static Board from_string(const char *s) {
    Board b;
    for (int i=0; i < B*B; ++i) {
      char who = s[i];
      assert(who == '.' || who == 'x' || who == 'o');
      b.cells_[i / B][i % B] = (who == '.') ? 0 : (who == 'x') ? 1 : 2;
    }
    return b;
  }

  std::string stringify() const {
    std::string s(B*B, '\0');
    for (int j=0; j < B; ++j) {
      for (int i=0; i < B; ++i) {
        char who = cells_[j][i];
        s[j*B+i] = (who == 0) ? '.' : (who == 1) ? 'x' : 'o';
      }
    }
    return s;
  }
  Board rotated(int rotation) const {
    Board b = *this;
    if (rotation == 0) {
      // do nothing
    } else if (rotation == 1) {
      for (int j=0; j < B; ++j) {
        for (int i=0; i < B; ++i) {
          b.cells_[j][i] = cells_[i][B-j-1];
        }
      }
    } else if (rotation == 2) {
      for (int j=0; j < B; ++j) {
        for (int i=0; i < B; ++i) {
          b.cells_[j][i] = cells_[B-j-1][B-i-1];
        }
      }
    } else if (rotation == 3) {
      for (int j=0; j < B; ++j) {
        for (int i=0; i < B; ++i) {
          b.cells_[j][i] = cells_[B-i-1][j];
        }
      }
    } else if (rotation == 4) {
      for (int j=0; j < B; ++j) {
        for (int i=0; i < B; ++i) {
          b.cells_[j][i] = cells_[j][B-i-1];
        }
      }
    } else if (rotation == 5) {
      for (int j=0; j < B; ++j) {
        for (int i=0; i < B; ++i) {
          b.cells_[j][i] = cells_[i][j];
        }
      }
    } else if (rotation == 6) {
      for (int j=0; j < B; ++j) {
        for (int i=0; i < B; ++i) {
          b.cells_[j][i] = cells_[B-j-1][i];
        }
      }
    } else {
      for (int j=0; j < B; ++j) {
        for (int i=0; i < B; ++i) {
          b.cells_[j][i] = cells_[B-i-1][B-j-1];
        }
      }
    }
    return b;
  }
  Rotation canonical_rotation() const {
    int minr = 0;
    std::string mins = stringify();
    for (int r=1; r < 8; ++r) {
      std::string s = this->rotated(r).stringify();
      if (s < mins) {
        mins = std::move(s);
        minr = r;
      }
    }
    return minr;
  }

  int number_of_xes() const {
    int n = 0;
    for (int j=0; j < B; ++j) {
      for (int i=0; i < B; ++i) {
        n += (cells_[j][i] == 1);
      }
    }
    return n;
  }

  void apply_move(Move m, int who) {
    assert(0 <= m && m <= B*B);
    assert(who == 1 || who == 2);
    assert(cells_[m / B][m % B] == 0);
    cells_[m / B][m % B] = who;
  }

  static Move rotated_move(Move m, Rotation r) {
    int j = m / B;
    int i = m % B;
    struct {
      int j; int i;
    } res;
    switch (r) {
      case 0: res = {j, i}; break;
      case 1: res = {B-i-1, j}; break;
      case 2: res = {B-j-1, B-i-1}; break;
      case 3: res = {i, B-j-1}; break;
      case 4: res = {j, B-i-1}; break;
      case 5: res = {i, j}; break;
      case 6: res = {B-j-1, i}; break;
      case 7: res = {B-i-1, B-j-1}; break;
    }
    return (res.j * B + res.i);
  }
};

struct Oracle {
  std::unordered_map<std::string, Move> dict_;

  void read_from_file(const char *fname) {
    FILE *fp = fopen(fname, "r");
    assert(fp != nullptr);
    char s[B*B + 10] = {};
    while (fscanf(fp, "%s", s) == 1) {
      assert(strchr(s, 'X') != nullptr);
      int m = (strchr(s, 'X') - s);
      s[m] = '.';
      Board b = Board::from_string(s);
      assert(b.canonical_rotation() == 0);
      dict_[s] = m;
    }
    fclose(fp);
  }

  void write_to_file(const char *fname) const {
    FILE *fp = fopen(fname, "w");
    assert(fp != nullptr);
    for (auto&& [s, m] : dict_) {
      auto modified_s = s;
      assert(modified_s[m] == '.');
      modified_s[m] = 'X';
      fprintf(fp, "%s\n", modified_s.c_str());
    }
    fclose(fp);
  }

  void read_compressed_from_file(const char *fname) {
    FILE *fp = fopen(fname, "r");
    assert(fp != nullptr);
    char s[B*B + 10] = {};
    while (fscanf(fp, "%s", s) == 1) {
      assert(strchr(s, 'X') != nullptr);
      int m = (strchr(s, 'X') - s);
      s[m] = '.';
      if (strchr(s, 'O') == nullptr) {
        dict_[s] = m;
        continue;
      }
      for (int i=0; i < B*B; ++i) {
        if (s[i] != 'O') continue;
        std::string modified_s = s;
        for (auto& c : modified_s) if (c == 'O') c = '.';
        modified_s[i] = 'o';
        Board b = Board::from_string(modified_s.c_str());
        Rotation r = b.canonical_rotation();
        dict_[b.rotated(r).stringify()] = Board::rotated_move(m, r);
      }
    }
    fclose(fp);
  }

  void write_compressed_to_file(const char *fname) const {
    auto pattern_in_common = [](std::string& a, const std::string& b, bool& has_wildcards) {
      assert(a.size() == b.size());
      if (!has_wildcards) {
        int ax = -1;
        int bx = -1;
        for (int i=0; i < a.size(); ++i) {
          if (a[i] == b[i]) continue;
          else if (a[i] == 'o' && b[i] == '.' && ax == -1) ax = i;
          else if (a[i] == '.' && b[i] == 'o' && bx == -1) bx = i;
          else return false;
        }
        if (ax == -1 || bx == -1) return false;
        a[ax] = 'O';
        a[bx] = 'O';
        has_wildcards = true;
        return true;
      } else {
        int bx = -1;
        for (int i=0; i < a.size(); ++i) {
          if (a[i] == b[i]) continue;
          else if (a[i] == 'O' && b[i] == '.') continue;
          else if (a[i] == '.' && b[i] == 'o' && bx == -1) bx = i;
          else return false;
        }
        if (bx == -1) return false;
        a[bx] = 'O';
        return true;
      }
    };
    FILE *fp = fopen(fname, "w");
    assert(fp != nullptr);
    auto dict2 = dict_;
    while (!dict2.empty()) {
      auto [s, m] = std::pair<std::string, Move>(*dict2.begin());
      dict2.erase(dict2.begin());
      bool has_wildcards = false;
      // We don't really have a good way to compress this stuff.
      // TODO FIXME BUG HACK: make this better?
      for (auto it = dict2.begin(); it != dict2.end(); ) {
        if (it->second == m && pattern_in_common(s, it->first, has_wildcards)) {
          it = dict2.erase(it);
        } else {
          ++it;
        }
      }
      assert(s[m] == '.');
      s[m] = 'X';
      fprintf(fp, "%s\n", s.c_str());
    }
    fclose(fp);
  }

  int move_for(const Board& b) const {
    Rotation r = b.canonical_rotation();
    std::string s = b.rotated(r).stringify();
    auto it = dict_.find(s);
    if (it == dict_.end()) return -1;
    return Board::rotated_move(it->second, invert(r));
  }

  void add_response(const Board& b, Move m) {
    Rotation r = b.canonical_rotation();
    std::string s = b.rotated(r).stringify();
    dict_[s] = Board::rotated_move(m, r);
  }

  void remove_response(const Board& b) {
    Rotation r = b.canonical_rotation();
    std::string s = b.rotated(r).stringify();
    auto it = dict_.find(s);
    if (it != dict_.end()) {
      dict_.erase(it);
    }
  }
};

inline void display_board(const Board& b) {
  printf("   ");
  for (int i=0; i < B; ++i) {
    printf("%c", 'A'+i);
  }
  printf("\n");
  for (int j=0; j < B; ++j) {
    printf("%2d ", 1+j);
    for (int i=0; i < B; ++i) {
      char who = b.cells_[j][i];
      printf("%c", (who == 0) ? '.' : (who == 1) ? 'x' : 'o');
    }
    printf("\n");
  }
}
