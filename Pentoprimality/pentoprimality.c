/* https://www.reddit.com/r/askmath/comments/194jmfe/comment/khpnkg2/ */

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dancing.h"

void solve();
 int print_pento_result(size_t n, struct data_object **sol, void *info);

void do_error(const char *fmt, ...);

int main(int argc, char **argv)
{
    solve();
    return 0;
}

/* Each row of the matrix corresponds to a possible pentomino placement
 * and coloring. For example, we have one row that corresponds to
 * placing the V pentomino at (1,1) oriented southeast with coloration
 * (1,2,3,4,5); another row that corresponds to placing it there with
 * coloration (1,2,3,5,4); and so on.
 *
 * A row placing the X pentomino at one position/orientation conflicts
 * with any row placing it anywhere else. A row placing e.g. 1 in a
 * given cell conflicts with any row placing 3 or 5 in any adjacent cell.
 */

struct Placement {
  int x, y;
  char name;
  int orientation;
  char digits[5];
};
typedef struct Placement Placement;

#define PLACEMENTS \
  X('F', 0, 3, 3, ".FFFF..F.") \
  X('F', 1, 3, 3, ".F.FFF..F") \
  X('F', 2, 3, 3, ".F..FFFF.") \
  X('F', 3, 3, 3, "F..FFF.F.") \
  X('F', 4, 3, 3, "FF..FF.F.") \
  X('F', 5, 3, 3, "..FFFF.F.") \
  X('F', 6, 3, 3, ".F.FF..FF") \
  X('F', 7, 3, 3, ".F.FFFF..") \
  X('I', 0, 1, 5, "IIIII") \
  X('I', 1, 5, 1, "IIIII") \
  X('L', 0, 2, 4, "L.L.L.LL") \
  X('L', 1, 4, 2, "LLLLL...") \
  X('L', 2, 2, 4, "LL.L.L.L") \
  X('L', 3, 4, 2, "...LLLLL") \
  X('L', 4, 2, 4, ".L.L.LLL") \
  X('L', 5, 4, 2, "LLLL...L") \
  X('L', 6, 2, 4, "LLL.L.L.") \
  X('L', 7, 4, 2, "L...LLLL") \
  X('N', 0, 2, 4, ".NNNN.N.") \
  X('N', 1, 4, 2, "NNN...NN") \
  X('N', 2, 2, 4, ".N.NNNN.") \
  X('N', 3, 4, 2, "NN...NNN") \
  X('N', 4, 2, 4, "N.NN.N.N") \
  X('N', 5, 4, 2, ".NNNNN..") \
  X('N', 6, 2, 4, "N.N.NN.N") \
  X('N', 7, 4, 2, "..NNNNN.") \
  X('P', 0, 2, 3, "PPPPP.") \
  X('P', 1, 3, 2, "PPP.PP") \
  X('P', 2, 2, 3, ".PPPPP") \
  X('P', 3, 3, 2, "PP.PPP") \
  X('P', 4, 2, 3, "PPPP.P") \
  X('P', 5, 3, 2, ".PPPPP") \
  X('P', 6, 2, 3, "P.PPPP") \
  X('P', 7, 3, 2, "PPPPP.") \
  X('T', 0, 3, 3, "TTT.T..T.") \
  X('T', 1, 3, 3, "..TTTT..T") \
  X('T', 2, 3, 3, ".T..T.TTT") \
  X('T', 3, 3, 3, "T..TTTT..") \
  X('U', 0, 3, 2, "U.UUUU") \
  X('U', 1, 2, 3, "UUU.UU") \
  X('U', 2, 3, 2, "UUUU.U") \
  X('U', 3, 2, 3, "UU.UUU") \
  X('V', 0, 3, 3, "V..V..VVV") \
  X('V', 1, 3, 3, "VVVV..V..") \
  X('V', 2, 3, 3, "VVV..V..V") \
  X('V', 3, 3, 3, "..V..VVVV") \
  X('W', 0, 3, 3, "W..WW..WW") \
  X('W', 1, 3, 3, ".WWWW.W..") \
  X('W', 2, 3, 3, "WW..WW..W") \
  X('W', 3, 3, 3, "..W.WWWW.") \
  X('X', 0, 3, 3, ".X.XXX.X.") \
  X('Y', 0, 2, 4, ".YYY.Y.Y") \
  X('Y', 1, 4, 2, "..Y.YYYY") \
  X('Y', 2, 2, 4, "Y.Y.YYY.") \
  X('Y', 3, 4, 2, "YYYY.Y..") \
  X('Y', 4, 2, 4, "Y.YYY.Y.") \
  X('Y', 5, 4, 2, "YYYY..Y.") \
  X('Y', 6, 2, 4, ".Y.YYY.Y") \
  X('Y', 7, 4, 2, ".Y..YYYY") \
  X('Z', 0, 3, 3, "ZZ..Z..ZZ") \
  X('Z', 1, 3, 3, "..ZZZZZ..") \
  X('Z', 2, 3, 3, ".ZZ.Z.ZZ.") \
  X('Z', 3, 3, 3, "Z..ZZZ..Z")

