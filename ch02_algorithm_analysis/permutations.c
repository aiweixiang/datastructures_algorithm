/* 习题 2.7：三种生成随机置换的算法（外加一种常见错误做法）。
 *
 *  A 拒绝采样：反复随机取数直到取到没用过的。均匀，期望 O(N log N)。
 *  B 与任意位置交换：i 从 0 到 N-1，每次与 [0, N) 中随机位置交换。
 *    O(N)，但**不均匀**！共有 N^N 条等概率的选择序列，要均匀就得让
 *    N! 整除 N^N；N = 3 时 6 ∤ 27，已经不可能。经典的"朴素洗牌陷阱"。
 *  C Fisher–Yates：i 从 0 到 N-1，每次与 [i, N) 中随机位置交换。均匀，O(N)。
 *  D 直接赋值：a[i] = random(N)。**不是置换**（有重复、有缺失）。
 *
 * 均匀性证明、B 的精确分布（3/27 种置换为 4/27、另三种为 5/27）与期望代价
 * 推导见 ANSWERS.md 2.7；tests/test_ch02.c 既做了统计检验，也对 B 做了
 * 3^3 = 27 条序列的**精确枚举**验证。
 */
#include <stdlib.h>
#include "ch02.h"

void perm_identity(int a[], int n)
{
    for (int i = 0; i < n; ++i)
        a[i] = i;
}

int perm_rejection(int a[], int n, unsigned long *state)
{
    if (a == NULL || n < 0)
        return -1;
    if (n == 0)
        return 0;
    unsigned char *used = calloc((size_t)n, 1);
    if (used == NULL)
        return -1;

    for (int i = 0; i < n; ++i) {
        int v;
        do {
            v = rng_uniform(state, n);
        } while (used[v]);
        used[v] = 1;
        a[i] = v;
    }
    free(used);
    return 0;
}

/* 算法 B：每次都在**全体**位置里挑一个来换。
 * 注意：它**不是**均匀的。总选择序列数为 N^N，每一个等概率，
 * 所以每个置换的概率必然是 1/N^N 的整数倍；要均匀就需要 N! | N^N，
 * 而 N = 3 时 27 不是 6 的倍数，N = 4 时 256 不是 24 的倍数……
 * 代码保留在这里，正是为了用测试把这个经典错误钉住。 */
void perm_swap_any(int a[], int n, unsigned long *state)
{
    perm_identity(a, n);
    for (int i = 0; i < n; ++i) {
        int j = rng_uniform(state, n);
        int t = a[i]; a[i] = a[j]; a[j] = t;
    }
}

/* 算法 C：Fisher–Yates。均匀性有最干净的证明：
 * 第 i 步从 n-i 个候选里选，总选择数 = n·(n-1)·…·1 = n!，
 * 而每一步的选择序列与最终置换一一对应（双射），故每个置换等概率。 */
void perm_fisher_yates(int a[], int n, unsigned long *state)
{
    perm_identity(a, n);
    for (int i = 0; i < n; ++i) {
        int j = i + rng_uniform(state, n - i);
        int t = a[i]; a[i] = a[j]; a[j] = t;
    }
}

void perm_naive_random(int a[], int n, unsigned long *state)
{
    for (int i = 0; i < n; ++i)
        a[i] = rng_uniform(state, n);
}
