
#include <assert.h>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>
#include "wolves.h"

struct WorkItem {
    std::atomic<bool> *notify_done = nullptr;
    int min_t = 0;
    int max_t = INT_MAX;

    void pre_solve(int t) {
        min_t = t;
        max_t = t;
    }

    bool is_unstarted() const {
        return (min_t != max_t) && (notify_done == nullptr);
    }

    bool is_in_progress() const {
        return notify_done != nullptr;
    }

    bool is_done() const {
        return (min_t == max_t) && (notify_done == nullptr);
    }

    void interrupt_worker() {
        if (notify_done != nullptr) {
            *notify_done = true;
        }
    }
};

struct Triangle {
    std::mutex mtx;
    std::condition_variable cv;
    std::vector<std::vector<WorkItem>> entries;

    explicit Triangle(int n) {
        std::vector<std::vector<int>> known_entries = {
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
        for (int n = 0; n < known_entries.size(); ++n) {
            assert(known_entries[n].size() == n + 1);
            entries.emplace_back();
            for (int k = 0; k <= n; ++k) {
                int t = known_entries[n][k];
                entries.back().emplace_back();
                if (t >= 0) {
                    entries.back().back().min_t = t;
                    entries.back().back().max_t = t;
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
        int min_t = entries[n][k].min_t;
        int max_t = entries[n][k].max_t;
        // Anyone down-and-right of (n,k) cannot take fewer tests than (n,k) takes.
        for (int n2 = n; n2 < entries.size(); ++n2) {
            for (int k2 = k; k2 < n2; ++k2) {
                // We skip k2 == n2 on purpose.
                if (entries[n2][k2].min_t < min_t) {
                    entries[n2][k2].min_t = min_t;
                    entries[n2][k2].interrupt_worker();
                }
            }
        }
        if (k != n) {
            // Anyone above-and-left of (n,k) cannot take more tests than (n,k) takes.
            for (int n2 = 0; n2 <= n; ++n2) {
                for (int k2 = 0; k2 <= k && k2 < n2; ++k2) {
                    // We skip k2 == n2 on purpose.
                    if (entries[n2][k2].max_t > max_t) {
                        entries[n2][k2].max_t = max_t;
                        entries[n2][k2].interrupt_worker();
                    }
                }
            }
        }
    }

    std::tuple<int, int> get_work(std::atomic<bool> *done) {
        auto assign = [&](int n, int k) {
            assert(entries[n][k].notify_done == nullptr);
            entries[n][k].notify_done = done;
            return std::make_tuple(n, k);
        };
        std::unique_lock<std::mutex> lk(mtx);
        for (int n=0; n < entries.size(); ++n) {
            assert(entries[n].size() == n+1);
            // Assign work from the middle of a row: k == n/2 is easier.
            if (entries[n][1].is_unstarted()) {
                return assign(n, 1);
            }
            for (int i=0; i < n; ++i) {
                int k = (n % 2) == 1 ?
                    ((i % 2) == 0 ? (n / 2) - (i / 2) : (n / 2) + (i / 2) + 1) :
                    ((i % 2) == 0 ? (n / 2) + (i / 2) : (n / 2) - (i / 2) - 1);
                assert(0 <= k && k < n);
                if (entries[n][k].is_unstarted()) {
                    return assign(n, k);
                }
            }
        }
        // We didn't find any work not-yet-being-done. Start a fresh row.
        int n = entries.size();
        entries.push_back(std::vector<WorkItem>(n+1));
        entries[n][0].pre_solve(0);
        entries[n][n-1].pre_solve(n-1);
        entries[n][n].pre_solve(0);
        update_mins_and_maxes(n, n-1);
        lk.unlock();
        return get_work(done);
    }

    int get_min_t(int n, int k) {
        if (n == 0 || k == 0 || k == n) return 0;
        if (k == n-1) return n-1;
        std::lock_guard<std::mutex> lk(mtx);
        return entries[n][k].min_t;
    }

    int get_max_t(int n, int k) {
        if (n == 0 || k == 0 || k == n) return 0;
        if (k == n-1) return n-1;
        std::lock_guard<std::mutex> lk(mtx);
        return entries[n][k].max_t;
    }

    void report_early_terminate(int n, int k) {
        std::lock_guard<std::mutex> lk(mtx);
        assert(0 <= n && n < entries.size());
        assert(0 <= k && k <= entries[n].size());
        entries[n][k].notify_done = nullptr;
    }

    void report_positive_result(int n, int k, int t) {
        std::lock_guard<std::mutex> lk(mtx);
        assert(0 <= n && n < entries.size());
        assert(0 <= k && k <= entries[n].size());
        if (entries[n][k].is_done()) {
            // Someone else must have raced us to the finish line.
            // Check that we got the same answer.
            assert(entries[n][k].min_t == t);
            assert(entries[n][k].max_t == t);
        } else {
            entries[n][k].min_t = t;
            entries[n][k].max_t = t;
            entries[n][k].notify_done = nullptr;
            update_mins_and_maxes(n, k);
        }
        cv.notify_all();
    }

    void report_new_min_t(int n, int k, int min_t) {
        std::lock_guard<std::mutex> lk(mtx);
        assert(0 <= n && n < entries.size());
        assert(0 <= k && k <= entries[n].size());
        if (entries[n][k].is_done()) {
            // Someone else must have raced us to the finish line.
            assert(min_t <= entries[n][k].min_t);
            assert(entries[n][k].min_t == entries[n][k].max_t);
            return;
        }
        assert(entries[n][k].is_in_progress());
        if (entries[n][k].min_t < min_t) {
            assert(min_t <= entries[n][k].max_t);
            entries[n][k].min_t = min_t;
            update_mins_and_maxes(n, k);
            cv.notify_all();
        }
    }
};

static void worker_thread(Triangle& triangle)
{
    std::atomic<bool> done(false);
    std::tuple<int, int> nk = triangle.get_work(&done);
    auto early_terminate = [&]() {
        return done.load();
    };
    int n = std::get<0>(nk);
    int k = std::get<1>(nk);
    int min_t = triangle.get_min_t(n, k);
    int max_t = triangle.get_max_t(n, k);
    for (int t = min_t; t < max_t; ++t) {
        triangle.report_new_min_t(n, k, t);
        try {
            NktResult result = solve_wolves(n, k, t, early_terminate);
            if (result.success) {
                return triangle.report_positive_result(n, k, t);
            }
        } catch (const EarlyTerminateException&) {
            // We have been instructed to give up early.
            return triangle.report_early_terminate(n, k);
        }
    }
    return triangle.report_positive_result(n, k, max_t);
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
                if (triangle.entries[n][k].is_done()) {
                    assert(triangle.entries[n][k].min_t == triangle.entries[n][k].max_t);
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
