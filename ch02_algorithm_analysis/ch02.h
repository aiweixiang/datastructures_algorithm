/* Chapter 2: Algorithm Analysis.
 * Public interface for every exercise implemented in this chapter.
 */
#ifndef CH02_H
#define CH02_H

#include <stddef.h>          /* NULL */

/* ---- 2.x  Maximum contiguous subsequence sum (empty subsequence -> 0) ---- */
int mss_cubic(const int a[], int n);            /* O(N^3)   */
int mss_quadratic(const int a[], int n);        /* O(N^2)   */
int mss_divide_conquer(const int a[], int n);   /* O(N logN)*/
int mss_linear(const int a[], int n);           /* O(N)     */
/* O(N) version that also reports the winning range [*start, *end];
 * both are set to -1 when the best sum is the empty subsequence. */
int mss_linear_range(const int a[], int n, int *start, int *end);

/* ---- 2.x  Binary search ---- */
#define BISECT_NOT_FOUND (-1)
int bisect(const int a[], int x, int n);                    /* classic Weiss */
int bisect_recursive(const int a[], int x, int lo, int hi);
int lower_bound_idx(const int a[], int x, int n);           /* first a[i] >= x */
int upper_bound_idx(const int a[], int x, int n);           /* first a[i] >  x */

/* ---- 2.x  Euclid's algorithm / gcd ---- */
long gcd_euclid(long m, long n);
long gcd_iterative(long m, long n);
/* a*x + b*y == gcd(a, b) */
long gcd_extended(long a, long b, long *x, long *y);
long lcm(long m, long n);

/* ---- 2.x  Exponentiation ---- */
long long pow_slow(long long x, int n);                     /* O(N)   */
long long pow_fast(long long x, int n);                     /* O(logN)*/
long long pow_mod(long long x, long long n, long long mod); /* O(logN)*/

/* ---- 2.x  Logarithms and powers of two ---- */
int log2_floor_loop(int n);     /* floor(log2 n), -1 for n <= 0 */
int log2_floor_bits(int n);     /* same, via shifts            */
int ceil_log2(int n);           /* smallest k with 2^k >= n    */
int is_power_of_two(int n);

/* ---- 2.x  Horner's rule for polynomial evaluation ---- */
double horner(const double coeff[], int degree, double x);
double poly_eval_naive(const double coeff[], int degree, double x);

/* ---- 2.x  Fibonacci: bad recursion vs. good algorithms ---- */
#define FIB_MEMO_MAX 94         /* F(93) is the largest value in uint64_t */
unsigned long long fib_recursive(int n);   /* O(phi^n) -- exponential */
unsigned long long fib_iterative(int n);   /* O(N) */
unsigned long long fib_memo(int n);        /* O(N), memoized recursion */
void fib_memo_reset(void);
/* fast doubling via 2x2 matrix power: *fn = F(n), *fn1 = F(n+1) */
void fib_matrix(unsigned long long n, unsigned long long *fn, unsigned long long *fn1);

/* ---- 2/7.x  Selection problem: k-th largest element ---- */
/* All three return INT_MIN when k is out of range. */
int kth_largest_sort(int a[], int n, int k);          /* O(N log N) sorts a  */
int kth_largest_heap(const int a[], int n, int k);    /* O(N log k), no copy */
int kth_largest_quickselect(int a[], int n, int k);   /* O(N) average        */

/* ---- 可复现的伪随机数（供 2.7 等随机实验使用） ---- */
void          rng_seed(unsigned long *state, unsigned long seed);
unsigned long rng_next(unsigned long *state);
int           rng_uniform(unsigned long *state, int n);      /* [0, n) */

/* ---- 2.7 三种随机置换算法（外加一种错误做法） ---- */
void perm_identity(int a[], int n);
int  perm_rejection(int a[], int n, unsigned long *state);   /* A：均匀，期望 O(N log N) */
void perm_swap_any(int a[], int n, unsigned long *state);    /* B：均匀，O(N) */
void perm_fisher_yates(int a[], int n, unsigned long *state);/* C：均匀，O(N) */
void perm_naive_random(int a[], int n, unsigned long *state);/* D：不是置换 */

/* ---- 2.11 是否存在 i 使 A[i] == i（有序数组） ---- */
int fixed_point(const int a[], int n);            /* 0 起始；-1 表示不存在 */
int fixed_point_one_based(const int a[], int n);  /* 返回 1 起始位置 */

/* ---- 2.12 最小子序列和 / 最小正子序列和 / 最大子序列乘积 ---- */
int       mss_min(const int a[], int n);              /* 允许空 → ≤ 0 */
int       mss_min_nonempty(const int a[], int n);     /* 非空 */
int       mss_min_positive(const int a[], int n);     /* 最小正和；0 表示不存在 */
long long mss_max_product(const int a[], int n);      /* 非空连续乘积 */

/* ---- 2.25 立方/平方算法的迭代次数 ---- */
long long mss_cubic_iterations(int n);                /* N(N+1)(N+2)/6 */
long long mss_quadratic_iterations(int n);            /* N(N+1)/2 */

/* ---- 2.13 / 2.14 素数 ---- */
int is_prime(unsigned long long n);
int sieve_primes(int limit, unsigned char *composite);        /* 返回素数个数 */
int sieve_collect(int limit, int *primes, int max);           /* 前 max 个素数 */

/* ---- 2.19 主要元素（出现次数 > N/2） ---- */
int majority_element(const int a[], int n, int *found);

/* ---- 2.15 / 2.16 / 2.17 快速幂的乘法次数 ---- */
long long pow_binary_count(long long x, int n, int *mults);
int       binary_pow_mult_count(int n);        /* ⌊log2 N⌋ + popcount(N) */
long long pow62_optimal(long long x);          /* 恰好 8 次乘法 */
int       pow62_optimal_mults(void);           /* == 8 */

/* ---- 2.23 每次迭代只做一次比较的二分查找 ---- */
int bisect_one_compare(const int a[], int x, int n);
int bisect_one_compare_count(const int a[], int x, int n, int *comparisons);

#endif /* CH02_H */
