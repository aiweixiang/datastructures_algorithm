/* 习题 2.12：最小子序列和 / 最小正子序列和 / 最大子序列乘积。
 * 习题 2.25：立方算法与平方算法的迭代次数。
 *
 * 约定：子序列都指**连续**的一段；除特别说明外允许空段（和为 0）。
 */
#include <limits.h>
#include "ch02.h"

/* 允许空子序列 → 结果 ≤ 0。把"最大子序列和"的判据整体反向即可。 */
int mss_min(const int a[], int n)
{
    int min_sum = 0, this_sum = 0;
    for (int i = 0; i < n; ++i) {
        this_sum += a[i];
        if (this_sum < min_sum)
            min_sum = this_sum;
        else if (this_sum > 0)
            this_sum = 0;      /* 正前缀只会拖累"最小"，直接丢弃 */
    }
    return min_sum;
}

/* 不允许空段：dp[i] = min(a[i], dp[i-1] + a[i])。 */
int mss_min_nonempty(const int a[], int n)
{
    if (a == NULL || n <= 0)
        return 0;
    int best = a[0], cur = a[0];
    for (int i = 1; i < n; ++i) {
        cur = (a[i] < cur + a[i]) ? a[i] : cur + a[i];
        if (cur < best)
            best = cur;
    }
    return best;
}

/* 最小**正**子序列和：O(N^2) 直接枚举。
 * O(N log N) 的做法：令 prefix[j] = a[0..j-1] 之和，答案 =
 *   min{ prefix[j] - prefix[i] > 0 : i < j }
 * 对每个 j 只需在"之前出现过的所有前缀和"里找**小于 prefix[j] 的最大者**，
 * 这需要一个有序集合（平衡树，第 4 章；或 treap，第 12 章）；
 * 用有序数组 + 二分插入也能做到 O(N log N) 的比较次数，但插入是 O(N)。
 * 返回 0 表示不存在正的和。 */
int mss_min_positive(const int a[], int n)
{
    long long best = 0;
    for (int i = 0; i < n; ++i) {
        long long s = 0;
        for (int j = i; j < n; ++j) {
            s += a[j];
            if (s > 0 && (best == 0 || s < best))
                best = s;
        }
    }
    return (int)best;
}

/* 最大子序列乘积（非空连续）。同时维护"最大"和"最小"两个候选：
 * 因为负数会把最小值翻成最大值。 */
long long mss_max_product(const int a[], int n)
{
    if (a == NULL || n <= 0)
        return 0;
    long long best = a[0], cur_max = a[0], cur_min = a[0];
    for (int i = 1; i < n; ++i) {
        long long x = a[i];
        long long t1 = cur_max * x, t2 = cur_min * x;
        long long nmax = x > t1 ? x : t1;
        if (t2 > nmax) nmax = t2;
        long long nmin = x < t1 ? x : t1;
        if (t2 < nmin) nmin = t2;
        cur_max = nmax;
        cur_min = nmin;
        if (cur_max > best)
            best = cur_max;
    }
    return best;
}

/* 习题 2.25：显式数出内层语句执行的次数。
 * 立方版本：Σ_{i} Σ_{j≥i} (j-i+1) = N(N+1)(N+2)/6
 * 平方版本：Σ_{i} (N-i)          = N(N+1)/2
 * 两者之比 ≈ N/3，这就是"为什么立方版本慢得多"的定量答案。 */
long long mss_cubic_iterations(int n)
{
    long long count = 0;
    for (int i = 0; i < n; ++i)
        for (int j = i; j < n; ++j)
            for (int k = i; k <= j; ++k)
                ++count;
    return count;
}

long long mss_quadratic_iterations(int n)
{
    long long count = 0;
    for (int i = 0; i < n; ++i)
        for (int j = i; j < n; ++j)
            ++count;
    return count;
}
