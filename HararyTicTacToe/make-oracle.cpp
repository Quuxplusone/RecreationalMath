#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unordered_set>
#include "./shared-code.h"

struct WinDetector {
  void add(Board b, Move m) {
    b.apply_move(m, 1);
    std::string s = b.stringify();
    for (char& ch : s) {
      if (ch != 'x') {
        ch = '.';
      }
    }
    auto b2 = Board::from_string(s.c_str());
    wins_.insert(b2.rotated(b2.canonical_rotation()).stringify());
  }
  Move move_for(const Board& b) const {
    std::string s = b.stringify();
    std::string ss = s;
    for (char& ch : ss) {
      if (ch != 'x') {
        ch = '.';
      }
    }
    for (int i=0; i < B*B; ++i) {
      if (s[i] == '.') {
        // If this is a legal move, and X went here...
        ss[i] = 'x';
        Board b2 = Board::from_string(ss.c_str());
        auto it = wins_.find(b2.rotated(b2.canonical_rotation()).stringify());
        // ...would it make a winning configuration?
        if (it != wins_.end()) {
          return i;
        }
        ss[i] = '.';
      }
    }
    return -1;
  }

  std::unordered_set<std::string> wins_;
};

static WinDetector win_detector;

Move get_human_move(const Board& b) {
  display_board(b);
  char c = 0;
  int r = 0;
  while (true) {
    r = c = 0;
    printf("\nYour move (Q to restart game, X to lose): "); fflush(stdout);
    static char line[1000];
    (void)fgets(line, 1000, stdin);
    if (sscanf(line, "%c%d", &c, &r) != 2) {
      if (c == 'q' || c == 'Q') return -1;
      if (c == 'x' || c == 'X') return -2;
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

Move get_ai_move(const Oracle& oracle, const Board& b) {
  for (int j=0; j < B; ++j) {
    for (int i=0; i < B; ++i) {
      if (b.cells_[j][i] != 0) continue;
      Move m = j*B+i;
      Board b2 = b;
      b2.apply_move(m, 2);
      if (oracle.move_for(b2) != -1) {
        printf("AI rejecting move %c%d as already-explored...\n", 'A'+(m%B), 1+(m/B));
        continue;
      }
      return m;
    }
  }
  return -1;
}

bool play_game(int n_omino, Oracle& oracle, Oracle& how_you_moved) {
  Board b;
  bool is_first_move = true;
  Board previous_board;
  while (true) {
    // Player 1's turn
    b = b.rotated(b.canonical_rotation());
    Move m = oracle.move_for(b);
    if (m == -1 && b.number_of_xes() + 1 >= n_omino) {
      // Maybe we can move so as to create a winning configuration of Xs,
      // even without knowing what the winning shape looks like in general.
      printf("Consulting win detector (with %zu patterns)...\n", win_detector.wins_.size());
      m = win_detector.move_for(b);
      if (m != -1) {
        oracle.add_response(b, m);
        return true;
      }
    }
    if (m == -1) {
      m = how_you_moved.move_for(b);
    }
    if (m == -1) {
      m = get_human_move(b);
      if (m == -2) {
        // Okay, this position is a loser for P1: we don't want to get here anymore!
        how_you_moved.remove_response(previous_board);
        oracle.remove_response(previous_board);
        return true;
      }
      if (m == -1) {
        return true;
      }
      if (is_first_move) {
        oracle.add_response(b, m);
      } else {
        how_you_moved.add_response(b, m);
        if (b.number_of_xes() + 1 >= n_omino) {
          printf("If this move created a win for Player 1, say 'Y'.\n");
          static char line[1000];
          fgets(line, 1000, stdin);
          if (line[0] == 'y' || line[0] == 'Y') {
            oracle.add_response(b, m);
            win_detector.add(b, m);
            return true;
          }
        }
      }
    }
    previous_board = b;

    // Player 2's turn
    Board b2 = b;
    b2.apply_move(m, 1);
    b2 = b2.rotated(b2.canonical_rotation());
    Move m2 = get_ai_move(oracle, b2);
    if (m2 == -1) {
      if (is_first_move) {
        return false;
      }
      printf("I have no unexplored response for that move! You win!\n");
      oracle.add_response(b, m);
      return true;
    }
    b2.apply_move(m2, 2);
    b = b2;
    is_first_move = false;
  }
}

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("Usage: ./make-oracle 4 oracle.foo-tetromino-b%d-m42.txt", B);
    printf("  The first argument is the (minimum) number of cells to make a win.\n");
    printf("  The second argument is the name of the oracle file. I'll overwrite it as I run.\n");
    exit(1);
  }
  int n_omino = atoi(argv[1]);
  assert(2 <= n_omino && n_omino <= (B*B + 1) / 2);

  // We'd like a way to save off a "partial" oracle and "update" it as we go;
  // but in order to do that, we'd need to know how far we explored the game tree,
  // which requires searching the whole tree. What we have here now requires
  // only *descending* the tree, skipping any branch that seems to have been
  // explored: we have no notion that a branch might have been *partially* explored.
  // Therefore, `oracle` is not loaded from the file; it starts off empty.
  //
  Oracle oracle;
  Oracle how_you_moved;
  while (true) {
    bool play_again = play_game(n_omino, oracle, how_you_moved);
    oracle.write_compressed_to_file(argv[2]);
    if (!play_again) {
      break;
    }
  }
}
