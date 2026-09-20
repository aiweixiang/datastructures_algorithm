/* Chapter 2: floor(log2 n) and friends.
 * Both implementations are O(log N) in the number of shifts, but the shift
 * version shows why logarithms in algorithm analysis are really bit counts.
 */
#include "ch02.h"

int log2_floor_loop(int n)
{
    if (n <= 0)
        return -1;
    int k = 0;
    long long p = 1;
    while (p * 2 <= n) {
        p *= 2;
        ++k;
    }
    return k;
}

int log2_floor_bits(int n)
{
    if (n <= 0)
        return -1;
    unsigned v = (unsigned)n;
    int k = 0;
    while ((v >>= 1) != 0)
        ++k;
    return k;
}

int ceil_log2(int n)
{
    if (n <= 1)
        return 0;
    int k = log2_floor_bits(n - 1) + 1;
    return k;
}

int is_power_of_two(int n)
{
    return n > 0 && (n & (n - 1)) == 0;
}
