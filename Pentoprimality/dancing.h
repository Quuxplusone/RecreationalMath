/*
   This library implements Donald Knuth's "Dancing Links" algorithm
   for solving "exact cover" problems on (0,1)-matrices. The routines
   may be successfully applied to polyomino tilings, the solution of
   Sudoku puzzles, and even the filling of crossword grids.

   For Knuth's original paper, see
   http://xxx.lanl.gov/PS_cache/cs/pdf/0011/0011047.pdf

   For information on Dancing Links in general, see
   http://en.wikipedia.org/wiki/Dancing_Links

   This program is copyright Arthur O'Dwyer, December 2005.
   Free for all non-commercial use.
*/

#ifndef H_DANCING
 #define H_DANCING

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct data_object {
    struct data_object *up, *down, *left, *right;
    struct column_object *column;
};

struct column_object {
    struct data_object data; /* must be the first field */
    size_t size;
    char *name;
};

struct dance_matrix {
    size_t nrows, ncolumns;
    struct column_object *columns;
    struct column_object head;
    struct data_object *all_data;
    size_t all_data_len, all_data_cap;
};

/*
   Initialize a matrix with |cols| columns and |rows| rows. If |rows|
   is non-zero, |data| must point to a matrix of |rows|-times-|cols|
   integers, each being either 0 or 1. If |names| are provided, then
   there must be exactly |cols| of them. If |names| are not provided,
   then the columns will be named with decimal integers, starting with
   |"0"| and proceeding upward.

   The client may also provide an estimate |est| of the number of
   1 entries in the matrix, which informs the matrix's initial memory
   usage. If you know exactly how many 1 entries your matrix has,
   give that number; a little too high is better than a little too
   low. If you don't have any estimate, the default behavior will
   probably do just fine.
*/
int dance_init(struct dance_matrix *m,
        size_t rows, size_t cols, const int *data);
int dance_init_named(struct dance_matrix *m,
        size_t rows, size_t cols, const int *data, char * const *names);
int dance_init_cap(struct dance_matrix *m,
        size_t rows, size_t cols, const int *data, size_t est);
int dance_init_named_cap(struct dance_matrix *m, size_t rows, size_t cols,
        const int *data, char * const *names, size_t est);

/*
   Add a new row to the matrix. The columns may be provided in terms
   of their indices, or in terms of their names; the |named| routine
   is defined in terms of the other.

   Rows cannot efficiently be deleted from a matrix, but an inefficient
   routine is provided anyway.
*/
int dance_addrow(struct dance_matrix *m,
        size_t nentries, const size_t *entries);
int dance_addrow_named(struct dance_matrix *m,
        size_t nentries, char * const *names);

int dance_deleterow(struct dance_matrix *m,
        size_t nentries, const size_t *entries);
int dance_deleterow_named(struct dance_matrix *m,
        size_t nentries, char * const *names);


/*
   Destroy the given matrix, freeing all its memory. After this routine
   has been called, the contents of |m| are indeterminate, and you
   must call |dance_init| before using the matrix again.
*/
int dance_free(struct dance_matrix *m);


/*
   Find exact covers for the given matrix. The |dance_solve| routine
   comes in two varieties: "smart" and |dumb|. The "smart" routine
   scans the matrix each iteration to cover the most-restricted columns
   first; the "dumb" routine just takes the first open column each
   time. There is a reason the "smart" routine is the default.

   The callback function |f| is called for each solution found. It
   is provided with three parameters: |k|; |s|, an array of |k| pointers
   to data objects; and |info|, a user-supplied pointer to any data
   the function might need to do its job. The data objects |s[0]|
   through |s[k-1]| are members of the rows in the current solution;
   the other members of those rows can be found by looping around the
   cycles |s[i]|, |s[i]->right|, |s[i]->right->right|,... for every
   $0\le i < k$.

   |dance_solve| normally returns the accumulated sum of the return
   values from the callback |f|. However, if |f| ever returns a
   negative value, |dance_solve| will immediately terminate and
   return that value, ignoring the sum accumulated so far.
*/
int dance_solve(struct dance_matrix *m,
        int (*f)(size_t, struct data_object **, void *), void *info);
int dance_solve_dumb(struct dance_matrix *m,
        int (*f)(size_t, struct data_object **, void *), void *info);


/*
   To print all the solutions to the standard output in a bare-bones
   format, invoke |dance_solve(mat, dance_sample_callback, NULL)|.
   To print the same information to a file, invoke
   |dance_solve(mat, dance_sample_callback, fp)| where |fp| is a
   |FILE *| object.

   |dance_sample_callback| always returns 1.
*/
int dance_sample_callback(size_t, struct data_object **, void *);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
