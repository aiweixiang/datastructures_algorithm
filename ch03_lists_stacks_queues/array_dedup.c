/* 习题 3.16：删除数组中的重复元素。
 *
 *   slow 版本：O(N^2)，用"前面是否出现过"逐个检查；**保持首次出现的相对顺序**。
 *   sort 版本：O(N log N)，先排序再去重；顺序被破坏（但通常正是我们想要的）。
 *   若允许 O(N) 额外空间：用散列（第 5 章）可以做到 O(N) 且保持顺序。
 * 均返回去重后的元素个数，结果写回 a[0..返回值-1]。
 */
#include <stdlib.h>
#include "ch03.h"

int remove_duplicates_slow(int a[], int n)
{
    if (a == NULL || n <= 0)
        return 0;
    int m = 0;
    for (int i = 0; i < n; ++i) {
        int seen = 0;
        for (int j = 0; j < m; ++j) {
            if (a[j] == a[i]) {
                seen = 1;
                break;
            }
        }
        if (!seen)
            a[m++] = a[i];
    }
    return m;
}

static int cmp_int(const void *pa, const void *pb)
{
    int x = *(const int *)pa, y = *(const int *)pb;
    return (x > y) - (x < y);
}

int remove_duplicates_sort(int a[], int n)
{
    if (a == NULL || n <= 0)
        return 0;
    qsort(a, (size_t)n, sizeof *a, cmp_int);
    int m = 1;
    for (int i = 1; i < n; ++i)
        if (a[i] != a[m - 1])
            a[m++] = a[i];
    return m;
}
