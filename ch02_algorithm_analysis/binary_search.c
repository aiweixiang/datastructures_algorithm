/* Chapter 2: binary search, including the overflow-safe midpoint and the
 * lower/upper bound variants used by the standard library.
 * Precondition for every routine: a[0..n-1] is sorted in non-decreasing order.
 */
#include "ch02.h"

int bisect(const int a[], int x, int n)
{
    int low = 0, high = n - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;   /* avoids low + high overflow */
        if (a[mid] < x)
            low = mid + 1;
        else if (a[mid] > x)
            high = mid - 1;
        else
            return mid;
    }
    return BISECT_NOT_FOUND;
}

int bisect_recursive(const int a[], int x, int lo, int hi)
{
    if (lo > hi)
        return BISECT_NOT_FOUND;
    int mid = lo + (hi - lo) / 2;
    if (a[mid] < x)
        return bisect_recursive(a, x, mid + 1, hi);
    if (a[mid] > x)
        return bisect_recursive(a, x, lo, mid - 1);
    return mid;
}

int lower_bound_idx(const int a[], int x, int n)
{
    int lo = 0, hi = n;                 /* answer in [0, n] */
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] < x)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}

int upper_bound_idx(const int a[], int x, int n)
{
    int lo = 0, hi = n;
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] <= x)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}

/* 习题 2.23：每次迭代只做**一次**比较的二分查找。
 * 经典写法在循环内可能比较两次（先比 < 再比 >）。改成"只判断 a[mid] < x"
 * 之后，循环内每轮恰好一次比较，代价是循环结束后需要一次确认比较：
 *   循环比较次数 ≤ ⌈log2 N⌉，总计 ≤ ⌈log2 N⌉ + 1，
 * 而经典写法最坏约 2⌊log2 N⌋ + 2 —— 几乎省一半。
 * 语义上返回的是"第一个等于 x 的位置"（即 lower_bound 一旦命中）。 */
int bisect_one_compare_count(const int a[], int x, int n, int *comparisons)
{
    int cmps = 0;
    int low = 0, high = n - 1;

    while (low < high) {
        int mid = low + (high - low) / 2;
        ++cmps;                          /* 循环体内唯一的一次比较 */
        if (a[mid] < x)
            low = mid + 1;
        else
            high = mid;
    }

    if (n <= 0) {
        if (comparisons != NULL)
            *comparisons = cmps;
        return BISECT_NOT_FOUND;
    }
    ++cmps;                              /* 结束后的确认比较 */
    if (comparisons != NULL)
        *comparisons = cmps;
    return (a[low] == x) ? low : BISECT_NOT_FOUND;
}

int bisect_one_compare(const int a[], int x, int n)
{
    return bisect_one_compare_count(a, x, n, NULL);
}
