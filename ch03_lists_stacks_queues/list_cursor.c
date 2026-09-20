/* Chapter 3: cursor implementation of a linked list (Weiss's curslist).
 * Used when pointers are unavailable or expensive; every node lives in a
 * global-ish array `space`, and `space[0]` heads the free list.  A "pointer"
 * is just an int index, CURSOR_NULL plays the role of NULL.
 */
#include <stdlib.h>
#include "ch03.h"

static int lcs_alloc(ListCursor *l)
{
    int p = l->space[0].next;
    if (p != CURSOR_NULL)
        l->space[0].next = l->space[p].next;
    return p;
}

static void lcs_free(ListCursor *l, int p)
{
    l->space[p].next = l->space[0].next;
    l->space[0].next = p;
}

ListCursor *lcs_create(int capacity)
{
    if (capacity < 2)
        capacity = 2;
    ListCursor *l = malloc(sizeof *l);
    if (l == NULL)
        return NULL;
    l->space = malloc((size_t)(capacity + 1) * sizeof *l->space);
    if (l->space == NULL) {
        free(l);
        return NULL;
    }
    l->capacity = capacity;
    /* fill the free list: 1 -> 2 -> ... -> capacity -> NULL */
    l->space[0].next = 1;
    for (int i = 1; i <= capacity; ++i)
        l->space[i].next = (i == capacity) ? CURSOR_NULL : i + 1;

    l->header = lcs_alloc(l);            /* allocate this list's header */
    l->space[l->header].next = CURSOR_NULL;
    l->size = 0;
    return l;
}

void lcs_dispose(ListCursor *l)
{
    if (l == NULL)
        return;
    free(l->space);
    free(l);
}

void lcs_make_empty(ListCursor *l)
{
    if (l == NULL)
        return;
    int p = l->space[l->header].next;
    while (p != CURSOR_NULL) {
        int next = l->space[p].next;
        lcs_free(l, p);
        p = next;
    }
    l->space[l->header].next = CURSOR_NULL;
    l->size = 0;
}

int lcs_is_empty(const ListCursor *l)
{
    return l == NULL || l->space[l->header].next == CURSOR_NULL;
}

int lcs_size(const ListCursor *l) { return l ? l->size : 0; }

int lcs_find(element_type x, const ListCursor *l)
{
    if (l == NULL)
        return ELEMENT_NOT_FOUND;
    for (int p = l->space[l->header].next; p != CURSOR_NULL; p = l->space[p].next)
        if (l->space[p].elem == x)
            return p;
    return ELEMENT_NOT_FOUND;
}

int lcs_find_kth(int k, const ListCursor *l)
{
    if (l == NULL || k < 1)
        return ELEMENT_NOT_FOUND;
    int p = l->space[l->header].next;
    for (int i = 1; i < k && p != CURSOR_NULL; ++i)
        p = l->space[p].next;
    return p;
}

element_type lcs_at(int index, const ListCursor *l)
{
    int p = lcs_find_kth(index + 1, l);
    return p == CURSOR_NULL ? ELEMENT_NOT_FOUND : l->space[p].elem;
}

int lcs_insert(element_type x, int index, ListCursor *l)
{
    if (l == NULL || index < 0 || index > l->size)
        return -1;
    int prev = l->header;
    for (int i = 0; i < index; ++i)
        prev = l->space[prev].next;

    int node = lcs_alloc(l);
    if (node == CURSOR_NULL)
        return -1;                       /* out of space: a real overflow */
    l->space[node].elem = x;
    l->space[node].next = l->space[prev].next;
    l->space[prev].next = node;
    ++l->size;
    return 0;
}

int lcs_delete(element_type x, ListCursor *l)
{
    if (l == NULL)
        return -1;
    int prev = l->header;
    int p = l->space[prev].next;
    while (p != CURSOR_NULL && l->space[p].elem != x) {
        prev = p;
        p = l->space[p].next;
    }
    if (p == CURSOR_NULL)
        return -1;
    l->space[prev].next = l->space[p].next;
    lcs_free(l, p);
    --l->size;
    return 0;
}

void lcs_print(const ListCursor *l, FILE *fp)
{
    fprintf(fp, "(");
    if (l != NULL) {
        for (int p = l->space[l->header].next; p != CURSOR_NULL; p = l->space[p].next) {
            fprintf(fp, "%d", l->space[p].elem);
            if (l->space[p].next != CURSOR_NULL)
                fprintf(fp, ", ");
        }
    }
    fprintf(fp, ")");
}
