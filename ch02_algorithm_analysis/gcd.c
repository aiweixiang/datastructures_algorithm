/* Chapter 2: Euclid's algorithm.
 * Theorem (Weiss, exercise): gcd(m, n) takes O(log N) recursive calls.
 * Proof sketch: after two iterations the arguments at least halve, because
 * if m > n then m mod n < m/2 (either n <= m/2, or m mod n = m - n < m/2).
 */
#include "ch02.h"

long gcd_euclid(long m, long n)
{
    if (n == 0)
        return m < 0 ? -m : m;
    return gcd_euclid(n, m % n);
}

long gcd_iterative(long m, long n)
{
    while (n != 0) {
        long r = m % n;
        m = n;
        n = r;
    }
    return m < 0 ? -m : m;
}

long gcd_extended(long a, long b, long *x, long *y)
{
    if (b == 0) {
        if (x) *x = 1;
        if (y) *y = 0;
        return a < 0 ? -a : a;
    }
    long x1 = 0, y1 = 0;
    long g = gcd_extended(b, a % b, &x1, &y1);
    if (x) *x = y1;
    if (y) *y = x1 - (a / b) * y1;
    return g;
}

long lcm(long m, long n)
{
    long g = gcd_iterative(m, n);
    if (g == 0)
        return 0;
    long v = (m / g) * n;               /* divide first: less overflow */
    return v < 0 ? -v : v;
}
