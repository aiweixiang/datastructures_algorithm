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

/* ============ 2.7 随机置换 ============ */
static int is_permutation(const int a[], int n)
{
    unsigned char *seen = calloc((size_t)n, 1);
    int ok = 1;
    if (seen == NULL)
        return 0;
    for (int i = 0; i < n; ++i) {
        if (a[i] < 0 || a[i] >= n || seen[a[i]]) {
            ok = 0;
            break;
        }
        seen[a[i]] = 1;
    }
    free(seen);
    return ok;
}

static void perm_rejection_wrap(int a[], int n, unsigned long *st)
{
    (void)perm_rejection(a, n, st);
}

/* N = 3 时用频次检验均匀性：6 种置换各应出现 trials/6 次左右 */
static int uniform_on_3(void (*algo)(int[], int, unsigned long *))
{
    static const int perms[6][3] = {
        {0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}
    };
    const int trials = 30000;
    int freq[6] = {0};
    int idx[3];
    unsigned long st;
    rng_seed(&st, 424242UL);

    for (int t = 0; t < trials; ++t) {
        algo(idx, 3, &st);
        for (int p = 0; p < 6; ++p)
            if (idx[0] == perms[p][0] && idx[1] == perms[p][1] &&
                idx[2] == perms[p][2]) {
                ++freq[p];
                break;
            }
    }
    int expected = trials / 6;
    for (int p = 0; p < 6; ++p)
        if (freq[p] < expected * 9 / 10 || freq[p] > expected * 11 / 10)
            return 0;
    return 1;
}

static void test_swap_any_exact(void);    /* 前置声明：定义在 test_permutations 之后 */

static void test_permutations(void)
{
    unsigned long st;
    int a[64];

    for (int n = 1; n <= 32; ++n) {
        rng_seed(&st, 1234UL + (unsigned long)n);
        CHECK_EQ(perm_rejection(a, n, &st), 0);
        CHECK(is_permutation(a, n));
        perm_swap_any(a, n, &st);
        CHECK(is_permutation(a, n));
        perm_fisher_yates(a, n, &st);
        CHECK(is_permutation(a, n));
    }

    /* 算法 D 不是置换：N = 64 时几乎必然重复 */
    rng_seed(&st, 99UL);
    perm_naive_random(a, 64, &st);
    CHECK(!is_permutation(a, 64));
    rng_seed(&st, 100UL);
    perm_naive_random(a, 64, &st);
    CHECK(!is_permutation(a, 64));

    /* 2.7 的核心结论：A 与 C 均匀，B **不均匀** */
    CHECK(uniform_on_3(perm_rejection_wrap));
    CHECK(uniform_on_3(perm_fisher_yates));
    CHECK(!uniform_on_3(perm_swap_any));
    test_swap_any_exact();
}

/* 算法 B 的精确分布：枚举 3^3 = 27 条选择序列，不做任何随机采样 */
static void test_swap_any_exact(void)
{
    static const int perms[6][3] = {
        {0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}
    };
    int freq[6] = {0};
    int a[3];

    for (int j0 = 0; j0 < 3; ++j0)
        for (int j1 = 0; j1 < 3; ++j1)
            for (int j2 = 0; j2 < 3; ++j2) {
                int js[3] = {j0, j1, j2};
                perm_identity(a, 3);
                for (int i = 0; i < 3; ++i) {
                    int t = a[i]; a[i] = a[js[i]]; a[js[i]] = t;
                }
                for (int p = 0; p < 6; ++p)
                    if (a[0] == perms[p][0] && a[1] == perms[p][1] &&
                        a[2] == perms[p][2]) {
                        ++freq[p];
                        break;
                    }
            }

    int total = 0, fours = 0, fives = 0;
    for (int p = 0; p < 6; ++p) {
        total += freq[p];
        if (freq[p] == 4) ++fours;
        else if (freq[p] == 5) ++fives;
    }
    CHECK_EQ(total, 27);                 /* 27 条等概率的选择序列 */
    CHECK_EQ(fours, 3);                  /* 三种置换各 4 次 */
    CHECK_EQ(fives, 3);                  /* 另三种各 5 次 → 概率 4/27 vs 5/27 */
    CHECK(27 % 6 != 0);                  /* 均匀的必要条件就不满足 */
}

