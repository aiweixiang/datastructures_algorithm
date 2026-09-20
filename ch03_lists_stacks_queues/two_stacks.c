/* Exercise 3.24: two stacks in one array, growing towards each other.
 * An overflow is reported only when *every* slot is occupied.
 * Caveat (worth stating in the answer): we must know the array size in
 * advance, because a real overflow now means the whole array is full.
 */
#include <stdlib.h>
#include "ch03.h"

TwoStacks *ts_create(int capacity)
{
    if (capacity < 1)
        capacity = 1;
    TwoStacks *t = malloc(sizeof *t);
    if (t == NULL)
        return NULL;
    t->data = malloc((size_t)capacity * sizeof *t->data);
    if (t->data == NULL) {
        free(t);
        return NULL;
    }
    t->capacity = capacity;
    t->top_left = -1;
    t->top_right = capacity;
    return t;
}

void ts_dispose(TwoStacks *t)
{
    if (t == NULL) return;
    free(t->data);
    free(t);
}

int ts_is_full(const TwoStacks *t)
{
    return t == NULL || t->top_left + 1 == t->top_right;
}

int ts_is_empty(int which, const TwoStacks *t)
{
    if (t == NULL) return 1;
    return which == 0 ? t->top_left < 0 : t->top_right == t->capacity;
}

int ts_size(int which, const TwoStacks *t)
{
    if (t == NULL) return 0;
    return which == 0 ? t->top_left + 1 : t->capacity - t->top_right;
}

int ts_push(int which, element_type x, TwoStacks *t)
{
    if (t == NULL || ts_is_full(t))
        return -1;
    if (which == 0)
        t->data[++t->top_left] = x;
    else
        t->data[--t->top_right] = x;
    return 0;
}

int ts_pop(int which, TwoStacks *t, element_type *out)
{
    if (t == NULL || ts_is_empty(which, t))
        return -1;
    if (which == 0)
        *out = t->data[t->top_left--];
    else
        *out = t->data[t->top_right++];
    return 0;
}
