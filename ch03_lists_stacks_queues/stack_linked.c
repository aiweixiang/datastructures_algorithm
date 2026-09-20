/* Chapter 3: stack as a singly linked list.  Every operation is O(1) and
 * there is no fixed capacity, at the cost of one pointer per element.
 */
#include <stdlib.h>
#include "ch03.h"

struct StackLinked {
    PtrToNode top;
};

StackLinked *stkl_create(void)
{
    StackLinked *s = malloc(sizeof *s);
    if (s == NULL)
        return NULL;
    s->top = NULL;
    return s;
}

void stkl_make_empty(StackLinked *s)
{
    if (s == NULL)
        return;
    while (s->top != NULL) {
        PtrToNode next = s->top->next;
        free(s->top);
        s->top = next;
    }
}

void stkl_dispose(StackLinked *s)
{
    if (s == NULL) return;
    stkl_make_empty(s);
    free(s);
}

int stkl_is_empty(const StackLinked *s) { return s == NULL || s->top == NULL; }

void stkl_push(element_type x, StackLinked *s)
{
    if (s == NULL)
        return;
    PtrToNode n = malloc(sizeof *n);
    if (n == NULL)
        return;                        /* silently drops on OOM (see note) */
    n->elem = x;
    n->next = s->top;
    s->top = n;
}

element_type stkl_top(const StackLinked *s)
{
    if (s == NULL || s->top == NULL)
        return ELEMENT_NOT_FOUND;
    return s->top->elem;
}

element_type stkl_pop(StackLinked *s)
{
    if (s == NULL || s->top == NULL)
        return ELEMENT_NOT_FOUND;
    PtrToNode n = s->top;
    element_type v = n->elem;
    s->top = n->next;
    free(n);
    return v;
}
