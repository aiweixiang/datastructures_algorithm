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

/* ========== 栈：支持 O(1) 的 FindMin（习题 3.22/3.25 a） ========== */
typedef struct MinStack MinStack;
MinStack    *mstk_create(void);
void         mstk_dispose(MinStack *s);
int          mstk_push(element_type x, MinStack *s);
element_type mstk_pop(MinStack *s);
element_type mstk_find_min(const MinStack *s);   /* O(1) */
int          mstk_size(const MinStack *s);
int          mstk_is_empty(const MinStack *s);

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

/* ================= 3.2 PrintLots ================= */
/* 按 P 中给出的位置（**1 起始**）打印 L 中的元素；越界位置跳过并计入 *skipped。
 * simple 版对每个位置各走一遍链表 O(|P|·|L|)；linear 版要求 P 升序，O(|L|+|P|)。 */
int print_lots_simple(const ListLinked *l, const int positions[], int npos,
                      FILE *fp, int *skipped);
int print_lots_linear(const ListLinked *l, const int positions[], int npos,
                      FILE *fp, int *skipped);

/* ================= 3.8 多项式的幂 ================= */
Polynomial *poly_one(void);
Polynomial *poly_pow(const Polynomial *p, int n);   /* 二进制幂，O(deg^2 · log n) */

/* ================= 3.9 任意精度整数 ================= */
typedef struct BigInt BigInt;
BigInt *big_from_str(const char *s);
BigInt *big_from_ll(long long v);
void    big_dispose(BigInt *b);
char   *big_to_str(const BigInt *b);        /* 调用者 free */
int     big_cmp(const BigInt *a, const BigInt *b);
BigInt *big_neg(const BigInt *a);
BigInt *big_add(const BigInt *a, const BigInt *b);
BigInt *big_sub(const BigInt *a, const BigInt *b);
BigInt *big_mul(const BigInt *a, const BigInt *b);

/* ================= 3.11 递归查找链表元素 ================= */
PtrToNode lil_find_recursive(element_type x, const ListLinked *l);

/* ================= 3.13 基数排序学生记录 ================= */
typedef struct {
    int  key;
    char name[16];
} StudentRecord;

void radix_sort_records(StudentRecord a[], int n);   /* LSD，十进制，O(P·(N+10)) */

/* ================= 3.14 图 → 邻接表（链表版 / 游标版） ================= */
typedef struct AdjNode {
    int             vertex;
    int             weight;
    struct AdjNode *next;
} AdjNode;

typedef struct {
    int        n;          /* 顶点数，编号 1..n */
    int        m;          /* 有向边条数 */
    AdjNode  **head;       /* head[1..n] */
} AdjGraph;

AdjGraph *adj_create(int n);
void      adj_dispose(AdjGraph *g);
int       adj_add_edge(AdjGraph *g, int u, int v, int w, int undirected);
AdjGraph *adj_read(FILE *fp, int n);          /* 每行 "u v [w]"，w 默认 1 */
void      adj_print(const AdjGraph *g, FILE *fp);

typedef struct {
    int vertex;
    int weight;
    int next;              /* 下一条边的下标，CURSOR_NULL 表示结束 */
} CursorAdj;

typedef struct {
    int        n;
    int        m;
    int        capacity;
    CursorAdj *edges;      /* edges[1..capacity] */
    int       *head;       /* head[1..n] */
} CursorAdjGraph;

CursorAdjGraph *cadg_create(int n, int max_edges);
void            cadg_dispose(CursorAdjGraph *g);
int             cadg_add_edge(CursorAdjGraph *g, int u, int v, int w, int undirected);
CursorAdjGraph *cadg_from_adj(const AdjGraph *g);

/* ================= 3.16 删除数组中的重复元素 ================= */
int remove_duplicates_slow(int a[], int n);   /* O(N^2)，保持首次出现顺序 */
int remove_duplicates_sort(int a[], int n);   /* O(N log N)，改变顺序 */

/* ================= 3.17 懒惰删除 ================= */
typedef struct LazyList LazyList;
LazyList *lz_create(int cap);
void      lz_dispose(LazyList *l);
int       lz_compact(LazyList *l);
int       lz_insert(element_type x, LazyList *l);
int       lz_find(element_type x, const LazyList *l);    /* 返回槽位下标或 -1 */
int       lz_delete(element_type x, LazyList *l);
int       lz_live(const LazyList *l);
int       lz_deleted(const LazyList *l);
int       lz_slots(const LazyList *l);          /* 含已删除的槽位总数 */
int       lz_compactions(const LazyList *l);    /* 整理次数（摊还分析用） */

/* ================= 3.23 一个数组实现三个栈 ================= */
typedef struct ThreeStacks ThreeStacks;
ThreeStacks *ts3_create(int capacity);
void         ts3_dispose(ThreeStacks *t);
int          ts3_push(int which, element_type x, ThreeStacks *t);
int          ts3_pop(int which, ThreeStacks *t, element_type *out);
int          ts3_is_empty(int which, const ThreeStacks *t);
int          ts3_is_full(int which, const ThreeStacks *t);
int          ts3_size(int which, const ThreeStacks *t);
int          ts3_capacity_share(const ThreeStacks *t, int which);

/* ================= 3.24 斐波那契递归的栈空间 ================= */
int                fib_max_depth(int n);        /* 最大栈帧数：n≥1 时为 n，n=0 时为 1 */
unsigned long long fib_call_count(int n);       /* == 2F(n+1) - 1 */

/* ================= 3.26 双端队列 ================= */
typedef struct Deque Deque;
Deque       *deq_create(int init_capacity);
void         deq_dispose(Deque *d);
void         deq_make_empty(Deque *d);
int          deq_is_empty(const Deque *d);
int          deq_size(const Deque *d);
int          deq_capacity(const Deque *d);
int          deq_push_front(element_type x, Deque *d);
int          deq_push_back(element_type x, Deque *d);
element_type deq_front(const Deque *d);
element_type deq_back(const Deque *d);
element_type deq_pop_front(Deque *d);
element_type deq_pop_back(Deque *d);
void         deq_print(const Deque *d, FILE *fp);

#endif /* CH03_H */