char charAtHelper(const Placement *p, int x, int y, int WIDTH, int HEIGHT, const char *REP) {
  if (!(p->x <= x && x < p->x + WIDTH)) return '.';
  if (!(p->y <= y && y < p->y + HEIGHT)) return '.';
  int i = (y - p->y) * WIDTH + (x - p->x);
  return REP[i];
}
char digitAtHelper(const Placement *p, int x, int y, int WIDTH, int HEIGHT, const char *REP) {
  assert(p->x <= x && x < p->x + WIDTH);
  assert(p->y <= y && y < p->y + HEIGHT);
  int i = (y - p->y) * WIDTH + (x - p->x);
  assert(REP[i] != '.');
  int count = 0;
  for (int j=0; j < i; ++j) {
    if (REP[j] != '.') ++count;
  }
  assert(0 <= count && count < 5);
  return p->digits[count];
}

char charAt(const Placement *p, int x, int y) {
#define X(P, O, W, H, R) if (p->name == P && p->orientation == O) return charAtHelper(p, x, y, W, H, R);
PLACEMENTS
#undef X
  assert(0);
  return '.';
}
char digitAt(const Placement *p, int x, int y) {
#define X(P, O, W, H, R) if (p->name == P && p->orientation == O) return digitAtHelper(p, x, y, W, H, R);
PLACEMENTS
#undef X
  assert(0);
  return '.';
}

void maybe_add_row(struct dance_matrix *mat, Placement *p) {
  size_t constraint[5];
  int idx = 0;
  char alphabet[] = "FILNPTUVWXYZ";
  constraint[idx++] = 60 + (strchr(alphabet, p->name) - alphabet);
  for (int i=0; i < 60; ++i) {
    if (charAt(p, i%10, i/10) != '.') {
      constraint[idx++] = i;
    }
  }
  if (idx == 6) {
    // The piece is entirely within-bounds.
    (void)dance_addrow(mat, idx, constraint);
  }
}

void solve()
{
  struct dance_matrix mat;
  /* Set up the info for our callback grid-printing function. */
  int rc = dance_init(&mat, 0, 72, NULL);
  assert(rc == 0);

  for (int x=0; x < 10; ++x) {
    for (int y=0; y < 10; ++y) {
#define X(P, O, W, H, R) do { \
        Placement p = {x, y, P, O, {1,2,3,4,5}}; \
        /* Avoid rotations and reflections! Force 'F' to be one of the first four rotations, */ \
        /* and force its middle to be in [1,5) instead of [5,9). */ \
        if (P == 'F' && !(0 <= x && x < 4 && 0 <= O && O < 4)) { \
          /* do nothing */ \
        } else { \
          maybe_add_row(&mat, &p); \
        } \
      } while (0);
      PLACEMENTS
#undef X
    }
  }
  printf("The completed matrix has %ld columns and %ld rows.\n",
        (long)mat.ncolumns, (long)mat.nrows);
  printf("Solving...\n");

  int ns = dance_solve(&mat, print_pento_result, NULL);
  printf("%d solutions found.\n", ns);
  dance_free(&mat);
}

int print_pento_result(size_t n, struct data_object **sol, void *vinfo)
{
    char grid[60];

    for (size_t k=0; k < n; ++k) {
        struct data_object *o = sol[k];
        /* o is an arbitrary 1 entry in the kth row of the solution.
         * Scan to the right in the circular linked list that is o->right,
         * looking for a 1 in an "Across or Down" column-pair; that will
         * tell us whether this row represents an Across word, a Down word,
         * or a set of black squares. We really only care about the
         * Across words, since the letters Down are by definition the
         * same as the letters Across. */
        char piece = '?';
        do {
            int colx = atoi(o->column->name);
            if (colx >= 60) {
                piece = "FILNPTUVWXYZ"[colx - 60];
            }
            o = o->right;
        } while (o != sol[k]);
        assert(piece != '?');
        /* Extract its letters. */
        o = sol[k];
        do {
            int colx = atoi(o->column->name);
            if (0 <= colx && colx < 60) {
                grid[colx] = piece;
            }
            o = o->right;
        } while (o != sol[k]);
    }
    for (int y=0; y < 6; ++y) {
      for (int x=0; x < 10; ++x) {
        printf("%c", grid[y*10+x]);
      }
    }
    printf("\n");
    return 1;  // keep going
}

void do_error(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    putchar('\n');
    va_end(ap);
    exit(EXIT_FAILURE);
}