/* ============ 2.11 不动点 ============ */
static void test_fixed_point(void)
{
    for (int trial = 0; trial < 800; ++trial) {
        int n = rng_range(1, 20);
        int a[20];
        int v = rng_range(-6, 2);
        for (int i = 0; i < n; ++i) {          /* 严格递增 */
            a[i] = v;
            v += rng_range(1, 2);
        }

        int any = 0;
        for (int i = 0; i < n; ++i)
            if (a[i] == i)
                any = 1;
        int got = fixed_point(a, n);
        CHECK_EQ(got != BISECT_NOT_FOUND, any);
        if (got != BISECT_NOT_FOUND) {
            CHECK(got >= 0 && got < n);
            CHECK_EQ(a[got], got);             /* 必须是**一个**合法不动点 */
        }

        int any1 = 0;
        for (int i = 0; i < n; ++i)
            if (a[i] == i + 1)
                any1 = 1;
        int got1 = fixed_point_one_based(a, n);
        CHECK_EQ(got1 != BISECT_NOT_FOUND, any1);
        if (got1 != BISECT_NOT_FOUND)
            CHECK_EQ(a[got1 - 1], got1);
    }

    {   /* 固定例子：不动点集合是连续区间，取到其中哪一个都合法 */
        int x[] = {-3, 1, 3, 4, 5};            /* 0 起始下 a[1] == 1 */
        CHECK_EQ(fixed_point(x, 5), 1);
        CHECK_EQ(fixed_point_one_based(x, 5), 3);   /* 1 起始下 a[2] == 3 */

        int y[] = {-1, 0, 3, 4, 6};
        CHECK_EQ(fixed_point(y, 5), BISECT_NOT_FOUND);   /* 0 起始下无 */
        CHECK_EQ(fixed_point_one_based(y, 5), 3);        /* 1 起始下 a[2] == 3 */

        int y2[] = {-1, 0, 1, 2, 3};        /* 整体下移一格：两种下标下都没有 */
        CHECK_EQ(fixed_point(y2, 5), BISECT_NOT_FOUND);
        CHECK_EQ(fixed_point_one_based(y2, 5), BISECT_NOT_FOUND);

        int z[] = {1, 2, 3};                 /* 1 起始下三个位置都是不动点 */
        int g = fixed_point_one_based(z, 3);
        CHECK(g >= 1 && g <= 3);
        CHECK_EQ(z[g - 1], g);

        int w[] = {0, 1, 2};                 /* 0 起始下全是，1 起始下没有 */
        CHECK(fixed_point(w, 3) >= 0);
        CHECK_EQ(fixed_point_one_based(w, 3), BISECT_NOT_FOUND);
    }
}

