/* Chapter 3: the List ADT implemented with a growable array.
 * Find/FindKth are O(1); Insert/Delete cost O(N) because elements shift.
 */
#include <stdlib.h>
#include "ch03.h"

struct ListArray {
    int           capacity;
    int           size;
    element_type *data;
};

ListArray *lst_create(int init_capacity)
{
    if (init_capacity < 1)
        init_capacity = 1;
    ListArray *l = malloc(sizeof *l);
    if (l == NULL)
        return NULL;
    l->data = malloc((size_t)init_capacity * sizeof *l->data);
    if (l->data == NULL) {
        free(l);
        return NULL;
    }
    l->capacity = init_capacity;
    l->size = 0;
    return l;
}

void lst_dispose(ListArray *l)
{
    if (l == NULL)
        return;
    free(l->data);
    free(l);
}

void lst_make_empty(ListArray *l) { if (l) l->size = 0; }
int  lst_is_empty(const ListArray *l) { return l == NULL || l->size == 0; }
int  lst_size(const ListArray *l) { return l ? l->size : 0; }
int  lst_capacity(const ListArray *l) { return l ? l->capacity : 0; }

int lst_find(element_type x, const ListArray *l)
{
    if (l == NULL)
        return ELEMENT_NOT_FOUND;
    for (int i = 0; i < l->size; ++i)
        if (l->data[i] == x)
            return i;
    return ELEMENT_NOT_FOUND;
}

int lst_find_kth(int k, const ListArray *l)
{
    if (l == NULL || k < 1 || k > l->size)
        return ELEMENT_NOT_FOUND;
    return k - 1;
}

element_type lst_get(int index, const ListArray *l)
{
    if (l == NULL || index < 0 || index >= l->size)
        return ELEMENT_NOT_FOUND;
    return l->data[index];
}

static int lst_grow(ListArray *l)
{
    int new_cap = l->capacity * 2;
    element_type *p = realloc(l->data, (size_t)new_cap * sizeof *p);
    if (p == NULL)
        return -1;
    l->data = p;
    l->capacity = new_cap;
    return 0;
}

int lst_insert(element_type x, int index, ListArray *l)
{
    if (l == NULL || index < 0 || index > l->size)
        return -1;
    if (l->size == l->capacity && lst_grow(l) != 0)
        return -1;
    for (int i = l->size; i > index; --i)
        l->data[i] = l->data[i - 1];
    l->data[index] = x;
    ++l->size;
    return 0;
}

int lst_delete(element_type x, ListArray *l)
{
    int p = lst_find(x, l);
    if (p == ELEMENT_NOT_FOUND)
        return ELEMENT_NOT_FOUND;
    for (int i = p; i < l->size - 1; ++i)
        l->data[i] = l->data[i + 1];
    --l->size;
    return p;
}

element_type lst_delete_at(int index, ListArray *l)
{
    if (l == NULL || index < 0 || index >= l->size)
        return ELEMENT_NOT_FOUND;
    element_type v = l->data[index];
    for (int i = index; i < l->size - 1; ++i)
        l->data[i] = l->data[i + 1];
    --l->size;
    return v;
}

void lst_print(const ListArray *l, FILE *fp)
{
    fprintf(fp, "(");
    for (int i = 0; l && i < l->size; ++i)
        fprintf(fp, "%d%s", l->data[i], i + 1 < l->size ? ", " : "");
    fprintf(fp, ")");
}

int lso_find_move_to_front(element_type x, ListArray *l)
{
    int p = lst_find(x, l);
    if (p <= 0)
        return p;                       /* not found, or already first */
    element_type v = l->data[p];
    for (int i = p; i > 0; --i)
        l->data[i] = l->data[i - 1];
    l->data[0] = v;
    return 0;
}

int lso_find_transpose(element_type x, ListArray *l)
{
    int p = lst_find(x, l);
    if (p <= 0)
        return p;
    element_type t = l->data[p - 1];
    l->data[p - 1] = l->data[p];
    l->data[p] = t;
    return p - 1;
}
