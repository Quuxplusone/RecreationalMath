
#include <assert.h>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>
#include "eytzinger_utils.h"
#include "wolves.h"

struct WorkItem {
    std::atomic<bool> *stop_working = nullptr;
    int min_t = 0;
    int max_t = INT_MAX;

    void pre_solve(int t) {
        min_t = t;
        max_t = t;
    }

    bool is_unstarted() const {
        return (min_t != max_t) && (stop_working == nullptr);
    }

    bool is_in_progress() const {
        return stop_working != nullptr;
    }

    bool has_answer() const {
        return (min_t == max_t);
    }

    void interrupt_worker() {
        if (stop_working != nullptr) {
            stop_working->store(true);
        }
    }
};

struct Triangle {
    std::mutex mtx;
    std::condition_variable cv;
    std::vector<std::vector<WorkItem>> entries;

    explicit Triangle(int n) {
        static constexpr int X = -1;
        std::vector<std::vector<int>> known_entries = {
            {0},
            {0, 0},
            {0, 1, 0},
            {0, 2, 2, 0},
            {0, 2, 3, 3, 0},
            {0, 3, 4, 4, 4, 0},
            {0, 3, 5, 5, 5, 5, 0},
            {0, 3, 6, 6, 6, 6, 6, 0},
            {0, 3, 6, 7, 7, 7, 7, 7, 0},
            {0, 4, 7, 8, 8, 8, 8, 8, 8, 0},
            {0, 4, 7, 9, 9, 9, 9, 9, 9, 9, 0},
            {0, 4, 8,10,10,10,10,10,10,10,10, 0},
            {0, 4, X, X,11,11,11,11,11,11,11,11, 0},
            {0, 4, X, X,12,12,12,12,12,12,12,12,12, 0},
            {0, 4, X, X, X,13,13,13,13,13,13,13,13,13, 0},
            {0, 4, X, X, X,14,14,14,14,14,14,14,14,14,14, 0},
            {0, 4, X, X, X, X,15,15,15,15,15,15,15,15,15,15, 0},
            {0, 5, X, X, X, X, X,16,16,16,16,16,16,16,16,16,16, 0},
        };
        for (int n = 0; n < known_entries.size(); ++n) {
            assert(known_entries[n].size() == n + 1);
            entries.emplace_back();
            for (int k = 0; k <= n; ++k) {
                int t = known_entries[n][k];
                entries.back().emplace_back();
                if (t >= 0) {
                    entries.back().back().pre_solve(t);
                }
            }
        }
        // Now fill in all the correct mins and maxes.
        for (int n = 0; n < entries.size(); ++n) {
            assert(entries[n].size() == n + 1);
            for (int k = 0; k <= n; ++k) {
                update_mins_and_maxes(n, k);
            }
        }
        if (entries.size() > n) {
            entries.resize(n);
        }
    }

    void update_mins_and_maxes(int n, int k) {
        if (k == 0 || k == n) {
            // The answer is "0" and we learn nothing from this.
            return;
        }
        int min_t = entries[n][k].min_t;
        int max_t = entries[n][k].max_t;
        // Anyone down-and-right of (n,k) cannot take fewer tests than (n,k) takes.
        // In fact, each additional (+1,+1) definitely requires one extra test.
        for (int n2 = n; n2 < entries.size(); ++n2) {
            for (int k2 = k; k2 < n2; ++k2) {
                // We skip k2 == n2 on purpose.
                int extra = std::min((n2 - n), (k2 - k));
                if (entries[n2][k2].min_t < min_t + extra) {
                    entries[n2][k2].min_t = min_t + extra;
                    entries[n2][k2].interrupt_worker();
                }
            }
        }
        // Anyone above-and-left of (n,k) cannot take more tests than (n,k) takes.
        // In fact, each additional (-1,-1) definitely can be done in one fewer test.
        for (int n2 = 0; n2 <= n; ++n2) {
            for (int k2 = 0; k2 <= k && k2 < n2; ++k2) {
                // We skip k2 == n2 on purpose.
                int extra = std::min((n - n2), (k - k2));
                assert(0 <= max_t - extra);
                if (max_t - extra < entries[n2][k2].max_t) {
                    entries[n2][k2].max_t = max_t - extra;
                    if (entries[n2][k2].has_answer()) {
                        entries[n2][k2].interrupt_worker();
                    }
                }
            }
        }
        // Any (n+i, k) cannot take more than t(n,k) + i tests.
        for (int n2 = n; n2 < entries.size(); ++n2) {
            int new_max = max_t + (n2 - n);
            for (int k2 = 1; k2 <= k; ++k2) {
                if (new_max < entries[n2][k2].max_t) {
                    entries[n2][k2].max_t = new_max;
                    if (entries[n2][k2].has_answer()) {
                        entries[n2][k2].interrupt_worker();
                    }
                }
            }
        }
    }

