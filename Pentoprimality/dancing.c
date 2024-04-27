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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dancing.h"

/*
   This flag controls the compiling-in of debugging routines.
   They're not useful for debugging client programs; they're only
   supplied for the convenience of people who want to hack the
   "dancing links" code itself.
*/
#define DANCING_DEBUG 0

/* The default size of a new matrix, if none is given. */
#define DEFAULT_INITIAL_SIZE 1000

/*
   Static function prototypes.
*/
static struct data_object *new_data_object(struct dance_matrix *m);
 static void dancing_adjust_pointers(struct dance_matrix *m,
     struct data_object *newdata);
static int dancing_search(size_t k, struct dance_matrix *m,
    int (*f)(size_t, struct data_object **, void *), void *info,
    struct data_object **solution);
static int dancing_search_dumb(size_t k, struct dance_matrix *m,
    int (*f)(size_t, struct data_object **, void *), void *info,
    struct data_object **solution);
 static void dancing_cover(struct column_object *c);
 static void dancing_uncover(struct column_object *c);

static int h_sort_sizet(const void *p, const void *q);

#if DANCING_DEBUG
static char *objnam(const struct data_object *o,
    const struct dance_matrix *m);
#endif



int dance_init(struct dance_matrix *m,
        size_t rows, size_t cols, const int *data)
{
    return dance_init_cap(m, rows, cols, data, DEFAULT_INITIAL_SIZE);
}

int dance_init_named(struct dance_matrix *m,
        size_t rows, size_t cols, const int *data, char * const  *names)
{
    return dance_init_named_cap(m, rows, cols, data, names,
        DEFAULT_INITIAL_SIZE);
}

int dance_init_cap(struct dance_matrix *m,
        size_t rows, size_t cols, const int *data, size_t initial_size)
{
    char **names = malloc(cols * sizeof *names);
    size_t i;
    int rc;

    if (names == NULL) return -3;

    if (cols < 26) {
        const char *alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        for (i=0; i < cols; ++i) {
            if ((names[i] = malloc(2)) == NULL) {
                while (i--) free(names[i]);
                free(names);
                return -3;
            }
            sprintf(names[i], "%c", alpha[i]);
        }
    }
    else {
        for (i=0; i < cols; ++i) {
            size_t len = (i < 10)? 2: (i < 100)? 3: (i < 1000)? 4: 20;
            if ((names[i] = malloc(len)) == NULL) {
                while (i--) free(names[i]);
                free(names);
                return -3;
            }
            sprintf(names[i], "%lu", (long unsigned)i);
        }
    }

    rc = dance_init_named_cap(m, rows, cols, data, names, initial_size);
    for (i=0; i < cols; ++i)
      free(names[i]);
    free(names);
    return rc;
}

int dance_init_named_cap(struct dance_matrix *m, size_t rows, size_t cols,
    const int *data, char * const *names, size_t initial_size)
{
    size_t i, j;

    m->all_data = (initial_size == 0)? NULL:
        malloc(initial_size * sizeof *m->all_data);
    m->all_data_cap = (m->all_data == NULL)? 0: initial_size;
    m->all_data_len = 0;

    m->nrows = rows;
    m->ncolumns = cols;
    m->columns = malloc(m->ncolumns * sizeof *m->columns);
    if (m->columns == NULL)
      return -3;
    m->head.data.right = &m->columns[0].data;
    m->head.data.left = &m->columns[m->ncolumns-1].data;

    for (i=0; i < m->ncolumns; ++i) {
        m->columns[i].name = malloc(strlen(names[i])+1);
        if (m->columns[i].name == NULL) {
            while (i--) free(m->columns[i].name);
            free(m->columns);
            return -3;
        }
        strcpy(m->columns[i].name, names[i]);
        m->columns[i].size = 0;
        m->columns[i].data.up = &m->columns[i].data;
        m->columns[i].data.down = &m->columns[i].data;
        if (i > 0)
          m->columns[i].data.left = &m->columns[i-1].data;
        else m->columns[i].data.left = &m->head.data;
        if (i < cols-1)
          m->columns[i].data.right = &m->columns[i+1].data;
        else m->columns[i].data.right = &m->head.data;

        for (j=0; j < rows; ++j) {
            if (data[j*cols+i] != 0) {
                /*
                   Found one! To insert it in the mesh at the
                   proper place, look to our left, and then
                   look up that column.
                */
                int ccol;
                struct data_object *new = new_data_object(m);
                if (new == NULL) {
                    dance_free(m);
                    return -3;
                }
                new->down = &m->columns[i].data;
                new->up = new->down->up;
                new->down->up = new;
                new->up->down = new;
                new->left = new;
                new->right = new;
                new->column = &m->columns[i];

                for (ccol=i-1; ccol >= 0; --ccol) {
                    if (data[j*cols+ccol] != 0) break;
                }
                if (ccol >= 0) {
                    int crow, cnt=0;
                    struct data_object *left;
                    for (crow=j; crow >= 0; --crow)
                      if (data[crow*cols+ccol] != 0) ++cnt;
                    left = &m->columns[ccol].data;
                    for ( ; cnt > 0; --cnt)
                      left = left->down;
                    new->left = left;
                    new->right = left->right;
                    new->left->right = new;
                    new->right->left = new;
                }

                /* Done inserting this "1" into the mesh. */
                m->columns[i].size += 1;
            }
        }
    }

    return 0;
}


