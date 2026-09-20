/* 习题 2.15 / 2.16 / 2.17：快速求幂的乘法次数。
 *
 * 2.15  x^62 用 8 次乘法：
 *       x2 = x*x            (1)  x^2
 *       x3 = x2*x           (2)  x^3
 *       x5 = x3*x2          (3)  x^5
 *       x10 = x5*x5         (4)  x^10
 *       x20 = x10*x10       (5)  x^20
 *       x30 = x20*x10       (6)  x^30
 *       x31 = x30*x         (7)  x^31
 *       x62 = x31*x31       (8)  x^62
 *       一般下界：m 次乘法最多把指数翻到 2^m，故 2^m ≥ 62 ⇒ m ≥ 6。
 *
 * 2.16  非递归（迭代）二进制幂：按 N 的二进制位处理。
 *
 * 2.17  精确乘法次数。采用"跳过最后一次无用平方"的实现约定：
 *       次数 = ⌊log2 N⌋ + popcount(N)
 *       （⌊log2 N⌋ 次平方 + popcount(N) 次"乘 x"；也可以把最高位那次
 *        省掉，得到 ⌊log2 N⌋ + popcount(N) − 1，两种约定只差最高位。）
 *       N = 62 时：⌊log2 62⌋ + popcount(62) = 5 + 5 = 10 次，
 *       而最优加法链只要 8 次 —— 说明二进制幂不总是最优。
 */
#include "ch02.h"

long long pow_binary_count(long long x, int n, int *mults)
{
    long long result = 1;
    int m = 0;
    while (n > 0) {
        if (n & 1) {
            result *= x;
            ++m;
        }
        n >>= 1;
        if (n > 0) {           /* 不再做无用的最后一次平方 */
            x *= x;
            ++m;
        }
    }
    if (mults != NULL)
        *mults = m;
    return result;
}

int binary_pow_mult_count(int n)
{
    if (n <= 0)
        return 0;
    int floor_log2 = log2_floor_bits(n);
    int popcount = 0;
    for (unsigned v = (unsigned)n; v != 0; v >>= 1)
        popcount += (int)(v & 1u);
    return floor_log2 + popcount;
}

long long pow62_optimal(long long x)
{
    long long x2  = x * x;          /* 1 */
    long long x3  = x2 * x;         /* 2 */
    long long x5  = x3 * x2;        /* 3 */
    long long x10 = x5 * x5;        /* 4 */
    long long x20 = x10 * x10;      /* 5 */
    long long x30 = x20 * x10;      /* 6 */
    long long x31 = x30 * x;        /* 7 */
    return x31 * x31;               /* 8 → x^62 */
}

int pow62_optimal_mults(void)
{
    return 8;
}
