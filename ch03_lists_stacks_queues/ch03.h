/* Chapter 3: Lists, Stacks, and Queues.
 * Public interface for all exercise implementations in this chapter.
 *
 * Conventions (following Weiss):
 *   - element positions in a list are 0-based indexes here;
 *     "k-th" queries are 1-based, because that is how the exercises are worded.
 *   - a lookup that fails returns ELEMENT_NOT_FOUND (-1).
 */
#ifndef CH03_H
#define CH03_H

#include <stdio.h>

typedef int element_type;
#define ELEMENT_NOT_FOUND (-1)

/* ================= Array-based list (Weiss listar) ================= */
typedef struct ListArray ListArray;

ListArray  *lst_create(int init_capacity);
void        lst_dispose(ListArray *l);
void        lst_make_empty(ListArray *l);
int         lst_is_empty(const ListArray *l);
int         lst_size(const ListArray *l);
int         lst_capacity(const ListArray *l);
/* returns the 0-based index of x, or ELEMENT_NOT_FOUND */
int         lst_find(element_type x, const ListArray *l);
/* 1-based: index of the k-th element, or ELEMENT_NOT_FOUND */
int         lst_find_kth(int k, const ListArray *l);
element_type lst_get(int index, const ListArray *l);
/* insert x so that it ends up at position `index` (0 <= index <= size) */
int         lst_insert(element_type x, int index, ListArray *l);
int         lst_delete(element_type x, ListArray *l);      /* index or -1 */
element_type lst_delete_at(int index, ListArray *l);
void        lst_print(const ListArray *l, FILE *fp);

/* 3.x self-organizing list heuristics; return the new index (or -1) */
int lso_find_move_to_front(element_type x, ListArray *l);
int lso_find_transpose(element_type x, ListArray *l);

/* ================= Singly linked list with a header node ================= */
typedef struct Node {
    element_type  elem;
    struct Node  *next;
} Node;
typedef Node *PtrToNode;

typedef struct {
    PtrToNode head;      /* dummy header node */
    int       size;
} ListLinked;

ListLinked *lil_create(void);
void        lil_dispose(ListLinked *l);
void        lil_make_empty(ListLinked *l);
int         lil_is_empty(const ListLinked *l);
int         lil_size(const ListLinked *l);
PtrToNode   lil_find(element_type x, const ListLinked *l);
PtrToNode   lil_find_previous(element_type x, const ListLinked *l);
PtrToNode   lil_find_kth(int k, const ListLinked *l);      /* 1-based */
element_type lil_at(int index, const ListLinked *l);
int         lil_insert_after(element_type x, PtrToNode p, ListLinked *l);
int         lil_insert(element_type x, int index, ListLinked *l);
int         lil_delete(element_type x, ListLinked *l);     /* 0 on success */
void        lil_reverse(ListLinked *l);
void        lil_swap_adjacent(ListLinked *l, int index);   /* 3.x, by relinking */
int         lil_has_loop(const ListLinked *l);             /* Floyd, 3.x */
void        lil_break_loop(ListLinked *l);                 /* 3.x: detect and cut */
void        lil_make_loop(ListLinked *l, int index);       /* test helper */
void        lil_print(const ListLinked *l, FILE *fp);

/* ================= Cursor (array-based) linked list ================= */
typedef struct {
    element_type elem;
    int          next;       /* index of the next node, or CURSOR_NULL */
} CursorNode;

typedef struct {
    int         capacity;    /* allocated slots; one is consumed by this
                                list's header, so capacity-1 elements fit */
    CursorNode *space;       /* space[0] heads the free list */
    int         header;      /* this list's header node */
    int         size;
} ListCursor;

#define CURSOR_NULL (-1)
ListCursor *lcs_create(int capacity);
void        lcs_dispose(ListCursor *l);
void        lcs_make_empty(ListCursor *l);
int         lcs_is_empty(const ListCursor *l);
int         lcs_size(const ListCursor *l);
int         lcs_find(element_type x, const ListCursor *l);       /* index */
int         lcs_find_kth(int k, const ListCursor *l);            /* 1-based */
element_type lcs_at(int index, const ListCursor *l);
int         lcs_insert(element_type x, int index, ListCursor *l);
int         lcs_delete(element_type x, ListCursor *l);
void        lcs_print(const ListCursor *l, FILE *fp);

/* ================= Doubly linked list (with sentinel) ================= */
typedef struct DNode {
    element_type   elem;
    struct DNode  *prev;
    struct DNode  *next;
} DNode;

