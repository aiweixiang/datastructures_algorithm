/* 习题 1.1：为选择问题（求第 k 大，取 k = N/2）画出运行时间表。
 *
 *   make demos && ./build/selection_timing
 *
 * 三种方法：排序、大小为 k 的最小堆、快速选择。
 * 注意：
 *   - 三种方法必须作用在**同一份输入**上，否则结果不同纯属输入不同（下面的
 *     程序会校验三个答案一致，并把这一步作为计时正确性的自检）；
 *   - "排序"这里用 qsort 而不是 kth_largest_sort（后者是插入排序 O(N²)，
 *     适合演示算法思想，不适合大 N 计时）。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ch02_algorithm_analysis/ch02.h"

static unsigned long seed = 12345UL;

static int next_rand(void)
{
    seed = seed * 1103515245UL + 12345UL;
    return (int)((seed >> 16) & 0x7fff);
}

static void fill_random(int a[], int n)
{
    for (int i = 0; i < n; ++i)
        a[i] = next_rand();
}

static int cmp_desc(const void *pa, const void *pb)
{
    int a = *(const int *)pa, b = *(const int *)pb;
    return (a < b) - (a > b);
}

static int kth_by_sort(const int src[], int n, int k)
{
    int *copy = malloc((size_t)n * sizeof *copy);
    if (copy == NULL)
        exit(1);
    memcpy(copy, src, (size_t)n * sizeof *copy);
    qsort(copy, (size_t)n, sizeof *copy, cmp_desc);
    int v = copy[k - 1];
    free(copy);
    return v;
}

static int kth_by_select(const int src[], int n, int k)
{
    int *copy = malloc((size_t)n * sizeof *copy);
    if (copy == NULL)
        exit(1);
    memcpy(copy, src, (size_t)n * sizeof *copy);
    int v = kth_largest_quickselect(copy, n, k);
    free(copy);
    return v;
}

static double ms_between(clock_t t0, clock_t t1)
{
    return (double)(t1 - t0) * 1000.0 / CLOCKS_PER_SEC;
}

int main(void)
{
    const int sizes[] = {10000, 40000, 160000, 640000};
    printf("%9s %12s %12s %12s\n", "N", "sort(ms)", "heap(ms)", "select(ms)");

    for (size_t i = 0; i < sizeof sizes / sizeof sizes[0]; ++i) {
        int n = sizes[i], k = n / 2;
        int *base = malloc((size_t)n * sizeof *base);
        if (base == NULL)
            exit(1);
        fill_random(base, n);

        clock_t t0 = clock();
        int a1 = kth_by_sort(base, n, k);
        clock_t t1 = clock();
        int a2 = kth_largest_heap(base, n, k);      /* 不修改输入 */
        clock_t t2 = clock();
        int a3 = kth_by_select(base, n, k);
        clock_t t3 = clock();

        printf("%9d %12.3f %12.3f %12.3f   (三法结果一致: %s)\n",
               n, ms_between(t0, t1), ms_between(t1, t2), ms_between(t2, t3),
               (a1 == a2 && a2 == a3) ? "yes" : "NO!");
        free(base);
    }
    return 0;
}
