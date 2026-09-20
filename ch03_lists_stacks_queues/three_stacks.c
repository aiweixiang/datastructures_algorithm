/* 习题 3.23：用一个数组实现三个栈。
 *
 * 做法：把数组三等分，第 k 个栈只能用第 k 段（base[k] .. end[k)-1）。
 *   base[k] = k*capacity/3，end[k] = (k+1)*capacity/3，top[k] = base[k]-1。
 * 溢出条件：某个栈的 top+1 == end，**即使其他两段还有大量空位**。
 *
 * 为什么做不到两栈那样"只有全部槽位用满才溢出"：
 *   两栈时两个栈分别从两端向中间长，任何时刻两栈的元素总数就是占用槽位数，
 *   所以"总满"与"某个栈满"等价。三个栈时中间那个栈的"生长方向"两侧都被夹住，
 *   除非允许在数组内部搬移元素（那就不是 O(1) 了），否则无法让三个栈共享全部空隙。
 * 因此本题的标准答案就是"等分三段"，代价是可能出现"虚假的满"。
 */
#include <stdlib.h>
#include "ch03.h"

struct ThreeStacks {
    element_type *data;
    int           capacity;
    int           base[3];
    int           end[3];
    int           top[3];
};

static void ts3_layout(ThreeStacks *t, int capacity)
{
    for (int k = 0; k < 3; ++k) {
        t->base[k] = k * capacity / 3;
        t->end[k] = (k + 1) * capacity / 3;
        t->top[k] = t->base[k] - 1;
    }
}

ThreeStacks *ts3_create(int capacity)
{
    if (capacity < 3)
        capacity = 3;
    ThreeStacks *t = malloc(sizeof *t);
    if (t == NULL)
        return NULL;
    t->data = malloc((size_t)capacity * sizeof *t->data);
    if (t->data == NULL) {
        free(t);
        return NULL;
    }
    t->capacity = capacity;
    ts3_layout(t, capacity);
    return t;
}

void ts3_dispose(ThreeStacks *t)
{
    if (t == NULL)
        return;
    free(t->data);
    free(t);
}

int ts3_capacity_share(const ThreeStacks *t, int which)
{
    if (t == NULL || which < 0 || which > 2)
        return 0;
    return t->end[which] - t->base[which];
}

int ts3_is_empty(int which, const ThreeStacks *t)
{
    if (t == NULL || which < 0 || which > 2)
        return 1;
    return t->top[which] < t->base[which];
}

int ts3_is_full(int which, const ThreeStacks *t)
{
    if (t == NULL || which < 0 || which > 2)
        return 1;
    return t->top[which] + 1 >= t->end[which];
}

int ts3_size(int which, const ThreeStacks *t)
{
    if (t == NULL || which < 0 || which > 2)
        return 0;
    return t->top[which] - t->base[which] + 1;
}

int ts3_push(int which, element_type x, ThreeStacks *t)
{
    if (t == NULL || which < 0 || which > 2 || ts3_is_full(which, t))
        return -1;
    t->data[++t->top[which]] = x;
    return 0;
}

int ts3_pop(int which, ThreeStacks *t, element_type *out)
{
    if (t == NULL || which < 0 || which > 2 || ts3_is_empty(which, t))
        return -1;
    if (out != NULL)
        *out = t->data[t->top[which]];
    --t->top[which];
    return 0;
}
