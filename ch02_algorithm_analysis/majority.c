/* 习题 2.19：寻找出现次数超过 N/2 的主要元素。
 *
 * Boyer–Moore 投票算法：O(N) 时间、O(1) 空间。
 * 直觉：把"多数元素"与"其他元素"两两抵消，剩下的候选者只能是多数元素
 * （若存在）；但因为也可能**不存在**多数元素，所以候选者必须再扫一遍验证。
 *
 * 另一种答案：排序后取中间元素再验证，O(N log N)；
 * 投票法胜在只需一遍扫描且空间 O(1)。
 */
#include "ch02.h"

int majority_element(const int a[], int n, int *found)
{
    if (found != NULL)
        *found = 0;
    if (a == NULL || n <= 0)
        return 0;

    int candidate = a[0], count = 0;
    for (int i = 0; i < n; ++i) {
        if (count == 0) {
            candidate = a[i];
            count = 1;
        } else if (a[i] == candidate) {
            ++count;
        } else {
            --count;                       /* 抵消一对 */
        }
    }

    int occ = 0;                            /* 必要：验证候选者 */
    for (int i = 0; i < n; ++i)
        if (a[i] == candidate)
            ++occ;

    if (occ > n / 2) {
        if (found != NULL)
            *found = 1;
        return candidate;
    }
    return 0;
}
