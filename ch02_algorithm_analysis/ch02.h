/* Chapter 2: Algorithm Analysis.
 * Public interface for every exercise implemented in this chapter.
 */
#ifndef CH02_H
#define CH02_H

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

#endif /* CH02_H */
