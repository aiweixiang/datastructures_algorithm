/* Chapter 3: singly linked list with a dummy header node (Weiss listli).
 * Insert at a known node is O(1); FindKth and Delete(value) are O(N).
 */
#include <stdlib.h>
#include "ch03.h"

ListLinked *lil_create(void)
{
    ListLinked *l = malloc(sizeof *l);
    if (l == NULL)
        return NULL;
    l->head = malloc(sizeof *l->head);
    if (l->head == NULL) {
        free(l);
        return NULL;
    }
    l->head->next = NULL;
    l->size = 0;
    return l;
}

void lil_make_empty(ListLinked *l)
{
    if (l == NULL)
        return;
    PtrToNode p = l->head->next;
    while (p != NULL) {
        PtrToNode next = p->next;
        free(p);
        p = next;
    }
    l->head->next = NULL;
    l->size = 0;
}

void lil_dispose(ListLinked *l)
{
    if (l == NULL)
        return;
    lil_make_empty(l);
    free(l->head);
    free(l);
}

int lil_is_empty(const ListLinked *l) { return l == NULL || l->head->next == NULL; }
int lil_size(const ListLinked *l) { return l ? l->size : 0; }

PtrToNode lil_find(element_type x, const ListLinked *l)
{
    if (l == NULL)
        return NULL;
    PtrToNode p = l->head->next;
    while (p != NULL && p->elem != x)
        p = p->next;
    return p;
}

PtrToNode lil_find_previous(element_type x, const ListLinked *l)
{
    if (l == NULL)
        return NULL;
    PtrToNode p = l->head;
    while (p->next != NULL && p->next->elem != x)
        p = p->next;
    return p->next != NULL ? p : NULL;
}

PtrToNode lil_find_kth(int k, const ListLinked *l)
{
    if (l == NULL || k < 1)
        return NULL;
    PtrToNode p = l->head->next;
    for (int i = 1; i < k && p != NULL; ++i)
        p = p->next;
    return p;
}

element_type lil_at(int index, const ListLinked *l)
{
    PtrToNode p = lil_find_kth(index + 1, l);
    return p ? p->elem : ELEMENT_NOT_FOUND;
}

int lil_insert_after(element_type x, PtrToNode p, ListLinked *l)
{
    if (l == NULL || p == NULL)
        return -1;
    PtrToNode n = malloc(sizeof *n);
    if (n == NULL)
        return -1;
    n->elem = x;
    n->next = p->next;
    p->next = n;
    ++l->size;
    return 0;
}

int lil_insert(element_type x, int index, ListLinked *l)
{
    if (l == NULL || index < 0 || index > l->size)
        return -1;
    PtrToNode p = l->head;                      /* node before position */
    for (int i = 0; i < index; ++i)
        p = p->next;
    return lil_insert_after(x, p, l);
}

int lil_delete(element_type x, ListLinked *l)
{
    PtrToNode prev = lil_find_previous(x, l);
    if (prev == NULL)
        return -1;
    PtrToNode victim = prev->next;
    prev->next = victim->next;
    free(victim);
    --l->size;
    return 0;
}

/* Relink the nodes in reverse order without allocating anything. */
void lil_reverse(ListLinked *l)
{
    if (l == NULL)
        return;
    PtrToNode prev = NULL, cur = l->head->next;
    while (cur != NULL) {
        PtrToNode next = cur->next;
        cur->next = prev;
        prev = cur;
        cur = next;
    }
    l->head->next = prev;
}

/* Swap the elements at positions index and index+1 by pointer surgery only,
 * i.e. without touching the stored elements. */
void lil_swap_adjacent(ListLinked *l, int index)
{
    if (l == NULL || index < 0 || index + 1 >= l->size)
        return;
    PtrToNode before = l->head;
    for (int i = 0; i < index; ++i)
        before = before->next;
    PtrToNode a = before->next;
    PtrToNode b = a->next;              /* non-NULL: index+1 < size */
    a->next = b->next;
    b->next = a;
    before->next = b;
}

int lil_has_loop(const ListLinked *l)
{
    if (l == NULL)
        return 0;
    PtrToNode slow = l->head->next, fast = l->head->next;   /* Floyd */
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast)
            return 1;
    }
    return 0;
}

/* Detect a cycle with Floyd's tortoise and hare, then cut it.  After the
 * meeting point, walking one pointer from the head and one from the meeting
 * point finds the loop start, whose predecessor is the tail. */
void lil_break_loop(ListLinked *l)
{
    if (l == NULL)
        return;
    PtrToNode slow = l->head->next, fast = l->head->next;
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast)
            break;
    }
    if (fast == NULL || fast->next == NULL)
        return;                                     /* no loop */

    slow = l->head->next;
    while (slow != fast) {
        slow = slow->next;
        fast = fast->next;
    }
    PtrToNode tail = slow;
    while (tail->next != slow)
        tail = tail->next;
    tail->next = NULL;
}

/* Test/demonstration helper: make the tail point back at position `index`. */
void lil_make_loop(ListLinked *l, int index)
{
    if (l == NULL || l->head->next == NULL || index < 0)
        return;
    PtrToNode target = lil_find_kth(index + 1, l);
    if (target == NULL)
        return;
    PtrToNode tail = l->head->next;
    while (tail->next != NULL)
        tail = tail->next;
    tail->next = target;
}

void lil_print(const ListLinked *l, FILE *fp)
{
    fprintf(fp, "(");
    if (l != NULL) {
        for (PtrToNode p = l->head->next; p != NULL; p = p->next)
            fprintf(fp, "%d%s", p->elem, p->next ? ", " : "");
    }
    fprintf(fp, ")");
}
