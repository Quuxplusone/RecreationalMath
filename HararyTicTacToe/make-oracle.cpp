#include <cassert>
#include <cstdio>
#include <string>
#include <unordered_map>
#include "./shared-code.h"

Move get_human_move(const Board& b) {
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

bool play_game(Oracle& oracle, Oracle& how_you_moved) {
  Board b;
  bool is_first_move = true;
  Board previous_board;
  while (true) {
    // Player 1's turn
    b = b.rotated(b.canonical_rotation());
    Move m = oracle.move_for(b);
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
        printf("If this move created a win for Player 1, say 'Y'.\n");
        static char line[1000];
        fgets(line, 1000, stdin);
        if (line[0] == 'y' || line[0] == 'Y') {
          oracle.add_response(b, m);
          return true;
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

int main() {
  Oracle oracle;
  oracle.read_compressed_from_file("oracle.in.txt");
  Oracle how_you_moved;
  while (true) {
    bool play_again = play_game(oracle, how_you_moved);
    oracle.write_compressed_to_file("oracle.out.txt");
    if (!play_again) {
      break;
    }
  }
}
