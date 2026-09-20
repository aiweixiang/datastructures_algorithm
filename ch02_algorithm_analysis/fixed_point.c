/* 习题 2.11：判断有序数组中是否存在 A[i] == i（不动点）。
 *
 * 关键不变量：数组严格递增时，f(i) = A[i] - i 是**非减**的
 * （因为 A[i+1] > A[i] ⇒ A[i+1] - (i+1) >= A[i] - i）。
 * 于是可以二分：
 *   A[mid] < mid  → 左侧不可能有不动点，向右找
 *   A[mid] > mid  → 右侧不可能有不动点，向左找
 * 时间 O(log N)，空间 O(1)。
 */
#include "ch02.h"

int fixed_point(const int a[], int n)
{
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] == mid)
            return mid;
        if (a[mid] < mid)
            lo = mid + 1;
        else
            hi = mid - 1;
    }
    return BISECT_NOT_FOUND;
}

/* 题目常见的 1 起始写法：求 i 使得 A[i] == i，其中 i ∈ [1, N]。
 * 等价于在 0 起始下标里比较 a[mid] == mid + 1。返回 1 起始位置。 */
int fixed_point_one_based(const int a[], int n)
{
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] == mid + 1)
            return mid + 1;
        if (a[mid] < mid + 1)
            lo = mid + 1;
        else
            hi = mid - 1;
    }
    return BISECT_NOT_FOUND;
}
