#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unordered_set>
#include <vector>
#include "./shared-code.h"

int n_omino = 0;
int moves_to_win_within = 0;
Oracle how_you_moved;
Oracle how_you_moved_without_p2;

struct WinDetector {
  void add(const Board& b, Move m) {
    auto b2 = b.without_p2();
    b2.apply_move(m, 1);
    wins_.insert(b2.rotated(b2.canonical_rotation()).stringify());
  }

  std::vector<Move> moves_for(const Board& b) const {
    Board b2 = b.without_p2();
    // Look for moves that win for X.
    std::vector<Move> result;
    for (int m=0; m < B*B; ++m) {
      int j = m / B;
      int i = m % B;
      // If this is a legal move on b, and X went here on b2...
      if (b.can_move(m)) {
        b2.cells_[j][i] = 1;
        // ...would it make b2 a winning configuration?
        auto it = wins_.find(b2.rotated(b2.canonical_rotation()).stringify());
        if (it != wins_.end()) {
          result.push_back(m);
        }
        b2.cells_[j][i] = 0;
      }
    }
    return result;
  }

  Move find_fork_for(Board b) const {
    // Look for a move such that it creates *two* possible winning moves.
    for (int m=0; m < B*B; ++m) {
      int j = m / B;
      int i = m % B;
      if (b.can_move(m)) {
        // If this is a legal move, and X went here...
        b.cells_[j][i] = 1;
        // ...would it fork player O?
        auto moves = this->moves_for(b);
        if (moves.size() >= 2) {
          return m;
        }
        b.cells_[j][i] = 0;
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
        // Reject this move; it's the base of an already-explored branch of the tree.
        continue;
      }
      return m;
    }
  }
  return -1;
}

Move get_possibly_rotated_move(const Board& b, const Board& b_without_p2, Move m)
{
  // When our Xs are in this configuration, we move at `m`.
  // But if `m` is blocked, maybe we can rotate the board so that
  // the corresponding `m` will not be blocked. E.g.
  //
  // X..                 o..                     o..
  // .xx on a board like .xx can be rotated into .xx
  // ...                 .o.                     Xo.
  //
  for (Rotation r = 0; r < 8; ++r) {
    if (r == 0 || b_without_p2.rotated(r) == b_without_p2) {
      Move rotm = rotated_move(m, r);
      if (b.can_move(rotm)) {
        return rotm;
      }
    }
  }
  // Every rotation of `m` was occupied.
  return -1;
}

bool play_game(Oracle& oracle) {
  Board b;
  bool is_first_move = true;
  Board previous_board;
  while (true) {
    // Player 1's turn

    if (b.number_of_xes() >= moves_to_win_within) {
      // If you haven't won yet, you're over time: you lose!
      // This position is a loser for P1: we don't want to get here anymore!
      how_you_moved.remove_response(previous_board);
      how_you_moved_without_p2.remove_response(previous_board.without_p2());
      oracle.remove_response(previous_board);
      return true;
    }

    b = b.rotated(b.canonical_rotation());
    Move m = oracle.move_for(b);
    if (m == -1 && b.number_of_xes() + 1 >= n_omino) {
      // Maybe we can move so as to create a winning configuration of Xs,
      // even without knowing what the winning shape looks like in general.
      auto wins = win_detector.moves_for(b);
      if (!wins.empty()) {
        oracle.add_response(b, wins[0]);
        return true;
      }
    }
    if (m == -1) {
      m = how_you_moved.move_for(b);
    }
    if (m == -1 && b.number_of_xes() + 2 >= n_omino) {
      m = win_detector.find_fork_for(b);
      if (m != -1) {
        how_you_moved.add_response(b, m);
      }
    }
    if (m == -1) {
      Board b2 = b.without_p2();
      Move m2 = how_you_moved_without_p2.move_for(b2);
      if (m2 != -1) {
        m = get_possibly_rotated_move(b, b2, m2);
      }
    }
    if (m == -1) {
      m = get_human_move(b);
      if (m == -2) {
        // Okay, this position is a loser for P1: we don't want to get here anymore!
        how_you_moved.remove_response(previous_board);
        how_you_moved_without_p2.remove_response(previous_board.without_p2());
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
        how_you_moved_without_p2.add_response(b.without_p2(), m);
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
      // I have no unexplored response for that move. X wins!
      oracle.add_response(b, m);
      return true;
    }
    b2.apply_move(m2, 2);
    b = b2;
    is_first_move = false;
  }
}

int main(int argc, char **argv) {
  if (argc != 4) {
    printf("Usage: ./make-oracle 4 6 oracle.foo-tetromino-b%d-m6.txt", B);
    printf("  The first argument is the (minimum) number of cells to make a win.\n");
    printf("  The second argument is the 'm' value: the number of moves you have to win within.\n");
    printf("  The third argument is the name of the oracle file. I'll overwrite it when I finish running.\n");
    exit(1);
  }
  n_omino = atoi(argv[1]);
  assert(2 <= n_omino && n_omino <= (B*B + 1) / 2);
  moves_to_win_within = atoi(argv[2]);
  assert(n_omino <= moves_to_win_within);

  // We'd like a way to save off a "partial" oracle and "update" it as we go;
  // but in order to do that, we'd need to know how far we explored the game tree,
  // which requires searching the whole tree. What we have here now requires
  // only *descending* the tree, skipping any branch that seems to have been
  // explored: we have no notion that a branch might have been *partially* explored.
  // Therefore, `oracle` is not loaded from the file; it starts off empty.
  //
  Oracle oracle;
  while (true) {
    bool play_again = play_game(oracle);
    if (!play_again) {
      printf("All done! Writing oracle to file...\n");
      oracle.write_compressed_to_file(argv[3]);
      break;
    }
  }
}
