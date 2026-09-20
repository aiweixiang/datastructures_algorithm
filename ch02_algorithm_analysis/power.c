/* Chapter 2: computing x^n.  The naive loop is O(N); the divide-and-conquer
 * version (or its iterative binary-power twin) is O(log N).
 */
#include "ch02.h"

long long pow_slow(long long x, int n)
{
    long long result = 1;
    for (int i = 0; i < n; ++i)
        result *= x;
    return result;
}

long long pow_fast(long long x, int n)
{
    if (n < 0)
        return 0;                       /* not supported for integer base */
    long long result = 1;
    while (n > 0) {
        if (n & 1)
            result *= x;
        x *= x;
        n >>= 1;
    }
    return result;
}

long long pow_mod(long long x, long long n, long long mod)
{
    if (mod == 1)
        return 0;
    long long result = 1;
    x %= mod;
    if (x < 0)
        x += mod;
    while (n > 0) {
        if (n & 1)
            result = (result * x) % mod;
        x = (x * x) % mod;
        n >>= 1;
    }
    return result;
}
