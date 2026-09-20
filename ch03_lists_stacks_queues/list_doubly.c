/* Chapter 3: doubly linked list with a sentinel head node.
 * The sentinel removes every "am I at the head?" special case, and the
 * extra prev pointer makes Delete of a known node O(1).
 */
#include <stdlib.h>
#include "ch03.h"

ListDoubly *dll_create(void)
{
    ListDoubly *l = malloc(sizeof *l);
    if (l == NULL)
        return NULL;
    l->head = malloc(sizeof *l->head);
    if (l->head == NULL) {
        free(l);
        return NULL;
    }
    l->head->prev = l->head;
    l->head->next = l->head;
    l->size = 0;
    return l;
}

void dll_make_empty(ListDoubly *l)
{
    if (l == NULL)
        return;
    DNode *p = l->head->next;
    while (p != l->head) {
        DNode *next = p->next;
        free(p);
        p = next;
    }
    l->head->next = l->head;
    l->head->prev = l->head;
    l->size = 0;
}

void dll_dispose(ListDoubly *l)
{
    if (l == NULL)
        return;
    dll_make_empty(l);
    free(l->head);
    free(l);
}

int dll_is_empty(const ListDoubly *l) { return l == NULL || l->size == 0; }
int dll_size(const ListDoubly *l) { return l ? l->size : 0; }

/* insert x between `before` and before->next */
static void dll_link(DNode *before, element_type x, ListDoubly *l)
{
    DNode *n = malloc(sizeof *n);
    if (n == NULL)
        return;
    n->elem = x;
    n->prev = before;
    n->next = before->next;
    before->next->prev = n;
    before->next = n;
    ++l->size;
}

int dll_push_front(element_type x, ListDoubly *l)
{
    if (l == NULL) return -1;
    int before = l->size;
    dll_link(l->head, x, l);
    return l->size > before ? 0 : -1;
}

int dll_push_back(element_type x, ListDoubly *l)
{
    if (l == NULL) return -1;
    int before = l->size;
    dll_link(l->head->prev, x, l);
    return l->size > before ? 0 : -1;
}

static DNode *dll_node_at(int index, const ListDoubly *l)
{
    if (l == NULL || index < 0 || index >= l->size)
        return NULL;
    DNode *p;
    if (index <= l->size / 2) {          /* walk from the front */
        p = l->head->next;
        for (int i = 0; i < index; ++i)
            p = p->next;
    } else {                             /* or from the back */
        p = l->head->prev;
        for (int i = l->size - 1; i > index; --i)
            p = p->prev;
    }
    return p;
}

int dll_insert(element_type x, int index, ListDoubly *l)
{
    if (l == NULL || index < 0 || index > l->size)
        return -1;
    DNode *before = (index == l->size) ? l->head->prev
                                       : dll_node_at(index, l)->prev;
    int prev_size = l->size;
    dll_link(before, x, l);
    return l->size > prev_size ? 0 : -1;
}

int dll_delete(element_type x, ListDoubly *l)
{
    if (l == NULL)
        return -1;
    for (DNode *p = l->head->next; p != l->head; p = p->next) {
        if (p->elem == x) {
            p->prev->next = p->next;
            p->next->prev = p->prev;
            free(p);
            --l->size;
            return 0;
        }
    }
    return -1;
}

element_type dll_delete_at(int index, ListDoubly *l)
{
    DNode *p = dll_node_at(index, l);
    if (p == NULL)
        return ELEMENT_NOT_FOUND;
    element_type v = p->elem;
    p->prev->next = p->next;
    p->next->prev = p->prev;
    free(p);
    --l->size;
    return v;
}

element_type dll_at(int index, const ListDoubly *l)
{
    DNode *p = dll_node_at(index, l);
    return p ? p->elem : ELEMENT_NOT_FOUND;
}

void dll_reverse(ListDoubly *l)
{
    if (l == NULL)
        return;
    for (DNode *p = l->head; ; ) {
        DNode *next = p->next;
        p->next = p->prev;
        p->prev = next;
        if (next == l->head)
            break;
        p = next;
    }
}

void dll_print(const ListDoubly *l, FILE *fp)
{
    fprintf(fp, "(");
    if (l != NULL) {
        for (DNode *p = l->head->next; p != l->head; p = p->next)
            fprintf(fp, "%d%s", p->elem, p->next != l->head ? ", " : "");
    }
    fprintf(fp, ")");
}