/*
   Each matrix manages its own memory: the field |all_data| stores a
   pointer to an array of |data_object| structures, and each object
   in the matrix is drawn from this array. This approach makes adding
   new objects more complicated, but it saves huge amounts of time
   in |dance_free|, which otherwise would have to rely on the library's
   implementation of |free| to deallocate thousands --- sometimes
   millions --- of memory chunks. This way, we're only deallocating
   one chunk for the entire array, so memory doesn't get as fragmented
   and the user doesn't experience a slowdown.

   The most unfortunate side effect of this approach is that sometimes
   |new_data_object| has to call |malloc| to make the |all_data|
   array bigger. When this happens, all the existing pointers get
   invalidated, and we have to traverse the entire structure to patch
   it up. This is why the client is allowed to give |dance_init| an
   "initial size estimate" parameter --- to avoid the first few
   time-consuming resize operations.
*/
static struct data_object *new_data_object(struct dance_matrix *m)
{
    if (m->all_data_len >= m->all_data_cap) {
        struct data_object *data;
        const size_t bcap = m->all_data_cap * (sizeof *data);
        size_t newcap;
        size_t bnewcap = (m->all_data_cap == 0)?
            bcap + (2*m->ncolumns)*(sizeof *data): (bcap < ((size_t)-1)/2)?
            bcap + bcap/2:
            bcap + 200*(sizeof *data);
        if (bnewcap < bcap) bnewcap = (size_t)-1;
        newcap = bnewcap / sizeof *data;
        if (newcap <= m->all_data_cap) {
            return NULL;
        }
        if ((data = malloc(newcap * sizeof *data)) == NULL)
          return NULL;
        dancing_adjust_pointers(m, data); fflush(stdout);
        memcpy(data, m->all_data, m->all_data_len * sizeof *data);
        free(m->all_data);
        m->all_data = data;
        m->all_data_cap = newcap;
    }
    return &m->all_data[m->all_data_len++];
}


static void dancing_adjust_pointers(struct dance_matrix *m,
    struct data_object *newdata)
{
#define T(p) p = newdata+(p - m->all_data)
    struct data_object *c, *o, *next;
    for (c=m->head.data.right; c != &m->head.data; c = c->right) {
        if (c->down == c) continue;
        next = c->down;
        T(c->down);
        T(c->up);
        for (o = next; o != c; o = next) {
            next = o->down;
            T(o->left);
            T(o->right);
            if (o->up != c) T(o->up);
            if (o->down != c) T(o->down);
        }
    }
#undef T
}


int dance_addrow(struct dance_matrix *m, size_t nentries,
    const size_t *entries)
{
    size_t i;

    /*
       Note that |new_data_object| might invalidate any pointers
       to data objects; therefore we have to be somewhat indirect
       in referring to objects such as
       |m->columns[entries[i]].data.up|.  Be very careful when
       changing this section of code --- or any section that
       calls |new_data_object|!
    */
    for (i=0; i < nentries; ++i) {
        struct data_object *new = new_data_object(m);
        if (new == NULL) {
            m->all_data_len -= i;
            while (i--) {
                new = m->columns[entries[i]].data.up;
                new->up->down = new->down;
                new->down->up = new->up;
            }
            return -3;
        }
        new->column = &m->columns[entries[i]];
        new->down = &m->columns[entries[i]].data;
        new->up = new->down->up;
        new->down->up = new;
        new->up->down = new;
        if (i > 0) {
            new->left = m->columns[entries[i-1]].data.up;
            new->right = new->left->right;
            new->left->right = new;
            new->right->left = new;
        }
        else {
            new->left = new->right = new;
        }
        new->column->size += 1;
    }

    m->nrows += 1;
    return nentries;
}


