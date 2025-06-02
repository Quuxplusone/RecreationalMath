#include <cassert>
#include <cstdio>
#include <cstdlib>
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

bool play_game(Oracle& oracle) {
  Board b;
  while (true) {
    // Player 1's turn
    b = b.rotated(b.canonical_rotation());
    Move m = oracle.move_for(b);
    if (m == -1) {
      printf("Uh-oh, my oracle is incomplete! You win by default!\n");
      return false;
    }
    b.apply_move(m, 1);

    // Player 2's turn
    m = get_human_move(b);
    if (m == -1) {
      return false;
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
  while (true) {
    bool play_again = play_game(oracle);
    if (!play_again) {
      break;
    }
  }
}
