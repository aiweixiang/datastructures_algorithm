/* Chapter 2: Fibonacci.
 * fib_recursive is exponential (T(N) = T(N-1) + T(N-2) + O(1) ~ phi^N) and
 * exists only to show how a plausible recursion can be disastrous.
 */
#include <string.h>
#include "ch02.h"

unsigned long long fib_recursive(int n)
{
    if (n <= 1)
        return (unsigned long long)(n < 0 ? 0 : n);
    return fib_recursive(n - 1) + fib_recursive(n - 2);
}

unsigned long long fib_iterative(int n)
{
    if (n < 0)
        return 0;
    unsigned long long a = 0, b = 1;      /* a = F(i), b = F(i+1) */
    for (int i = 0; i < n; ++i) {
        unsigned long long t = a + b;
        a = b;
        b = t;
    }
    return a;
}

static unsigned long long fib_cache[FIB_MEMO_MAX];
static signed char fib_known[FIB_MEMO_MAX];

void fib_memo_reset(void)
{
    memset(fib_cache, 0, sizeof fib_cache);
    memset(fib_known, 0, sizeof fib_known);
}

static unsigned long long fib_memo_rec(int n)
{
    if (n <= 1)
        return (unsigned long long)n;
    if (fib_known[n])
        return fib_cache[n];
    fib_cache[n] = fib_memo_rec(n - 1) + fib_memo_rec(n - 2);
    fib_known[n] = 1;
    return fib_cache[n];
}

unsigned long long fib_memo(int n)
{
    if (n < 0)
        return 0;
    if (n >= FIB_MEMO_MAX)
        return fib_iterative(n);
    return fib_memo_rec(n);
}

static void mat_mul2(unsigned long long m[2][2], const unsigned long long n[2][2])
{
    unsigned long long t[2][2];
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            t[i][j] = m[i][0] * n[0][j] + m[i][1] * n[1][j];
    memcpy(m, t, sizeof t);
}

void fib_matrix(unsigned long long n, unsigned long long *fn, unsigned long long *fn1)
{
    unsigned long long r[2][2] = {{1, 0}, {0, 1}};   /* identity */
    unsigned long long base[2][2] = {{1, 1}, {1, 0}};

    while (n > 0) {
        if (n & 1)
            mat_mul2(r, base);
        mat_mul2(base, base);
        n >>= 1;
    }
    /* r == [[F(n+1), F(n)], [F(n), F(n-1)]] */
    if (fn)  *fn  = r[0][1];
    if (fn1) *fn1 = r[0][0];
}
