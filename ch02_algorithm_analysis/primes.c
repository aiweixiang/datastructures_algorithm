/* 习题 2.13（素数判定）与 2.14（埃拉托斯特尼筛法）。
 *
 * is_prime：试除到 sqrt(N)，O(sqrt(N)) 时间、O(1) 空间。
 *   只需试 2 和所有奇数：若 N 有因子 d > sqrt(N)，必有配对因子 N/d < sqrt(N)。
 * sieve_primes：筛法，O(N log log N) 时间、O(N) 空间。
 *   时间来源：Σ_{p ≤ N} N/p = N·Σ 1/p ≈ N·ln ln N（梅尔滕斯定理）。
 *   Σ 1/p 发散得极慢，所以筛法几乎线性。
 */
#include <stdlib.h>
#include "ch02.h"

int is_prime(unsigned long long n)
{
    if (n < 2)
        return 0;
    if (n % 2 == 0)
        return n == 2;
    /* 用 d <= n / d 而不是 d * d <= n，避免 d*d 溢出 */
    for (unsigned long long d = 3; d <= n / d; d += 2)
        if (n % d == 0)
            return 0;
    return 1;
}

/* composite[i] != 0 表示 i 是合数；composite[0] 与 [1] 都置 1。
 * 返回 [0, limit] 内的素数个数。 */
int sieve_primes(int limit, unsigned char *composite)
{
    if (limit < 0 || composite == NULL)
        return 0;
    for (int i = 0; i <= limit; ++i)
        composite[i] = 0;
    composite[0] = 1;
    if (limit >= 1)
        composite[1] = 1;

    int count = 0;
    for (int i = 2; i <= limit; ++i) {
        if (composite[i])
            continue;
        ++count;
        if ((long long)i * i <= limit)          /* 只需从 i*i 开始划掉 */
            for (long long j = (long long)i * i; j <= limit; j += i)
                composite[j] = 1;
    }
    return count;
}

int sieve_collect(int limit, int *primes, int max)
{
    if (limit < 2 || primes == NULL || max <= 0)
        return 0;
    unsigned char *comp = malloc((size_t)limit + 1);
    if (comp == NULL)
        return -1;
    sieve_primes(limit, comp);
    int n = 0;
    for (int i = 2; i <= limit && n < max; ++i)
        if (!comp[i])
            primes[n++] = i;
    free(comp);
    return n;
}
