/* Chapter 3: stack with a growing array.  Doubling the capacity keeps Push
 * O(1) amortized even though an individual Push may cost O(N).
 */
#include <stdlib.h>
#include "ch03.h"

struct StackArray {
    int           capacity;
    int           top;      /* index of the top element, -1 when empty */
    element_type *data;
};

StackArray *stk_create(int init_capacity)
{
    if (init_capacity < 1)
        init_capacity = 1;
    StackArray *s = malloc(sizeof *s);
    if (s == NULL)
        return NULL;
    s->data = malloc((size_t)init_capacity * sizeof *s->data);
    if (s->data == NULL) {
        free(s);
        return NULL;
    }
    s->capacity = init_capacity;
    s->top = -1;
    return s;
}

void stk_dispose(StackArray *s)
{
    if (s == NULL) return;
    free(s->data);
    free(s);
}

void stk_make_empty(StackArray *s) { if (s) s->top = -1; }
int  stk_is_empty(const StackArray *s) { return s == NULL || s->top < 0; }
int  stk_capacity(const StackArray *s) { return s ? s->capacity : 0; }
int  stk_size(const StackArray *s) { return s ? s->top + 1 : 0; }

int stk_push(element_type x, StackArray *s)
{
    if (s == NULL)
        return -1;
    if (s->top + 1 == s->capacity) {
        int new_cap = s->capacity * 2;
        element_type *p = realloc(s->data, (size_t)new_cap * sizeof *p);
        if (p == NULL)
            return -1;
        s->data = p;
        s->capacity = new_cap;
    }
    s->data[++s->top] = x;
    return 0;
}

element_type stk_top(const StackArray *s)
{
    if (s == NULL || s->top < 0)
        return ELEMENT_NOT_FOUND;
    return s->data[s->top];
}

element_type stk_pop(StackArray *s)
{
    if (s == NULL || s->top < 0)
        return ELEMENT_NOT_FOUND;
    return s->data[s->top--];
}
