
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "wolves.h"

int main(int argc, char **argv)
{
    if (argc == 4) {
        int n = atoi(argv[1]);
        int k = atoi(argv[2]);
        int t = atoi(argv[3]);
        NktResult result = solve_wolves(n, k, t);
        printf("%s\n", result.message.c_str());
    } else if (argc == 5) {
        int n = atoi(argv[1]);
        int k = atoi(argv[2]);
        int t = atoi(argv[3]);
        int s = atoi(argv[4]);
        NktResult result = solve_wolves(n, k, t, s);
        printf("%s\n", result.message.c_str());
    } else if (argc == 1 || argc == 2) {
        int n = (argc == 2) ? atoi(argv[1]) : 0;
        std::vector<int> triangle;
        switch (n - 1) {
            case -1: triangle = {}; break;
            case  0: triangle = {0}; break;
            case  1: triangle = {0,0}; break;
            case  2: triangle = {0,1,0}; break;
            case  3: triangle = {0,2,2,0}; break;
            case  4: triangle = {0,2,3,3,0}; break;
            case  5: triangle = {0,3,4,4,4,0}; break;
            case  6: triangle = {0,3,5,5,5,5,0}; break;
            case  7: triangle = {0,3,6,6,6,6,6,0}; break;
            case  8: triangle = {0,3,6,7,7,7,7,7,0}; break;
            case  9: triangle = {0,4,7,8,8,8,8,8,8,0}; break;
            case 10: triangle = {0,4,7,9,9,9,9,9,9,9,0}; break;
            case 11: triangle = {0,4,8,10,10,10,10,10,10,10,10,0}; break;
            case 12: triangle = {0,4,8,11,11,11,11,11,11,11,11,11,0}; break;

            default: {
                printf("Precomputed triangle rows are known only up to n=12.\n");
                exit(1);
            }
        }
        for ( ; true; ++n) {
            triangle.push_back(0);
            for (int k = 0; k <= n; ++k) {
                for (int t = triangle[k]; t <= n-1; ++t) {
                    NktResult result = solve_wolves(n, k, t);
                    printf("%s", result.message.c_str());
                    if (result.success) {
                        triangle[k] = t;
                        break;
                    }
                }
            }
            printf("SUCCESS: ");
            for (int elt : triangle) printf(" %2d", elt);
            printf("\n");
        }
    } else {
        printf("Usage:\n");
        printf("  ./st n k t   -- solve (n,k) in t tests\n");
        printf("  ./st n k t s -- ...each involving s animals\n");
        printf("  ./st         -- print the triangle of solutions t(n,k)\n");
        printf("  ./st r       -- ...having precomputed the first r rows\n");
    }
}