/* ============ 2.12 / 2.25 子序列的变体 ============ */
static void test_subsequence_variants(void)
{
    /* 2.25：迭代次数必须等于闭式公式 */
    for (int n = 0; n <= 60; ++n) {
        long long cube = (long long)n * (n + 1) * (n + 2) / 6;
        long long quad = (long long)n * (n + 1) / 2;
        CHECK_EQ(mss_cubic_iterations(n), cube);
        CHECK_EQ(mss_quadratic_iterations(n), quad);
    }

    for (int trial = 0; trial < 400; ++trial) {
        int n = rng_range(1, 14);
        int a[14];
        for (int i = 0; i < n; ++i)
            a[i] = rng_range(-6, 6);

        /* 独立参考：枚举所有 O(N^2) 窗口 */
        int ref_min = 0, ref_min_nonempty = INT_MAX, ref_min_pos = 0;
        long long ref_prod = 0;
        int have_prod = 0;
        for (int i = 0; i < n; ++i) {
            int s = 0;
            long long p = 1;
            for (int j = i; j < n; ++j) {
                s += a[j];
                p *= a[j];
                if (s < ref_min) ref_min = s;
                if (s < ref_min_nonempty) ref_min_nonempty = s;
                if (s > 0 && (ref_min_pos == 0 || s < ref_min_pos)) ref_min_pos = s;
                if (!have_prod || p > ref_prod) { ref_prod = p; have_prod = 1; }
            }
        }
        CHECK_EQ(mss_min(a, n), ref_min);
        CHECK_EQ(mss_min_nonempty(a, n), ref_min_nonempty);
        CHECK_EQ(mss_min_positive(a, n), ref_min_pos);
        CHECK_EQ(mss_max_product(a, n), ref_prod);
    }

    {
        int allneg[] = {-3, -1, -7};
        CHECK_EQ(mss_min(allneg, 3), -11);
        CHECK_EQ(mss_min_nonempty(allneg, 3), -11);
        CHECK_EQ(mss_min_positive(allneg, 3), 0);   /* 不存在正的和 */
        CHECK_EQ(mss_max_product(allneg, 3), 7);    /* (-1)*(-7) = 7 最大 */

        int mixed[] = {2, 3, -2, 4};
        CHECK_EQ(mss_max_product(mixed, 4), 6);
        CHECK_EQ(mss_min(mixed, 4), -2);
        CHECK_EQ(mss_min_positive(mixed, 4), 1);    /* 子序列 {3,-2} */

        int nn[] = {-2, -3, -1};
        CHECK_EQ(mss_max_product(nn, 3), 6);

        int zeros[] = {0, 2};
        CHECK_EQ(mss_max_product(zeros, 2), 2);
    }
}

/* ============ 2.13 / 2.14 素数 ============ */
static void test_primes(void)
{
    const int limit = 20000;
    unsigned char *comp = malloc((size_t)limit + 1);
    CHECK(comp != NULL);
    if (comp == NULL)
        return;

    int sieve_count = sieve_primes(limit, comp);
    CHECK_EQ(sieve_count, 2262);                 /* π(20000) */
    int mismatches = 0, brute_count = 0;
    for (int i = 0; i <= limit; ++i) {
        int want = (i >= 2) && !comp[i];
        if (want) ++brute_count;
        if (is_prime((unsigned long long)i) != want) ++mismatches;
    }
    CHECK_EQ(mismatches, 0);                     /* 试除法与筛法必须一致 */
    CHECK_EQ(brute_count, sieve_count);
    free(comp);

    CHECK_EQ(is_prime(0), 0);
    CHECK_EQ(is_prime(1), 0);
    CHECK_EQ(is_prime(2), 1);
    CHECK_EQ(is_prime(3), 1);
    CHECK_EQ(is_prime(97), 1);
    CHECK_EQ(is_prime(100), 0);
    CHECK_EQ(is_prime(1000000007ULL), 1);        /* 10^9 + 7 */
    CHECK_EQ(is_prime(1000000009ULL), 1);
    CHECK_EQ(is_prime(999999937ULL), 1);
    CHECK_EQ(is_prime(1000000005ULL), 0);

    unsigned char small[10001];
    CHECK_EQ(sieve_primes(10, small), 4);        /* π(10)   */
    CHECK_EQ(sieve_primes(100, small), 25);      /* π(100)  */
    CHECK_EQ(sieve_primes(1000, small), 168);    /* π(1000) */
    CHECK_EQ(sieve_primes(10000, small), 1229);  /* π(10^4) */
    CHECK_EQ(sieve_primes(1, small), 0);
    CHECK_EQ(sieve_primes(0, small), 0);
    CHECK_EQ(sieve_primes(-1, small), 0);

    int primes[32];
    CHECK_EQ(sieve_collect(100, primes, 32), 25);
    CHECK_EQ(primes[0], 2);
    CHECK_EQ(primes[1], 3);
    CHECK_EQ(primes[24], 97);
    CHECK_EQ(sieve_collect(100, primes, 5), 5);  /* 容量截断 */
    CHECK_EQ(sieve_collect(1, primes, 5), 0);
}

