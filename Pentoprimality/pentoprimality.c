/* https://www.reddit.com/r/askmath/comments/194jmfe/comment/khpnkg2/ */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dancing.h"

void solve();

int First = 0;
long long int Count = 0;

int main() {
    char line[100];
    while (fgets(line, sizeof(line), stdin) != NULL) {
        assert(strlen(line) == 61);
        assert(line[60] == '\n');
        line[60] = '\0';
        First = 1;
        Count = 0;
        solve(line);
printf("%s: %lld\n", line, Count);
    }
    return 0;
}

int is_prime(int x) {
  return (x == 2 || x == 3 || x == 5 || x == 7);
}

/* Coloring cell (1,1) as 1 conflicts with coloring it 2; and it
 * also conflicts with coloring any other cell of that pentomino as 1;
 * and it also conflicts with coloring any neighbor in a different
 * pentomino as 3 or 5.
 */
void odometer(const char *line, char *grid, int i)
{
  int x = i % 10;
  int y = i / 10;
  for (int digit=0; digit <= 4; ++digit) {
    if (x > 0 && line[y*10+(x-1)] != line[i] && !is_prime(grid[y*10+(x-1)] + digit))
      continue;
    if (y > 0 && line[(y-1)*10+x] != line[i] && !is_prime(grid[(y-1)*10+x] + digit))
      continue;
    for (int j=0; j < i; ++j) {
      if (line[j] == line[i] && grid[j] == digit) {
        // This digit has already been used in this pentomino.
        goto next;
      }
    }
    // All checks passed, so far.
    grid[i] = digit;
    if (i+1 == 60) {
      if (First) {
        printf("%s ", line);
        for (int j=0; j < 60; ++j) printf("%d", grid[j]);
        printf("\n");
        First = 0;
      }
      Count += 1;
    } else {
      odometer(line, grid, i+1);
    }
  next: ;
  }
}

void solve(const char *line)
{
  char grid[60];
  memset(grid, '.', 60);
  odometer(line, grid, 0);
}