typedef struct {
    DNode *head;             /* sentinel; head->next is the first element */
    int    size;
} ListDoubly;

ListDoubly  *dll_create(void);
void         dll_dispose(ListDoubly *l);
void         dll_make_empty(ListDoubly *l);
int          dll_is_empty(const ListDoubly *l);
int          dll_size(const ListDoubly *l);
int          dll_push_front(element_type x, ListDoubly *l);
int          dll_push_back(element_type x, ListDoubly *l);
int          dll_insert(element_type x, int index, ListDoubly *l);
int          dll_delete(element_type x, ListDoubly *l);
element_type dll_delete_at(int index, ListDoubly *l);
element_type dll_at(int index, const ListDoubly *l);
void         dll_reverse(ListDoubly *l);
void         dll_print(const ListDoubly *l, FILE *fp);

/* ================= Stack: growing array version ================= */
typedef struct StackArray StackArray;
StackArray  *stk_create(int init_capacity);
void         stk_dispose(StackArray *s);
void         stk_make_empty(StackArray *s);
int          stk_is_empty(const StackArray *s);
int          stk_capacity(const StackArray *s);
int          stk_size(const StackArray *s);
int          stk_push(element_type x, StackArray *s);   /* 0 ok, -1 alloc */
element_type stk_top(const StackArray *s);
element_type stk_pop(StackArray *s);

/* ================= Stack: linked-list version ================= */
typedef struct StackLinked StackLinked;
StackLinked *stkl_create(void);
void         stkl_dispose(StackLinked *s);
void         stkl_make_empty(StackLinked *s);
int          stkl_is_empty(const StackLinked *s);
void         stkl_push(element_type x, StackLinked *s);
element_type stkl_top(const StackLinked *s);
element_type stkl_pop(StackLinked *s);

/* ================= Queue: growing circular array ================= */
typedef struct QueueCircular QueueCircular;
QueueCircular *que_create(int init_capacity);
void           que_dispose(QueueCircular *q);
void           que_make_empty(QueueCircular *q);
int            que_is_empty(const QueueCircular *q);
int            que_size(const QueueCircular *q);
int            que_enqueue(element_type x, QueueCircular *q);
element_type   que_front(const QueueCircular *q);
element_type   que_dequeue(QueueCircular *q);
void           que_print(const QueueCircular *q, FILE *fp);

/* ========== 3.24 Two stacks sharing one array ========== */
typedef struct {
    element_type *data;
    int           capacity;
    int           top_left;      /* -1 when the left stack is empty  */
    int           top_right;     /* capacity when the right is empty */
} TwoStacks;

TwoStacks *ts_create(int capacity);
void       ts_dispose(TwoStacks *t);
int        ts_push(int which, element_type x, TwoStacks *t);  /* which: 0/1 */
int        ts_pop(int which, TwoStacks *t, element_type *out);
int        ts_is_empty(int which, const TwoStacks *t);
int        ts_is_full(const TwoStacks *t);
int        ts_size(int which, const TwoStacks *t);

/* ================= 3.x Applications ================= */
/* balanced (), [], {} -- returns 1 if balanced, 0 otherwise */
int   balanced_symbols(const char *s);
/* infix -> postfix; caller frees the returned string.
 * allow_exp enables the right-associative '^' operator. NULL on error. */
char *infix_to_postfix(const char *expr, int allow_exp);
/* postfix -> infix, fully parenthesised; caller frees. NULL on error. */
char *postfix_to_infix(const char *expr);
/* evaluate a postfix expression; 0 on success, -1 on malformed input */
int   postfix_eval(const char *expr, double *result);
/* 1-based position of the survivor */
int   josephus_linked(int n, int m);
int   josephus_formula(int n, int m);

/* ================= 3.x Polynomial ADT ================= */
typedef struct {
    int     degree;     /* coeff[i] multiplies x^i */
    double *coeff;      /* length degree + 1 */
} Polynomial;

Polynomial *poly_create(const double coeff[], int degree);
void        poly_dispose(Polynomial *p);
Polynomial *poly_add(const Polynomial *a, const Polynomial *b);
Polynomial *poly_mul(const Polynomial *a, const Polynomial *b);
double      poly_eval(const Polynomial *p, double x);
void        poly_print(const Polynomial *p, FILE *fp);

/* ================= 3.x Sorted-list set operations ================= */
int sorted_intersect(const int a[], int na, const int b[], int nb, int out[]);
int sorted_union(const int a[], int na, const int b[], int nb, int out[]);

#endif /* CH03_H */
