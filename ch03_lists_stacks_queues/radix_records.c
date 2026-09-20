/* 习题 3.13：用基数排序（LSD，十进制桶）排序学生记录。
 *
 * 每趟按一位数字做**稳定**的计数排序；P 趟之后整体有序。
 * 时间 O(P·(N + 10))，其中 P 是最大关键字的位数。P 是常数时是线性时间。
 * 空间 O(N + 10)。
 *
 * 关键性质：LSD 基数排序的正确性**依赖每趟排序的稳定性**——
 * 低位相同时，先排好的高位顺序必须被保留。测试专门验证了稳定性。
 */
#include <stdlib.h>
#include <string.h>
#include "ch03.h"

static void radix_pass(StudentRecord *src, StudentRecord *dst, int n, int exp)
{
    int count[10] = {0};
    for (int i = 0; i < n; ++i)
        ++count[(src[i].key / exp) % 10];
    for (int d = 1; d < 10; ++d)
        count[d] += count[d - 1];            /* 前缀和 → 结束位置 */
    for (int i = n - 1; i >= 0; --i) {       /* 从后向前保证稳定 */
        int d = (src[i].key / exp) % 10;
        dst[--count[d]] = src[i];
    }
}

void radix_sort_records(StudentRecord a[], int n)
{
    if (a == NULL || n < 2)
        return;
    int max_key = a[0].key;
    for (int i = 1; i < n; ++i)
        if (a[i].key > max_key)
            max_key = a[i].key;

    StudentRecord *tmp = malloc((size_t)n * sizeof *tmp);
    if (tmp == NULL)
        return;

    StudentRecord *src = a, *dst = tmp;
    for (int exp = 1; max_key / exp > 0; exp *= 10) {
        radix_pass(src, dst, n, exp);
        StudentRecord *t = src; src = dst; dst = t;   /* 交换缓冲 */
    }
    if (src != a)                                  /* 奇数趟：结果在 tmp 里 */
        memcpy(a, src, (size_t)n * sizeof *a);
    free(tmp);
}
