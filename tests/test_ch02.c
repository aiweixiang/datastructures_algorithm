/* Tests for every Chapter 2 implementation. */
#include <limits.h>
#include <stdlib.h>
#include "ch02_algorithm_analysis/ch02.h"
#include "test_util.h"

/* deterministic pseudo-random numbers, so failures are reproducible */
static unsigned long rng_state = 20260920UL;
static int rng_range(int lo, int hi)
{
    rng_state = rng_state * 1103515245UL + 12345UL;
    return lo + (int)((rng_state >> 16) % (unsigned long)(hi - lo + 1));
}

/* independent O(N^2) reference */
static int ref_mss(const int a[], int n)
{
    int best = 0;
    for (int i = 0; i < n; ++i) {
        int s = 0;
        for (int j = i; j < n; ++j) {
            s += a[j];
            if (s > best)
                best = s;
        }
    }
    return best;
}

static void test_max_subsequence_sum(void)
{
    const int weiss[] = {-2, 11, -4, 13, -5, -2};      /* answer 20 */
    CHECK_EQ(mss_cubic(weiss, 6), 20);
    CHECK_EQ(mss_quadratic(weiss, 6), 20);
    CHECK_EQ(mss_divide_conquer(weiss, 6), 20);
    CHECK_EQ(mss_linear(weiss, 6), 20);

    const int allneg[] = {-3, -1, -7};
    CHECK_EQ(mss_cubic(allneg, 3), 0);                 /* empty subsequence */
    CHECK_EQ(mss_divide_conquer(allneg, 3), 0);
    CHECK_EQ(mss_linear(allneg, 3), 0);
    CHECK_EQ(mss_linear(NULL, 0), 0);

    int s = -1, e = -1;
    CHECK_EQ(mss_linear_range(weiss, 6, &s, &e), 20);
    CHECK_EQ(s, 1);
    CHECK_EQ(e, 3);
    CHECK_EQ(mss_linear_range(allneg, 3, &s, &e), 0);
    CHECK_EQ(s, -1);
    CHECK_EQ(e, -1);

    for (int trial = 0; trial < 300; ++trial) {
        int n = rng_range(1, 45);
        int a[45];
        for (int i = 0; i < n; ++i)
            a[i] = rng_range(-25, 25);
        int want = ref_mss(a, n);
        CHECK_EQ(mss_cubic(a, n), want);
        CHECK_EQ(mss_quadratic(a, n), want);
        CHECK_EQ(mss_divide_conquer(a, n), want);
        CHECK_EQ(mss_linear(a, n), want);
        int rs, re;
        CHECK_EQ(mss_linear_range(a, n, &rs, &re), want);
    }
}

static void test_binary_search(void)
{
    for (int n = 0; n <= 30; ++n) {
        int a[30];
        for (int i = 0; i < n; ++i)
            a[i] = i * 2;                      /* 0, 2, 4, ... */
        for (int x = -1; x <= 2 * n + 1; ++x) {
            int found = BISECT_NOT_FOUND;
            for (int i = 0; i < n; ++i)
                if (a[i] == x) { found = i; break; }
            CHECK_EQ(bisect(a, x, n), found);
            CHECK_EQ(bisect_recursive(a, x, 0, n - 1), found);

            int lb = n;
            for (int i = 0; i < n; ++i)
                if (a[i] >= x) { lb = i; break; }
            CHECK_EQ(lower_bound_idx(a, x, n), lb);

            int ub = n;
            for (int i = 0; i < n; ++i)
                if (a[i] > x) { ub = i; break; }
            CHECK_EQ(upper_bound_idx(a, x, n), ub);
        }
    }
}

static long ref_gcd(long m, long n)
{
    long g = 0;
    for (long i = 1; i <= m && i <= n; ++i)
        if (m % i == 0 && n % i == 0)
            g = i;
    return g;
}

static void test_gcd(void)
{
    for (long m = 1; m <= 60; ++m) {
        for (long n = 1; n <= 60; ++n) {
            long want = ref_gcd(m, n);
            CHECK_EQ(gcd_euclid(m, n), want);
            CHECK_EQ(gcd_iterative(m, n), want);
            long x = 0, y = 0;
            long g = gcd_extended(m, n, &x, &y);
            CHECK_EQ(g, want);
            CHECK_EQ(m * x + n * y, want);
        }
    }
    CHECK_EQ(gcd_euclid(48, 18), 6);
    CHECK_EQ(gcd_iterative(0, 5), 5);
    CHECK_EQ(lcm(4, 6), 12);
    CHECK_EQ(lcm(21, 6), 42);
}

