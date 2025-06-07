#include <cstdio>
#include <cstdlib>
#include <random>
#include <ctime>
#include "./shared-code.h"

struct Compressor {
  static std::vector<std::string> compress(const Oracle& oracle) {
    auto dict2 = std::vector<std::pair<std::string, Move>>(oracle.dict_.begin(), oracle.dict_.end());

    // Shuffle the rows so that we get different output each time and can pick the best.
    auto g = std::mt19937(time(nullptr));
    std::shuffle(dict2.begin(), dict2.end(), g);

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
    auto pattern_in_common_with_rotations = [&](std::string& a, const std::string& b, Move m, bool& has_wildcards) {
      for (Rotation r = 0; r < 8; ++r) {
        std::string b2 = Board::from_string(b.c_str()).rotated(r).stringify();
        Move m2 = rotated_move(m, r);
        assert(b2[m2] == '.');
        if (a[m2] != 'X') continue;
        b2[m2] = 'X';
        if (pattern_in_common(a, b2, has_wildcards)) {
          return true;
        }
      }
      return false;
    };
    std::vector<std::string> result;
    for (size_t i=0; i < dict2.size(); ++i) {
      auto& [s, m] = dict2[i];
      if (s.empty()) continue;
      assert(s[m] == '.');
      s[m] = 'X';
      bool has_wildcards = false;
      for (size_t j = i + 1; j < dict2.size(); ++j) {
        const auto& [s2, m2] = dict2[j];
        if (s2.empty()) continue;
        if (pattern_in_common_with_rotations(s, s2, m2, has_wildcards)) {
          dict2[j].first.clear();
        }
      }
      result.push_back(std::move(s));
    }
    return result;
  }
};


int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: ./compress-oracle oracle.in.txt\n");
    printf("  The file will be compressed in-place, if and only if compression improves matters.\n");
    printf("  Otherwise the file is unchanged.");
    exit(1);
  }
  Oracle oracle;
  std::vector<std::string> best_strings;
  size_t original_size = oracle.read_compressed_from_file(argv[1]);
  size_t best_size = original_size;
  for (int fails=0; fails < 10; ++fails) {
    auto strings = Compressor::compress(oracle);
    if (strings.size() < best_size) {
      printf("Improved from %zu lines to %zu lines...\n", best_size, strings.size());
      best_strings = std::move(strings);
      best_size = best_strings.size();
      fails = 0;
    }
  }
  // If we've failed ten times in a row to improve the size of the oracle,
  // then write it back out.
  if (!best_strings.empty()) {
    assert(best_strings.size() == best_size);
    FILE *fp = fopen(argv[1], "w");
    assert(fp != nullptr);
    for (const auto& s : best_strings) {
      fprintf(fp, "%s\n", s.c_str());
    }
    fclose(fp);
    printf("Done! Improved from %zu lines to %zu lines.\n", original_size, best_size);
  } else {
    printf("Done! No improvement. The file has not been modified.\n");
  }
}
