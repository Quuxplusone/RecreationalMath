#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include "./shared-code.h"

Move get_human_move(const Board& b) {
  display_board(b);
  char c = 0;
  int r = 0;
  while (true) {
    r = c = 0;
    printf("\nYour move (Q to quit): "); fflush(stdout);
    static char line[1000];
    (void)fgets(line, 1000, stdin);
    if (sscanf(line, "%c%d", &c, &r) != 2) {
      if (c == 'q' || c == 'Q') return -1;
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

void play_game(const Oracle& oracle) {
  Board b;
  while (true) {
    // Player 1's turn
    Rotation r = b.canonical_rotation();
    Move m = oracle.move_for(b.rotated(r));
    if (m == -1) {
      printf("Uh-oh, my oracle is incomplete! You win by default!\n");
      return;
    }
    m = Board::rotated_move(m, invert(r));
    b.apply_move(m, 1);

    // See whether we can handle another (arbitrary) move from Player 2.
    // If not, then either the game's over, or our oracle is incomplete.
    bool game_seems_over = true;
    for (int j=0; j < B; ++j) {
      for (int i=0; i < B; ++i) {
        if (b.cells_[j][i] != 0) continue;
        Move m = j*B+i;
        Board b2 = b;
        b2.apply_move(m, 2);
        game_seems_over = (oracle.move_for(b2) == -1);
        goto break_loop;
      }
    }
    break_loop: ;
    if (game_seems_over) {
      printf("I think I just won the game!\n");
      printf("Either that, or my oracle is incomplete. Either way the game is over.\n\n");
      display_board(b);
      printf("\n");
      return;
    }

    // Player 2's turn
    m = get_human_move(b);
    if (m == -1) {
      return;
    }
    b.apply_move(m, 2);
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: ./play-against-oracle oracle.foo-tetromino-b%d-m42.txt\n", B);
    exit(1);
  }
  Oracle oracle;
  oracle.read_compressed_from_file(argv[1]);
  play_game(oracle);
}