static void test_power(void)
{
    for (long long x = 1; x <= 5; ++x)
        for (int n = 0; n <= 12; ++n)
            CHECK_EQ(pow_fast(x, n), pow_slow(x, n));

    for (long long x = 0; x <= 30; ++x) {
        for (long long n = 0; n <= 12; ++n) {
            for (long long mod = 2; mod <= 40; ++mod) {
                long long want = 1 % mod;
                for (long long i = 0; i < n; ++i)
                    want = (want * x) % mod;
                CHECK_EQ(pow_mod(x, n, mod), want);
            }
        }
    }
    CHECK_EQ(pow_fast(2, 10), 1024);
    CHECK_EQ(pow_mod(2, 10, 1000), 24);
}

static void test_logs(void)
{
    for (int n = 1; n <= 5000; ++n) {
        int want = 0;
        long long p = 1;
        while (p * 2 <= n) { p *= 2; ++want; }
        CHECK_EQ(log2_floor_loop(n), want);
        CHECK_EQ(log2_floor_bits(n), want);

        int cwant = 0;
        long long q = 1;
        while (q < n) { q *= 2; ++cwant; }
        CHECK_EQ(ceil_log2(n), cwant);
    }
    CHECK_EQ(log2_floor_loop(0), -1);
    CHECK_EQ(log2_floor_bits(-4), -1);
    CHECK_EQ(log2_floor_bits(1), 0);
    CHECK_EQ(ceil_log2(1), 0);
    CHECK_EQ(is_power_of_two(1024), 1);
    CHECK_EQ(is_power_of_two(1000), 0);
    CHECK_EQ(is_power_of_two(0), 0);
    CHECK_EQ(is_power_of_two(-8), 0);
}

static void test_horner(void)
{
    double c[] = {1.0, 2.0, 3.0};                 /* 1 + 2x + 3x^2 */
    CHECK_NEAR(horner(c, 2, 2.0), 17.0, 1e-9);
    CHECK_NEAR(poly_eval_naive(c, 2, 2.0), 17.0, 1e-9);

    for (int trial = 0; trial < 200; ++trial) {
        double coeff[8];
        int deg = rng_range(0, 7);
        for (int i = 0; i <= deg; ++i)
            coeff[i] = (double)rng_range(-100, 100) / 10.0;
        double x = (double)rng_range(-30, 30) / 10.0;
        CHECK_NEAR(horner(coeff, deg, x), poly_eval_naive(coeff, deg, x), 1e-6);
    }
}

static void test_fibonacci(void)
{
    for (int n = 0; n <= 30; ++n) {
        unsigned long long want = fib_iterative(n);
        CHECK_EQ(fib_recursive(n), want);
        CHECK_EQ(fib_memo(n), want);
        unsigned long long fn = 0, fn1 = 0;
        fib_matrix((unsigned long long)n, &fn, &fn1);
        CHECK_EQ(fn, want);
        CHECK_EQ(fn1, fib_iterative(n + 1));
    }
    CHECK_EQ(fib_iterative(10), 55);
    CHECK_EQ(fib_iterative(93), 12200160415121876738ULL);
    fib_memo_reset();
    CHECK_EQ(fib_memo(93), 12200160415121876738ULL);
    CHECK_EQ(fib_memo(92) + fib_memo(91), 12200160415121876738ULL);
}

static int cmp_desc(const void *pa, const void *pb)
{
    int a = *(const int *)pa, b = *(const int *)pb;
    return (a < b) - (a > b);
}

static void test_kth_largest(void)
{
    for (int trial = 0; trial < 300; ++trial) {
        int n = rng_range(1, 40);
        int a[40], sorted[40];
        for (int i = 0; i < n; ++i) {
            a[i] = rng_range(-50, 50);
            sorted[i] = a[i];
        }
        qsort(sorted, (size_t)n, sizeof *sorted, cmp_desc);   /* descending */
        int k = rng_range(1, n);
        int want = sorted[k - 1];

        int copy1[40], copy2[40];
        for (int i = 0; i < n; ++i) { copy1[i] = a[i]; copy2[i] = a[i]; }
        CHECK_EQ(kth_largest_sort(copy1, n, k), want);
        CHECK_EQ(kth_largest_heap(a, n, k), want);
        CHECK_EQ(kth_largest_quickselect(copy2, n, k), want);
    }
    {
        int a[] = {3, 2, 1, 5, 6, 4};
        int c1[6], c2[6];
        for (int i = 0; i < 6; ++i) { c1[i] = a[i]; c2[i] = a[i]; }
        CHECK_EQ(kth_largest_sort(c1, 6, 2), 5);
        CHECK_EQ(kth_largest_heap(a, 6, 2), 5);
        CHECK_EQ(kth_largest_quickselect(c2, 6, 2), 5);
        CHECK_EQ(kth_largest_heap(a, 6, 0), INT_MIN);
        CHECK_EQ(kth_largest_heap(a, 6, 7), INT_MIN);
    }
}

int main(void)
{
    test_max_subsequence_sum();
    test_binary_search();
    test_gcd();
    test_power();
    test_logs();
    test_horner();
    test_fibonacci();
    test_kth_largest();
    return TEST_REPORT();
}
