
#include <assert.h>
#include <condition_variable>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>
#include "wolves.h"

struct Triangle {
    std::mutex mtx;
    std::condition_variable cv;
    std::vector<std::vector<int>> entries;

    explicit Triangle(int n) {
        entries = {
            {0},
            {0,0},
            {0,1,0},
            {0,2,2,0},
            {0,2,3,3,0},
            {0,3,4,4,4,0},
            {0,3,5,5,5,5,0},
            {0,3,6,6,6,6,6,0},
            {0,3,6,7,7,7,7,7,0},
            {0,4,7,8,8,8,8,8,8,0},
            {0,4,7,9,9,9,9,9,9,9,0},
            {0,4,-2,10,10,10,10,10,10,10,10,0},
            {0,4,-2,-2,11,11,11,11,11,11,-2,11,0},
            {0,4,-2,-2,-2,12,12,12,12,12,12,-2,12,0},
        };
        if (entries.size() > n) {
            entries.resize(n);
        }
    }

    std::tuple<int, int> get_work() {
        auto assign = [&](int n, int k) {
            entries[n][k] = -1;
            return std::make_tuple(n, k);
        };
        std::unique_lock<std::mutex> lk(mtx);
        for (int n=0; n < entries.size(); ++n) {
            assert(entries[n].size() == n+1);
            // Assign work from right to left within a row: larger k are easier.
            if (entries[n][1] == -2) {
                return assign(n, 1);
            }
            for (int k=n; k >= 0; --k) {
                if (entries[n][k] == -2) {
                    return assign(n, k);
                }
            }
        }
        // We didn't find any work not-yet-being-done. Start a fresh row.
        int n = entries.size();
        entries.push_back(std::vector<int>(n+1, -2));
        entries[n][0] = 0;
        entries[n][n-1] = n-1;
        entries[n][n] = 0;
        lk.unlock();
        return get_work();
    }

    int get_min_t(int n, int k) {
        if (n == 0 || k == 0 || k == n) return 0;
        if (k == n-1) return n-1;
        std::lock_guard<std::mutex> lk(mtx);
        while (n >= k) {
            --n;
            if (entries[n][k] >= 0) {
                return entries[n][k];
            }
        }
        assert(false);  // we should have hit a "0" entry by now
        return 0;
    }

    int get_max_t(int n, int k) {
        if (n == 0 || k == 0 || k == n) return 0;
        return n-1;
    }

    void report_result(int n, int k, int t) {
        std::lock_guard<std::mutex> lk(mtx);
        assert(0 <= n && n < entries.size());
        assert(0 <= k && k <= entries[n].size());
        assert(entries[n][k] == -1);
        entries[n][k] = t;
        cv.notify_all();
    }
};

static void worker_thread(Triangle& triangle)
{
    while (true) {
        std::tuple<int, int> nk = triangle.get_work();
        int n = std::get<0>(nk);
        int k = std::get<1>(nk);
        int min_t = triangle.get_min_t(n, k);
        int max_t = triangle.get_max_t(n, k);
        for (int t = min_t; t <= max_t; ++t) {
            NktResult result = solve_wolves(n, k, t);
            if (result.success) {
                triangle.report_result(n, k, t);
                break;
            }
        }
    }
}

static void printer_thread(Triangle& triangle)
{
    int count = 0;
    std::unique_lock<std::mutex> lk(triangle.mtx);
    while (true) {
        printf("UPDATE %d!------------------------------\n", count);
        for (int n = 0; n < triangle.entries.size(); ++n) {
            printf("    n=%-2d ", n);
            for (int k = 0; k < triangle.entries[n].size(); ++k) {
                if (triangle.entries[n][k] == -2) {
                    printf("  .");
                } else if (triangle.entries[n][k] == -1) {
                    printf("  x");
                } else {
                    printf("%3d", triangle.entries[n][k]);
                }
            }
            printf("\n");
        }
        ++count;
        triangle.cv.wait(lk);
    }
}

int main(int argc, char **argv)
{
    // Precompute n rows, to pick up where we left off.
    int n = argc == 2 ? atoi(argv[1]) : 0;
    Triangle triangle(n);
    std::thread printer([&]() {
        printer_thread(triangle);
    });
    std::vector<std::thread> workers;
    for (int i=0; i < NUM_THREADS; ++i) {
        workers.emplace_back([&]() {
            while (true) {
                worker_thread(triangle);
            }
        });
    }
    printer.join();  // block forever
}