int dance_addrow_named(struct dance_matrix *m, size_t nentries,
    char * const *names)
{
    size_t *entries = malloc(nentries * sizeof *entries);
    size_t i, j;
    int rc;

    if (entries == NULL)
      return -3;

    for (i=0; i < nentries; ++i) {
        for (j=0; j < m->ncolumns; ++j) {
            if (strcmp(m->columns[j].name, names[i]) == 0)
              break;
        }
        if (j == m->ncolumns) return -1;
        entries[i] = j;
    }
    rc = dance_addrow(m, nentries, entries);
    free(entries);
    return rc;
}


int dance_deleterow(struct dance_matrix *m, size_t nentries,
    const size_t *entries)
{
    size_t ridx;
    struct data_object *firstc, *h, *o;
    size_t *useres;
    size_t *reales;

    if (nentries == 0)
      return 0;

    useres = malloc(nentries * sizeof *useres);
    reales = malloc(nentries * sizeof *reales);
    if (useres == NULL || reales == NULL) {
        free(useres);
        free(reales);
        return -3;
    }
    memcpy(useres, entries, nentries * sizeof *useres);
    qsort(useres, nentries, sizeof *useres, h_sort_sizet);

    firstc = &m->columns[entries[0]].data;
    for (h = firstc->down; h != firstc; h = h->down) {
        ridx = 0;
        reales[ridx++] = entries[0];
        for (o = h->right; o != h; o = o->right) {
            reales[ridx++] = (o->column - m->columns);
            if (ridx == nentries) {
                if (o->right != h) break;
                qsort(reales, nentries, sizeof *reales, h_sort_sizet);
                if (0 != memcmp(useres, reales, nentries * sizeof *reales))
                  break;
                /* We've found the row. */
                goto row_found;
            }
        }
    }
    /* No row was found matching the user's query. */
    free(useres);
    free(reales);
    return -1;

  row_found:
    free(useres);
    free(reales);

    o = h;
    do {
        o->left->right = o->right;
        o->right->left = o->left;
        o->up->down = o->down;
        o->down->up = o->up;
        o->column->size -= 1;
        o = o->right;
    } while (o != h);
    m->nrows -= 1;
    return 0;
}


int dance_deleterow_named(struct dance_matrix *m, size_t nentries,
    char * const *names)
{
    size_t *entries = malloc(nentries * sizeof *entries);
    size_t i, j;
    int rc;

    if (entries == NULL)
      return -3;

    for (i=0; i < nentries; ++i) {
        for (j=0; j < m->ncolumns; ++j) {
            if (strcmp(m->columns[j].name, names[i]) == 0)
              break;
        }
        if (j == m->ncolumns) return -1;
        entries[i] = j;
    }
    rc = dance_deleterow(m, nentries, entries);
    free(entries);
    return rc;
}


int dance_free(struct dance_matrix *m)
{
    size_t i;
    for (i=0; i < m->ncolumns; ++i)
      free(m->columns[i].name);
    free(m->columns);
    free(m->all_data);
    return 0;
}


int dance_solve(struct dance_matrix *m,
    int (*f)(size_t, struct data_object **, void *), void *info)
{
    struct data_object **solution;
    int ns;

    if ((solution = malloc(m->ncolumns * sizeof *solution)) == NULL)
      return -3;
    ns = dancing_search(0, m, f, info, solution);
    free(solution);
    return ns;
}

int dance_solve_dumb(struct dance_matrix *m,
    int (*f)(size_t, struct data_object **, void *), void *info)
{
    struct data_object **solution;
    int ns;

    if ((solution = malloc(m->ncolumns * sizeof *solution)) == NULL)
      return -3;
    ns = dancing_search_dumb(0, m, f, info, solution);
    free(solution);
    return ns;
}


int dancing_search(size_t k, struct dance_matrix *m,
    int (*f)(size_t, struct data_object **, void *), void *info,
    struct data_object **solution)
{
    struct column_object *c = NULL;
    struct data_object *r, *j;
    int count = 0;
    int rc;

    if (m->head.data.right == &m->head.data) {
        return f(k, solution, info);
    }

