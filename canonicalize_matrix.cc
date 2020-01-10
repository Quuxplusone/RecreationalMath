#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <nauty.h>

// Set this either way; the result should be identical.
#define ROWSFIRST 0

std::vector<std::string> canonicalize_with_nauty(std::vector<std::string> lines)
{
    const int rows = lines.size();
    const int cols = lines[0].size();
    for (auto&& line : lines) assert(line.size() == cols);

    const int n = rows + cols;
    const int m = SETWORDSNEEDED(n);
    nauty_check(WORDSIZE, m, n, NAUTYVERSIONID);

#if ROWSFIRST
    auto v_of_row = [&](int i) { assert(0 <= i && i < rows); return i; };
    auto v_of_col = [&](int j) { assert(0 <= j && j < cols); return rows + j; };
    auto row_of_v = [&](int vi) { assert(0 <= vi && vi < rows); return vi; };
    auto col_of_v = [&](int vj) { assert(rows <= vj && vj < n); return vj - rows; };
#else
    auto v_of_row = [&](int i) { assert(0 <= i && i < rows); return cols + i; };
    auto v_of_col = [&](int j) { assert(0 <= j && j < cols); return j; };
    auto row_of_v = [&](int vi) { assert(cols <= vi && vi < n); return vi - cols; };
    auto col_of_v = [&](int vj) { assert(0 <= vj && vj < cols); return vj; };
#endif

    DYNALLSTAT(graph, g, g_sz); DYNALLOC2(graph, g, g_sz, m, n, "malloc");
    EMPTYGRAPH(g, m, n);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (lines[i][j] == '1') {
                ADDONEEDGE(g, v_of_row(i), v_of_col(j), m);
            }
        }
    }

    // Add a labeling/coloring to distinguish the "rows" vertices from the "cols" vertices.
    // Nauty produces different canonicalizations for K_{red,blue} versus K_{blue,red},
    // so in our labeling we ALWAYS color the "rows" vertices red and the "cols" vertices blue,
    // never vice versa.
    DYNALLSTAT(int, lab, lab_sz); DYNALLOC1(int, lab, lab_sz, n, "malloc");
    DYNALLSTAT(int, ptn, ptn_sz); DYNALLOC1(int, ptn, ptn_sz, n, "malloc");

    for (int i=0; i < n; ++i) ptn[i] = 1;
    ptn[rows-1] = 0;  // "rows" red vertices
    ptn[n-1] = 0;  // followed by "cols" blue vertices

    for (int i=0; i < rows; ++i) { lab[i] = v_of_row(i); }
    for (int j=0; j < cols; ++j) { lab[rows + j] = v_of_col(j); }

    // Nauty's "options.getcanon" is a red herring. We don't want a brand-new graph;
    // what we want is a canonical labeling of our existing graph's vertices.
    DEFAULTOPTIONS_GRAPH(options);
    options.defaultptn = false;

    DYNALLSTAT(int, orbits, orbits_sz); DYNALLOC1(int, orbits, orbits_sz, n, "malloc");
    statsblk stats;
    densenauty(g, lab, ptn, orbits, &options, &stats, m, n, nullptr);
    assert(stats.errstatus == 0);

    // Convert the canonicalized graph back to a txn matrix.
    // We need to look at the labeling, which is now a permutation
    // of our original vertices.
    assert(ptn[rows-1] == 0);
    assert(ptn[n-1] == 0);

    for (int i=0; i < rows; ++i) {
        for (int j=0; j < cols; ++j) {
            int vi = lab[i];
            int vj = lab[rows + j];
            bool a = ISELEMENT(&g[vi*m], vj);
            bool b = ISELEMENT(&g[vj*m], vi);
            assert(a == b);
            lines[i][j] = (a ? '1' : '.');
        }
    }

    return lines;
}

std::vector<std::string> shuffle_for_prettiness(std::vector<std::string> lines)
{
    const int rows = lines.size();
    const int cols = lines[0].size();

    auto is_one = [&](char ch) { return ch == '1'; };
    auto trailing_ones = [&](const auto& a) {
        auto it1 = std::find_if(a.rbegin(), a.rend(), is_one);
        auto it2 = std::find_if_not(it1, a.rend(), is_one);
        return it2 - it1;
    };
    auto by_trailing_ones = [&](const auto& a, const auto& b) { return trailing_ones(a) > trailing_ones(b); };
    auto swap_columns = [&](int i, int j) {
        for (auto& line : lines) {
            std::swap(line[i], line[j]);
        }
    };

    for (int iter = 0; iter < 10; ++iter) {
        std::stable_sort(lines.begin(), lines.end(), std::greater<>());
        std::stable_sort(lines.begin(), lines.end(), by_trailing_ones);
        // The first column of a pair gets priority. Bubble-sort.
        for (int iter = 0; iter < cols; ++iter) {
            for (int c = cols-1; c >= 0; --c) {
                for (int r=0; r < rows; ++r) {
                    if (lines[r][c] != lines[r][c+1]) {
                        if (is_one(lines[r][c+1])) {
                            swap_columns(c, c+1);
                        }
                        break;
                    }
                }
            }
        }
    }
    return lines;
}

int main()
{
    std::vector<std::string> lines(
        std::istream_iterator<std::string>{std::cin},
        std::istream_iterator<std::string>{}
    );

    // This is the foolproof canonicalization step.
    // Every matrix in an equivalence class WILL be mapped
    // onto the same (arbitrary) member of its equivalence class.
    lines = canonicalize_with_nauty(lines);

    // This is the "pretty-print" step.
    // It takes the arbitrary canonical representation
    // and quickly shuffles rows and columns deterministically
    // to produce a representation that is qualitatively "prettier."
    lines = shuffle_for_prettiness(lines);

    // And finally, print out the result.
    for (auto&& line : lines) {
        std::cout << line << "\n";
    }
}
