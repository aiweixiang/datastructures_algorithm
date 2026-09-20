/* 习题 3.2：PrintLots(L, P) —— 按 P 中给出的位置打印 L 中的元素。
 *
 * 两个版本：
 *   print_lots_simple：对 P 的每个位置各自走一遍链表，O(|P|·|L|)。
 *   print_lots_linear：L 与 P 同步单遍推进（要求 P 已按升序排好），O(|L| + |P|)。
 * 位置约定为 **1 起始**（与题意一致）；越界的位置被跳过并计数返回。
 */
#include "ch03.h"

int print_lots_simple(const ListLinked *l, const int positions[], int npos,
                      FILE *fp, int *skipped)
{
    int skip = 0;
    if (skipped != NULL)
        *skipped = 0;
    if (l == NULL || positions == NULL || fp == NULL)
        return -1;

    int printed = 0;
    for (int i = 0; i < npos; ++i) {
        PtrToNode p = lil_find_kth(positions[i], l);      /* 1 起始 */
        if (p == NULL) {
            ++skip;
            continue;
        }
        if (printed > 0)
            fprintf(fp, " ");
        fprintf(fp, "%d", p->elem);
        ++printed;
    }
    if (skipped != NULL)
        *skipped = skip;
    return 0;
}

/* 线性版本：P 必须升序。用一个游标沿着 L 前进，指针总体只向前走一次。 */
int print_lots_linear(const ListLinked *l, const int positions[], int npos,
                      FILE *fp, int *skipped)
{
    int skip = 0, printed = 0;
    if (skipped != NULL)
        *skipped = 0;
    if (l == NULL || positions == NULL || fp == NULL)
        return -1;

    PtrToNode cur = l->head->next;
    int index = 1;                                        /* cur 的 1 起始位置 */
    for (int i = 0; i < npos; ++i) {
        if (positions[i] < index) {                       /* 位置已越过：不合法 */
            ++skip;
            continue;
        }
        while (cur != NULL && index < positions[i]) {
            cur = cur->next;
            ++index;
        }
        if (cur == NULL) {                                /* 后面全部越界 */
            skip += npos - i;
            break;
        }
        if (printed > 0)
            fprintf(fp, " ");
        fprintf(fp, "%d", cur->elem);
        ++printed;
    }
    if (skipped != NULL)
        *skipped = skip;
    return 0;
}
