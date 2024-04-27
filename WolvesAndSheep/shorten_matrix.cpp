#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

static int weightOf(const std::string& s)
{
    return std::count(s.begin(), s.end(), '1');
}

static bool reversedLess(const std::string& a, const std::string& b)
{
    if (weightOf(a) > weightOf(b)) return true;
    if (weightOf(a) < weightOf(b)) return false;
    return std::lexicographical_compare(a.rbegin(), a.rend(), b.rbegin(), b.rend());
}

int main() {
    std::vector<std::string> mat;
    std::copy(
        std::istream_iterator<std::string>(std::cin),
        std::istream_iterator<std::string>(),
        std::back_inserter(mat)
    );
    // Put the heaviest test first.
    std::sort(mat.begin(), mat.end(), [](auto&& a, auto&& b) { return weightOf(a) > weightOf(b); });

    // Introduce the animals in order.
    int r = 0;
    int c = 0;
    while (r != mat.size()) {
        // The first 'c' columns are fixed in place. Look for the next animal to be introduced.
        auto it = std::find(mat[r].begin() + c, mat[r].end(), '1');
        if (it != mat[r].end()) {
            int next_animal = (it - mat[r].begin());
            // Permute the columns so that this animal is in column 'c'.
            for (std::string& row : mat) {
                std::swap(row[c], row[next_animal]);
            }
            ++c;
        } else {
            // Done all the animals in this row. Go to the next row.
            ++r;
        }
    }

for (int it = 0; it < 10; ++it) {

    // For any two sheep who've been indistinguishable up to now,
    // don't put Sheep B into this group unless Sheep A is already there.
    for (int a = 0; a < mat[0].size(); ++a) {
        for (int b = a+1; b < mat[0].size(); ++b) {
            for (int r = 0; r < mat.size(); ++r) {
                if (mat[r][a] == mat[r][b]) {
                    // continue
                } else if (mat[r][a] == '1') {
                    break;  // this sheep-pair is OK
                } else if (mat[r][b] == '1') {
                    // This sheep-pair needs swapping.
                    for (std::string& row : mat) {
                        std::swap(row[a], row[b]);
                    }
                    break;
                } else {
                    assert(false);
                }
            }
        }
    }

    std::sort(mat.begin(), mat.end(), reversedLess);
}

    std::cout << "--------\n";
    for (const auto& row : mat) {
        std::cout << row << "\n";
    }
}