/* ============ 2.19 主要元素 ============ */
static void test_majority(void)
{
    int found = -1;

    int a1[] = {3, 3, 4, 2, 4, 4, 2, 4, 4};
    CHECK_EQ(majority_element(a1, 9, &found), 4);
    CHECK_EQ(found, 1);

    int a2[] = {1, 2, 3};                        /* 投票候选者必须再验证 */
    CHECK_EQ(majority_element(a2, 3, &found), 0);
    CHECK_EQ(found, 0);

    int a3[] = {5, 5, 5};
    CHECK_EQ(majority_element(a3, 3, &found), 5);
    CHECK_EQ(found, 1);

    int a4[] = {1};
    CHECK_EQ(majority_element(a4, 1, &found), 1);
    CHECK_EQ(found, 1);

    CHECK_EQ(majority_element(NULL, 0, &found), 0);
    CHECK_EQ(found, 0);

    for (int trial = 0; trial < 600; ++trial) {
        int n = rng_range(1, 30);
        int a[40];
        for (int i = 0; i < n; ++i)
            a[i] = rng_range(0, 4);
        if (rng_range(0, 1)) {                   /* 一半概率注入多数元素 */
            int maj = rng_range(10, 12);
            for (int i = 0; i <= n / 2; ++i)
                a[i] = maj;
            unsigned long st;
            rng_seed(&st, 1000UL + (unsigned long)trial);
            perm_fisher_yates(a, n, &st);        /* 打散布局 */
        }

        int ref = 0, ref_found = 0;
        for (int v = 0; v <= 12; ++v) {
            int c = 0;
            for (int i = 0; i < n; ++i)
                if (a[i] == v) ++c;
            if (c > n / 2) { ref = v; ref_found = 1; break; }
        }
        int got = majority_element(a, n, &found);
        CHECK_EQ(found, ref_found);
        if (ref_found)
            CHECK_EQ(got, ref);
    }
}

/* ============ 2.15 / 2.16 / 2.17 快速幂的乘法次数 ============ */
static void test_pow_chain(void)
{
    for (int n = 1; n <= 1000; ++n) {
        int counted = 0;
        (void)pow_binary_count(1, n, &counted);
        CHECK_EQ(counted, binary_pow_mult_count(n));
    }
    CHECK_EQ(binary_pow_mult_count(1), 1);
    CHECK_EQ(binary_pow_mult_count(2), 2);
    CHECK_EQ(binary_pow_mult_count(3), 3);
    CHECK_EQ(binary_pow_mult_count(62), 10);     /* 2.17：5 次平方 + 5 次乘 x */

    CHECK_EQ(pow62_optimal_mults(), 8);          /* 2.15 */
    for (long long x = 0; x <= 2; ++x) {
        CHECK_EQ(pow62_optimal(x), pow_fast(x, 62));
        CHECK_EQ(pow62_optimal(x), pow_binary_count(x, 62, NULL));
    }
    CHECK_EQ(pow62_optimal(2), 4611686018427387904LL);   /* 2^62 */

    for (int x = 0; x <= 6; ++x)                 /* 2.16：迭代版 == 朴素版 */
        for (int n = 0; n <= 12; ++n)
            CHECK_EQ(pow_fast(x, n), pow_slow(x, n));
}

/* ============ 2.23 单比较二分查找 ============ */
static void test_bisect_one_compare(void)
{
    for (int n = 0; n <= 40; ++n) {
        int a[40];
        for (int i = 0; i < n; ++i)
            a[i] = i * 3;
        for (int x = -2; x <= 3 * n + 2; ++x) {
            int cmps = 0;
            int got = bisect_one_compare_count(a, x, n, &cmps);
            CHECK_EQ(got, bisect(a, x, n));
            if (n > 0) {
                int ceil_log2 = 0;
                long long p = 1;
                while (p < n) { p *= 2; ++ceil_log2; }
                CHECK(cmps <= ceil_log2 + 1);       /* 每轮仅一次比较 */
            }
        }
    }
    CHECK_EQ(bisect_one_compare(NULL, 5, 0), BISECT_NOT_FOUND);
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
    test_permutations();
    test_fixed_point();
    test_subsequence_variants();
    test_primes();
    test_majority();
    test_pow_chain();
    test_bisect_one_compare();
    return TEST_REPORT();
}