    /* Choose a column object |c|. This is the "not-dumb" part. */
    do {
        size_t minsize = m->nrows+1;
        for (j = m->head.data.right; j != &m->head.data; j = j->right) {
            struct column_object *jj = (struct column_object *)j;
            if (jj->size < minsize) {
                c = jj;
                minsize = jj->size;
                if (minsize <= 1) break;
            }
        }
        if (minsize == 0) {
            /* If the most constrained column is unsatisfiable, don't even
             * bother to cover it. Just backtrack. */
            return count;
        }
    } while (0);

    /* Cover column |c|. */
    dancing_cover(c);

    for (r = c->data.down; r != &c->data; r = r->down) {
        solution[k] = r;
        for (j = r->right; j != r; j = j->right) {
            dancing_cover(j->column);
        }
        rc = dancing_search(k+1, m, f, info, solution);
        if (rc < 0)
          return rc;
        else count += rc;
        r = solution[k];
        c = r->column;
        for (j = r->left; j != r; j = j->left) {
            dancing_uncover(j->column);
        }
    }

    /* Uncover column |c| and backtrack. */
    dancing_uncover(c);
    return count;
}

int dancing_search_dumb(size_t k, struct dance_matrix *m,
    int (*f)(size_t, struct data_object **, void *), void *info,
    struct data_object **solution)
{
    struct column_object *c = NULL;
    struct data_object *r, *j;
    int count = 0;
    int rc;

    if (m->head.data.right == &m->head.data) {
        return f(k, solution, info);
    }

    /* Choose a column object |c|. This is the "dumb" part. */
    c = (struct column_object *)m->head.data.right;

    /* Cover column |c|. */
    dancing_cover(c);

    for (r = c->data.down; r != &c->data; r = r->down) {
        solution[k] = r;
        for (j = r->right; j != r; j = j->right) {
            dancing_cover(j->column);
        }
        rc = dancing_search_dumb(k+1, m, f, info, solution);
        if (rc < 0)
          return rc;
        else count += rc;
        r = solution[k];
        c = r->column;
        for (j = r->left; j != r; j = j->left) {
            dancing_uncover(j->column);
        }
    }

    /* Uncover column |c| and return. */
    dancing_uncover(c);
    return count;
}


static void dancing_cover(struct column_object *c)
{
    struct data_object *i, *j;

    c->data.right->left = c->data.left;
    c->data.left->right = c->data.right;
    for (i = c->data.down; i != &c->data; i = i->down) {
        for (j = i->right; j != i; j = j->right) {
            j->down->up = j->up;
            j->up->down = j->down;
            j->column->size -= 1;
        }
    }
}


static void dancing_uncover(struct column_object *c)
{
    struct data_object *i, *j;

    for (i = c->data.up; i != &c->data; i = i->up) {
        for (j = i->left; j != i; j = j->left) {
            j->column->size += 1;
            j->down->up = j;
            j->up->down = j;
        }
    }
    c->data.left->right = &c->data;
    c->data.right->left = &c->data;
}


int dance_sample_callback(size_t n, struct data_object **sol, void *vfp)
{
    size_t i;
    struct data_object *o;
    FILE *fp = vfp;
    if (fp == NULL) fp = stdout;

    for (i=0; i < n; ++i) {
        fprintf(fp, "Row %lu:", (long unsigned)i);
        fprintf(fp, " %s", sol[i]->column->name);
        for (o = sol[i]->right; o != sol[i]; o = o->right)
          fprintf(fp, " %s", o->column->name);
        fprintf(fp, "\n");
    }
    return 1;
}


static int h_sort_sizet(const void *p, const void *q)
{
    const size_t *a = p, *b = q;
    return (*a < *b)? -1: (*a > *b);
}



#if DANCING_DEBUG
static char *objnam(const struct data_object *o,
    const struct dance_matrix *m)
{
    static char buf[100];
    size_t i;

    if (o == NULL)
      return "NULL";
    if (o == &m->head.data)
      return "&m->head.data";

    for (i=0; i < m->ncolumns; ++i) {
        if (o == &m->columns[i].data) {
            sprintf(buf, "&columns[%lu].data", (long unsigned)i);
            return buf;
        }
    }

    if ((o - m->all_data) < 0) {
        sprintf(buf, "unknown < m->all_data: %p", (const void *)o);
        return buf;
    }
    else if ((signed long)(o - m->all_data) > (signed long)m->all_data_len) {
        sprintf(buf, "unknown > m->all_data: %p", (const void *)o);
        return buf;
    }
    else {
        sprintf(buf, "&m->all_data[%d]", (int)(o - m->all_data));
        return buf;
    }
}
#endif