    std::tuple<int, int, int> get_work(std::atomic<bool> *stop_working) {
        auto assign = [&](int n, int k) {
            assert(entries[n][k].stop_working == nullptr);
            entries[n][k].stop_working = stop_working;
            return std::make_tuple(n, k, entries[n][k].min_t);
        };
        std::unique_lock<std::mutex> lk(mtx);
        for (int n=0; n < entries.size(); ++n) {
            assert(entries[n].size() == n+1);
            // Assign work from the middle of a row: k == n/2 is easier.
            if (entries[n][1].is_unstarted()) {
                return assign(n, 1);
            }
            int workers_already_in_this_row = 0;
            for (int k=0; k < n; ++k) {
                if (entries[n][k].is_in_progress()) {
                    workers_already_in_this_row += 1;
                }
            }
            if (workers_already_in_this_row <= 1) {
                // Avoid putting too many workers in a single row
                // of the triangle, because they'll be constantly
                // interrupting each other.
                for (int i=0; i < n; ++i) {
                    int k = eytzinger_from_rank(i, n);
                    assert(0 <= k && k < n);
                    if (entries[n][k].is_unstarted()) {
                        return assign(n, k);
                    }
                }
            }
        }
        // We didn't find any work not-yet-being-done. Start a fresh row.
        int n = entries.size();
        entries.push_back(std::vector<WorkItem>(n+1));
        entries[n][0].pre_solve(0);
        entries[n][n-1].pre_solve(n-1);
        entries[n][n].pre_solve(0);
        // Fill in as much of this row as we can infer automatically.
        for (int k=1; k < n-1; ++k) {
            update_mins_and_maxes(n-1, k);
        }
        update_mins_and_maxes(n, n-1);
        lk.unlock();
        return get_work(stop_working);
    }

    void report_early_terminate(int n, int k) {
        std::lock_guard<std::mutex> lk(mtx);
        assert(0 <= n && n < entries.size());
        assert(0 <= k && k <= entries[n].size());
        entries[n][k].stop_working = nullptr;
    }

    void report_positive_result(int n, int k, int t) {
        std::lock_guard<std::mutex> lk(mtx);
        assert(0 <= n && n < entries.size());
        assert(0 <= k && k <= entries[n].size());
        assert(entries[n][k].min_t <= t && t <= entries[n][k].max_t);
        // It can be done in "t" steps, so the new maximum is "t".
        entries[n][k].stop_working = nullptr;
        if (t < entries[n][k].max_t) {
            entries[n][k].max_t = t;
            update_mins_and_maxes(n, k);
            cv.notify_all();
        }
    }

    void report_negative_result(int n, int k, int t) {
        std::lock_guard<std::mutex> lk(mtx);
        assert(0 <= n && n < entries.size());
        assert(0 <= k && k <= entries[n].size());
        // It can't be done in "t" steps, so the new minimum is "t+1".
        entries[n][k].stop_working = nullptr;
        if (entries[n][k].min_t < t+1) {
            entries[n][k].min_t = t+1;
            update_mins_and_maxes(n, k);
            cv.notify_all();
        }
    }
};

static void worker_thread(Triangle& triangle)
{
    std::atomic<bool> stop_working(false);
    std::tuple<int, int, int> nkt = triangle.get_work(&stop_working);
    auto early_terminate = [&]() {
        return stop_working.load();
    };
    int n = std::get<0>(nkt);
    int k = std::get<1>(nkt);
    int t = std::get<2>(nkt);
    try {
        NktResult result = solve_wolves(n, k, t, early_terminate);
        if (result.success) {
            return triangle.report_positive_result(n, k, t);
        } else {
            return triangle.report_negative_result(n, k, t);
        }
    } catch (const EarlyTerminateException&) {
        // We have been instructed to give up early.
        return triangle.report_early_terminate(n, k);
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
                if (triangle.entries[n][k].has_answer()) {
                    printf("%3d", triangle.entries[n][k].min_t);
                } else if (triangle.entries[n][k].is_in_progress()) {
                    printf("  x");
                } else {
                    printf("  .");
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
