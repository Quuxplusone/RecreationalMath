#include "dancing.h"
#include <cstdio>
#include <cassert>
#include <cstddef>
#include <cstring>

extern int CountAhead;

#if 0
 #define N 8
 #define K 6
#else
 #define N 49
 #define K 35
#endif

char grid[K][K] = {};

void add_rotations(dance_matrix *mat, size_t idx, size_t *constraint) {
    dance_addrow(mat, idx, constraint);
    if (idx == 1+1 || idx == N+1) {
        // don't rotate
        return;
    }
    size_t rotated_constraint[N+2];
    assert(constraint[0] >= K*K);
    rotated_constraint[0] = constraint[0];
    // Rotate 90 degrees right.
    for (int i=1; i < idx; ++i) {
        int x = constraint[i] % K;
        int y = constraint[i] / K;
        assert(0 <= x && x < K);
        assert(0 <= y && y < K);
        rotated_constraint[i] = K*x+(K-1-y);
    }
    dance_addrow(mat, idx, rotated_constraint);
    if (idx == 1+1) {
        // don't rotate 2 or 3 times, because it's mirror-symmetrical
        return;
    }
    // Rotate 180 degrees.
    for (int i=1; i < idx; ++i) {
        int x = constraint[i] % K;
        int y = constraint[i] / K;
        rotated_constraint[i] = K*(K-1-y)+(K-1-x);
    }
    dance_addrow(mat, idx, rotated_constraint);
    // Rotate 90 degrees left.
    for (int i=1; i < idx; ++i) {
        int x = constraint[i] % K;
        int y = constraint[i] / K;
        rotated_constraint[i] = K*(K-1-x)+y;
    }
    dance_addrow(mat, idx, rotated_constraint);
}

void add_L_placements(dance_matrix *mat, int n) {
    size_t constraint[N+2];
    for (int i=0; i < K; ++i) {
        for (int j=0; j < K; ++j) {
            // top corner at (i,j) 
            if (j + (n+1)/2 > K) continue;
            if (i + (n+1)/2 > K) continue;
            memset(constraint, '\0', sizeof(constraint));
            int idx = 0;
            constraint[idx++] = K*K + (n-1);  // uses the N piece
            constraint[idx++] = ((j+0)*K)+(i+0);
            for (int a = 1; a < (n+1)/2; ++a) {
                assert(i+a < K);
                assert(j+a < K);
                constraint[idx++] = ((j+0)*K)+(i+a);
                constraint[idx++] = ((j+a)*K)+(i+0);
            }
            if (n % 2 != 0) {
                assert(idx == n+1);
                add_rotations(mat, n+1, constraint);
            }
            if (n % 2 == 0 && i + n/2 < K) {
                // put the long leg vertically
                assert(idx == n);
                constraint[n] = ((j+0)*K)+(i+(n/2));
                add_rotations(mat, n+1, constraint);
            }
            if (n % 2 == 0 && j + n/2 < K) {
                // or put the long leg horizontally
                assert(idx == n);
                constraint[n] = ((j+(n/2))*K)+(i+0);
                add_rotations(mat, n+1, constraint);
            }
        }
    }
}

void add_W_placements(dance_matrix *mat, int n) {
    size_t constraint[N+2];
    for (int i=0; i < K; ++i) {
        for (int j=0; j < K; ++j) {
            // top corner at (i,j) 
            if (j + (n+1)/2 > K) continue;
            if (i + (n+1)/2 > K) continue;
            memset(constraint, '\0', sizeof(constraint));
            int idx = 0;
            constraint[idx++] = K*K + (n-1);  // uses the N piece
            for (int a = 0; a < (n+1)/2; ++a) {
                assert(i+a < K);
                assert(j+(n-1)/2-a < K);
                constraint[idx++] = ((j+(n-1)/2-a)*K)+(i+a);
            }
            for (int a = 1; a < (n+1)/2; ++a) {
                assert(j+(n+1)/2-a < K);
                constraint[idx++] = ((j+(n+1)/2-a)*K)+(i+a);
            }
            if (n % 2 != 0) {
                assert(idx == n+1);
                add_rotations(mat, n+1, constraint);
            }
            if (n % 2 == 0 && i + n/2 < K) {
                // put the long leg vertically
                assert(idx == n);
                constraint[n] = ((j+0)*K)+(i+(n/2));
                add_rotations(mat, n+1, constraint);
            }
            if (n % 2 == 0 && j + n/2 < K) {
                // or put the long leg horizontally
                assert(idx == n);
                constraint[n] = ((j+(n/2))*K)+(i+0);
                add_rotations(mat, n+1, constraint);
            }
        }
    }
}

void color_cell(int p, char color)
{
    if (p < K*K) {
        assert(grid[p/K][p%K] == '.');
        grid[p/K][p%K] = color;
    }
}

void color_piece_for_row(const data_object *origs)
{
    const data_object *s = origs;
    while (atoi(s->column->name) < K*K) {
        s = s->right;
    }
    char color = 'A' + (atoi(s->column->name) - K*K);
    origs = s;
    s = s->right;
    while (s != origs) {
        color_cell(atoi(s->column->name), color);
        s = s->right;
    }
}

void print_grid() {
    puts("--------------------------------");
    for (int j=0; j < K; ++j) {
        for (int i=0; i < K; ++i) {
            putchar(grid[j][i]);
        }
        putchar('\n');
    }
}

int main(int argc, char **argv)
{
    if (argc >= 2) {
        CountAhead = atoi(argv[1]);
    }

    struct dance_matrix mat;
    int rc = dance_init(&mat, 0, K*K + N, NULL);
    assert(rc == 0);
    for (int n=1; n <= N; ++n) {
        add_W_placements(&mat, N+1 - n);
    }
    printf("Matrix has %zu rows, %zu columns\n", mat.nrows, mat.ncolumns);
    printf("Sample rows:\n");
    for (int i=1; i < 10; ++i) {
        const column_object *col = &mat.columns[rand() % mat.ncolumns];
        const data_object *dat = col->data.down;
        for (int rowidx = rand() % col->size; rowidx != 0; --rowidx) {
            dat = dat->down;
        }
        memset(grid, '.', sizeof grid);
        color_piece_for_row(dat);
        print_grid();
    }
    dance_solve(&mat, [](size_t k, struct data_object **s, void *info) {
        (void)info;
        memset(grid, '.', sizeof grid);
        for (int i=0; i < k; ++i) {
            color_piece_for_row(s[i]);
        }
        print_grid();
        return -1;
    }, nullptr);
    dance_free(&mat);
}

