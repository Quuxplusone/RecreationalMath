#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unordered_set>
#include <vector>
#include "./shared-code.h"

static bool any_changes_were_made = false;
static Oracle verified_branches;

struct WinDetector {
  void add(Board b) {
    std::string s = b.stringify();
    for (char& ch : s) {
      if (ch != 'x') {
        ch = '.';
      }
    }
    auto b2 = Board::from_string(s.c_str());
    wins_.insert(b2.rotated(b2.canonical_rotation()).stringify());
  }

  bool contains(Board b) const {
    for (int j=0; j < B; ++j) {
      for (int i=0; i < B; ++i) {
        if (b.cells_[j][i] == 2) {
          b.cells_[j][i] = 0;
        }
      }
    }
    std::string s = b.rotated(b.canonical_rotation()).stringify();
    return wins_.find(s) != wins_.end();
  }

  std::unordered_set<std::string> wins_;
};

static WinDetector win_detector;
static WinDetector nonwin_detector;

Move get_human_move(const Board& b) {
  display_board(b);
  char c = 0;
  int r = 0;
  while (true) {
    r = c = 0;
    printf("\nYour move (Q to exit the verifier, W if this is a win already): "); fflush(stdout);
    static char line[1000];
    (void)fgets(line, 1000, stdin);
    if (sscanf(line, "%c%d", &c, &r) != 2) {
      if (c == 'q' || c == 'Q') return -1;
      if (c == 'w' || c == 'W') return -3;
      continue;
    }
    c = toupper(c);
    if (!(1 <= r && r <= B && 'A' <= c && c < 'A'+B)) {
      printf("Unrecognized move; try again.\n");
    } else {
      Move m = (r-1) * B + (c-'A');
      if (b.cells_[m / B][m % B] != 0) {
        printf("Cell already occupied; try again.\n");
      } else {
        return m;
      }
    }
  }
}

bool verify_tree(Board b, int n_omino, Oracle& oracle) {
  if (verified_branches.move_for(b) != -1) {
    // We've already verified this whole branch (in some other orientation). Prune it.
    return true;
  }
  // Player 1's turn; consult the oracle.
  Move m = oracle.move_for(b);
  if (m == -1) {
    // We have no continuation for this position. But maybe that's because it's a winner.
    if (b.number_of_xes() < n_omino || nonwin_detector.contains(b)) {
      // It's definitely a loser
      printf("The following position lacks a response in the oracle.\n");
      m = get_human_move(b);
      while (true) {
        if (m == -1) {
          printf("Exiting. The oracle file has not been updated.\n");
          exit(0);
        } else if (m == -3) {
          printf("No, it's not a win. Try again.\n");
        } else {
          break;
        }
      }
      oracle.add_response(b, m);
      any_changes_were_made = true;
    } else if (win_detector.contains(b)) {
      // It's a winner!
      return true;
    } else {
      printf("The following position lacks a response in the oracle.\n");
      m = get_human_move(b);
      if (m == -1) {
        printf("Exiting. The oracle file has not been updated.\n");
        exit(0);
      } else if (m == -3) {
        win_detector.add(b);
        return true;
      }
      nonwin_detector.add(b);
      oracle.add_response(b, m);
      any_changes_were_made = true;
    }
  }
  assert(0 <= m && m < B*B);
  verified_branches.add_response(b, m);
  b.apply_move(m, 1);

  // Player 2's turn; recurse on all possible moves.
  bool verified = true;
  for (int m=0; m < B*B; ++m) {
    if (b.cells_[m / B][m % B] != 0) continue;
    b.cells_[m / B][m % B] = 2;
    if (!verify_tree(b, n_omino, oracle)) {
      verified = false;
    }
    b.cells_[m / B][m % B] = 0;
  }
  return verified;
}

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("Usage: ./verify-oracle 4 oracle.foo-tetromino-b%d-m42.txt", B);
    printf("  The first argument is the (minimum) number of cells to make a win.\n");
    printf("  The second argument is the name of the oracle file. I'll overwrite it as I run.\n");
    exit(1);
  }
  int n_omino = atoi(argv[1]);
  assert(2 <= n_omino && n_omino <= (B*B + 1) / 2);

  Oracle oracle;
  oracle.read_compressed_from_file(argv[2]);
  while (true) {
    bool verified = verify_tree(Board(), n_omino, oracle);
    if (verified) {
      if (any_changes_were_made) {
        printf("Changes were made to the oracle. Saving...\n");
        oracle.write_compressed_to_file(argv[2]);
      } else {
        printf("Verified!\n");
      }
      break;
    }
  }
}
