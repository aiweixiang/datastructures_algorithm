/* 习题「带 FindMin 的栈」（英文 2e 编号 3.25 a / 你的版本编号 3.22 a）
 * 要求：Push、Pop、FindMin 三个操作全部 O(1)。b 部分的下界证明见 ANSWERS.md。
 *
 * 关键观察：栈是 LIFO，"当前栈"永远构成"从栈底开始的一条前缀"。
 * 于是每个结点只要记住「从栈底到本结点为止的最小值」，Pop 时该前缀最小值
 * 自动回退，FindMin 不需要任何扫描或比较。
 *
 * 另一种等价实现是维护一个辅助最小值栈（见 ANSWERS.md 的讨论）。
 */
#include <stdlib.h>
#include "ch03.h"

typedef struct MinNode {
    element_type    elem;
    element_type    min_below;   /* 从栈底到本结点（含）的最小值 */
    struct MinNode *next;
} MinNode;

struct MinStack {
    MinNode *top;
    int      size;
};

MinStack *mstk_create(void)
{
    MinStack *s = malloc(sizeof *s);
    if (s == NULL)
        return NULL;
    s->top = NULL;
    s->size = 0;
    return s;
}

void mstk_dispose(MinStack *s)
{
    if (s == NULL)
        return;
    MinNode *p = s->top;
    while (p != NULL) {
        MinNode *next = p->next;
        free(p);
        p = next;
    }
    free(s);
}

int mstk_push(element_type x, MinStack *s)
{
    if (s == NULL)
        return -1;
    MinNode *n = malloc(sizeof *n);
    if (n == NULL)
        return -1;
    n->elem = x;
    n->min_below = (s->top == NULL || x < s->top->min_below)
                       ? x : s->top->min_below;
    n->next = s->top;
    s->top = n;
    ++s->size;
    return 0;
}

element_type mstk_pop(MinStack *s)
{
    if (s == NULL || s->top == NULL)
        return ELEMENT_NOT_FOUND;
    MinNode *n = s->top;
    element_type v = n->elem;
    s->top = n->next;
    free(n);
    --s->size;
    return v;
}

element_type mstk_find_min(const MinStack *s)
{
    if (s == NULL || s->top == NULL)
        return ELEMENT_NOT_FOUND;
    return s->top->min_below;    /* O(1)：答案已在栈顶缓存 */
}

int mstk_size(const MinStack *s) { return s ? s->size : 0; }
int mstk_is_empty(const MinStack *s) { return s == NULL || s->top == NULL; }
