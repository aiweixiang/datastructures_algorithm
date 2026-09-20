/* Tests for every Chapter 3 implementation. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ch03_lists_stacks_queues/ch03.h"
#include "test_util.h"

static void test_list_array(void)
{
    ListArray *l = lst_create(2);
    CHECK(lst_is_empty(l));
    CHECK_EQ(lst_size(l), 0);
    CHECK_EQ(lst_delete(9, l), ELEMENT_NOT_FOUND);
    CHECK_EQ(lst_get(0, l), ELEMENT_NOT_FOUND);

    lst_insert(5, 0, l);
    lst_insert(3, 0, l);
    lst_insert(9, 2, l);                 /* (3, 5, 9) */
    CHECK_EQ(lst_size(l), 3);
    CHECK_EQ(lst_get(0, l), 3);
    CHECK_EQ(lst_get(1, l), 5);
    CHECK_EQ(lst_get(2, l), 9);
    CHECK_EQ(lst_find(5, l), 1);
    CHECK_EQ(lst_find(42, l), ELEMENT_NOT_FOUND);
    CHECK_EQ(lst_find_kth(1, l), 0);
    CHECK_EQ(lst_find_kth(3, l), 2);
    CHECK_EQ(lst_find_kth(4, l), ELEMENT_NOT_FOUND);
    CHECK_EQ(lst_insert(4, 9, l), -1);   /* index out of range */
    CHECK_EQ(lst_delete(5, l), 1);       /* (3, 9) */
    CHECK_EQ(lst_delete_at(0, l), 3);    /* (9) */

    for (int i = 0; i < 100; ++i)        /* forces several doublings */
        CHECK_EQ(lst_insert(i, lst_size(l), l), 0);
    CHECK_EQ(lst_size(l), 101);
    CHECK_EQ(lst_get(0, l), 9);
    CHECK_EQ(lst_get(1, l), 0);
    CHECK_EQ(lst_get(100, l), 99);
    CHECK(lst_capacity(l) >= 101);
    lst_make_empty(l);
    CHECK(lst_is_empty(l));
    lst_dispose(l);

    /* self-organizing heuristics */
    ListArray *m = lst_create(4);
    for (int i = 1; i <= 4; ++i)
        lst_insert(i, lst_size(m), m);           /* (1,2,3,4) */
    CHECK_EQ(lso_find_move_to_front(3, m), 0);   /* (3,1,2,4) */
    CHECK_EQ(lst_get(0, m), 3);
    CHECK_EQ(lst_get(1, m), 1);
    CHECK_EQ(lso_find_move_to_front(3, m), 0);   /* already first */
    CHECK_EQ(lso_find_move_to_front(9, m), ELEMENT_NOT_FOUND);
    CHECK_EQ(lso_find_transpose(1, m), 0);       /* (1,3,2,4) */
    CHECK_EQ(lst_get(0, m), 1);
    CHECK_EQ(lst_get(1, m), 3);
    lst_dispose(m);
}

static void test_list_linked(void)
{
    ListLinked *l = lil_create();
    CHECK(lil_is_empty(l));
    CHECK_EQ(lil_size(l), 0);
    CHECK(lil_find(1, l) == NULL);
    CHECK(lil_find_previous(1, l) == NULL);

    lil_insert(1, 0, l);
    lil_insert(2, 1, l);
    lil_insert(3, 1, l);                   /* (1, 3, 2) */
    CHECK_EQ(lil_size(l), 3);
    CHECK_EQ(lil_at(0, l), 1);
    CHECK_EQ(lil_at(1, l), 3);
    CHECK_EQ(lil_at(2, l), 2);
    CHECK_EQ(lil_at(3, l), ELEMENT_NOT_FOUND);
    CHECK(lil_find(3, l) != NULL);
    CHECK(lil_find_previous(1, l) == l->head);   /* first element */
    CHECK(lil_find_previous(3, l) != NULL);
    CHECK(lil_find_previous(3, l)->elem == 1);
    CHECK(lil_find_previous(2, l) != NULL);
    CHECK(lil_find_previous(2, l)->elem == 3);
    CHECK_EQ(lil_delete(3, l), 0);         /* (1, 2) */
    CHECK_EQ(lil_delete(99, l), -1);
    CHECK_EQ(lil_at(1, l), 2);

    lil_insert(4, 0, l);                   /* (4, 1, 2) */
    lil_reverse(l);                        /* (2, 1, 4) */
    CHECK_EQ(lil_at(0, l), 2);
    CHECK_EQ(lil_at(1, l), 1);
    CHECK_EQ(lil_at(2, l), 4);
    lil_swap_adjacent(l, 1);               /* (2, 4, 1) */
    CHECK_EQ(lil_at(1, l), 4);
    CHECK_EQ(lil_at(2, l), 1);
    lil_swap_adjacent(l, 2);               /* out of range: no-op */
    CHECK_EQ(lil_at(2, l), 1);
    lil_swap_adjacent(l, -1);
    CHECK_EQ(lil_size(l), 3);

    CHECK_EQ(lil_has_loop(l), 0);
    lil_make_loop(l, 1);                   /* tail -> node 1 */
    CHECK_EQ(lil_has_loop(l), 1);
    lil_break_loop(l);
    CHECK_EQ(lil_has_loop(l), 0);
    CHECK_EQ(lil_at(1, l), 4);
    CHECK_EQ(lil_size(l), 3);

    CHECK_EQ(lil_insert(7, 3, l), 0);      /* append */
    CHECK_EQ(lil_at(3, l), 7);
    CHECK_EQ(lil_insert(8, 9, l), -1);
    lil_dispose(l);
}

static void test_list_cursor(void)
{
    ListCursor *c = lcs_create(4);         /* header eats one slot -> 3 elems */
    CHECK(lcs_is_empty(c));
    CHECK_EQ(lcs_size(c), 0);
    CHECK_EQ(lcs_insert(10, 0, c), 0);
    CHECK_EQ(lcs_insert(20, 1, c), 0);
    CHECK_EQ(lcs_insert(30, 1, c), 0);     /* (10, 30, 20) */
    CHECK_EQ(lcs_size(c), 3);
    CHECK_EQ(lcs_at(0, c), 10);
    CHECK_EQ(lcs_at(1, c), 30);
    CHECK_EQ(lcs_at(2, c), 20);
    CHECK(lcs_find(30, c) != CURSOR_NULL);
    CHECK_EQ(lcs_find(77, c), CURSOR_NULL);
    CHECK_EQ(lcs_insert(40, 0, c), -1);    /* genuinely out of space */
    CHECK_EQ(lcs_delete(30, c), 0);
    CHECK_EQ(lcs_insert(40, 0, c), 0);     /* the freed slot is reused */
    CHECK_EQ(lcs_at(0, c), 40);
    CHECK_EQ(lcs_find_kth(1, c), lcs_find(40, c));
    CHECK_EQ(lcs_find_kth(2, c), lcs_find(10, c));
    CHECK_EQ(lcs_find_kth(3, c), lcs_find(20, c));
    CHECK_EQ(lcs_find_kth(4, c), CURSOR_NULL);
    CHECK_EQ(lcs_delete(1234, c), -1);
    lcs_make_empty(c);
    CHECK(lcs_is_empty(c));
    CHECK_EQ(lcs_insert(1, 0, c), 0);      /* free list survived empty */
    CHECK_EQ(lcs_at(0, c), 1);
    lcs_dispose(c);
}

static void test_list_doubly(void)
{
    ListDoubly *d = dll_create();
    CHECK(dll_is_empty(d));
    dll_push_back(2, d);
    dll_push_front(1, d);
    dll_push_back(3, d);                   /* (1, 2, 3) */
    CHECK_EQ(dll_size(d), 3);
    dll_insert(9, 1, d);                   /* (1, 9, 2, 3) */
    CHECK_EQ(dll_at(1, d), 9);
    dll_insert(0, 4, d);                   /* append -> (1,9,2,3,0) */
    CHECK_EQ(dll_at(4, d), 0);
    CHECK_EQ(dll_insert(5, 9, d), -1);
    CHECK_EQ(dll_delete(9, d), 0);
    CHECK_EQ(dll_at(1, d), 2);
    CHECK_EQ(dll_delete_at(0, d), 1);      /* (2, 3, 0) */
    dll_reverse(d);                        /* (0, 3, 2) */
    CHECK_EQ(dll_at(0, d), 0);
    CHECK_EQ(dll_at(1, d), 3);
    CHECK_EQ(dll_at(2, d), 2);
    CHECK_EQ(dll_delete(42, d), -1);
    CHECK_EQ(dll_delete_at(9, d), ELEMENT_NOT_FOUND);
    dll_dispose(d);
}

static void test_stacks(void)
{
    StackArray *s = stk_create(2);
    CHECK(stk_is_empty(s));
    CHECK_EQ(stk_pop(s), ELEMENT_NOT_FOUND);
    CHECK_EQ(stk_top(s), ELEMENT_NOT_FOUND);
    for (int i = 1; i <= 10; ++i)
        CHECK_EQ(stk_push(i, s), 0);       /* grows three times */
    CHECK_EQ(stk_size(s), 10);
    CHECK(stk_capacity(s) >= 10);
    CHECK_EQ(stk_top(s), 10);
    CHECK_EQ(stk_pop(s), 10);
    CHECK_EQ(stk_size(s), 9);
    stk_make_empty(s);
    CHECK(stk_is_empty(s));
    stk_dispose(s);

    StackLinked *t = stkl_create();
    CHECK(stkl_is_empty(t));
    for (int i = 1; i <= 5; ++i)
        stkl_push(i, t);
    CHECK_EQ(stkl_top(t), 5);
    CHECK_EQ(stkl_pop(t), 5);
    CHECK_EQ(stkl_pop(t), 4);
    CHECK_EQ(stkl_top(t), 3);
    stkl_make_empty(t);
    CHECK(stkl_is_empty(t));
    CHECK_EQ(stkl_top(t), ELEMENT_NOT_FOUND);
    CHECK_EQ(stkl_pop(t), ELEMENT_NOT_FOUND);
    stkl_dispose(t);
}

static void test_queue(void)
{
    QueueCircular *q = que_create(2);
    CHECK(que_is_empty(q));
    CHECK_EQ(que_dequeue(q), ELEMENT_NOT_FOUND);
    CHECK_EQ(que_front(q), ELEMENT_NOT_FOUND);
    for (int i = 1; i <= 6; ++i)
        CHECK_EQ(que_enqueue(i, q), 0);    /* grows */
    CHECK_EQ(que_size(q), 6);
    CHECK_EQ(que_front(q), 1);
    for (int i = 1; i <= 6; ++i)
        CHECK_EQ(que_dequeue(q), i);
    CHECK(que_is_empty(q));
    CHECK_EQ(que_enqueue(7, q), 0);        /* reusable after draining */
    CHECK_EQ(que_dequeue(q), 7);
    que_dispose(q);

    QueueCircular *r = que_create(4);      /* wraparound must not grow */
    for (int i = 1; i <= 4; ++i)
        que_enqueue(i, r);
    CHECK_EQ(que_dequeue(r), 1);
    CHECK_EQ(que_enqueue(5, r), 0);
    CHECK_EQ(que_dequeue(r), 2);
    CHECK_EQ(que_dequeue(r), 3);
    CHECK_EQ(que_dequeue(r), 4);
    CHECK_EQ(que_dequeue(r), 5);
    CHECK(que_is_empty(r));
    que_dispose(r);
}

static void test_two_stacks(void)
{
    TwoStacks *t = ts_create(4);
    element_type v = 0;
    CHECK(ts_is_empty(0, t));
    CHECK(ts_is_empty(1, t));
    CHECK_EQ(ts_push(0, 1, t), 0);
    CHECK_EQ(ts_push(1, 2, t), 0);
    CHECK_EQ(ts_push(0, 3, t), 0);
    CHECK_EQ(ts_push(1, 4, t), 0);
    CHECK(ts_is_full(t));                  /* all four slots used */
    CHECK_EQ(ts_push(0, 9, t), -1);
    CHECK_EQ(ts_size(0, t), 2);
    CHECK_EQ(ts_size(1, t), 2);
    CHECK_EQ(ts_pop(0, t, &v), 0);
    CHECK_EQ(v, 3);
    CHECK_EQ(ts_push(0, 5, t), 0);
    CHECK_EQ(ts_pop(1, t, &v), 0);
    CHECK_EQ(v, 4);
    CHECK_EQ(ts_pop(1, t, &v), 0);
    CHECK_EQ(v, 2);
    CHECK(ts_is_empty(1, t));
    CHECK_EQ(ts_pop(1, t, &v), -1);
    CHECK_EQ(ts_pop(0, t, &v), 0);
    CHECK_EQ(v, 5);
    ts_dispose(t);
}

static void test_balanced_symbols(void)
{
    CHECK_EQ(balanced_symbols(""), 1);
    CHECK_EQ(balanced_symbols("()[]{}"), 1);
    CHECK_EQ(balanced_symbols("a(b[c]{d})e"), 1);
    CHECK_EQ(balanced_symbols("((()))"), 1);
    CHECK_EQ(balanced_symbols("([)]"), 0);      /* interleaved */
    CHECK_EQ(balanced_symbols("("), 0);
    CHECK_EQ(balanced_symbols(")("), 0);
    CHECK_EQ(balanced_symbols("([{}]"), 0);
}

static void test_expressions(void)
{
    char *p;
    double v;
    char *inf;

    p = infix_to_postfix("1 + 2 * 3", 1);
    CHECK(p != NULL);
    if (p) { CHECK_EQ(strcmp(p, "1 2 3 * +"), 0); free(p); }

    p = infix_to_postfix("(1 + 2) * 3", 1);
    CHECK(p != NULL);
    if (p) { CHECK_EQ(strcmp(p, "1 2 + 3 *"), 0); free(p); }

    p = infix_to_postfix("12 + 345", 1);       /* multi-digit operands */
    CHECK(p != NULL);
    if (p) { CHECK_EQ(strcmp(p, "12 345 +"), 0); free(p); }

    p = infix_to_postfix("2 ^ 3 ^ 2", 1);      /* '^' is right-associative */
    CHECK(p != NULL);
    if (p) { CHECK_EQ(strcmp(p, "2 3 2 ^ ^"), 0); free(p); }

    CHECK(infix_to_postfix("2 ^ 3", 0) == NULL);   /* exponent disabled */
    CHECK(infix_to_postfix("(1 + 2", 1) == NULL);  /* unbalanced */
    CHECK(infix_to_postfix("1 # 2", 1) == NULL);   /* bad character */

    CHECK_EQ(postfix_eval("1 2 3 * +", &v), 0);
    CHECK_NEAR(v, 7.0, 1e-9);
    CHECK_EQ(postfix_eval("1 2 + 3 *", &v), 0);
    CHECK_NEAR(v, 9.0, 1e-9);
    CHECK_EQ(postfix_eval("7 2 /", &v), 0);
    CHECK_NEAR(v, 3.5, 1e-9);
    CHECK_EQ(postfix_eval("2 3 2 ^ ^", &v), 0);
    CHECK_NEAR(v, 512.0, 1e-9);
    CHECK_EQ(postfix_eval("12 345 +", &v), 0);
    CHECK_NEAR(v, 357.0, 1e-9);
    CHECK_EQ(postfix_eval("1 0 /", &v), -1);       /* division by zero */
    CHECK_EQ(postfix_eval("1 +", &v), -1);
    CHECK_EQ(postfix_eval("", &v), -1);
    CHECK_EQ(postfix_eval("1 2", &v), -1);

    inf = postfix_to_infix("1 2 + 3 *");
    CHECK(inf != NULL);
    if (inf) { CHECK_EQ(strcmp(inf, "((1 + 2) * 3)"), 0); free(inf); }
    inf = postfix_to_infix("12 345 +");
    CHECK(inf != NULL);
    if (inf) { CHECK_EQ(strcmp(inf, "(12 + 345)"), 0); free(inf); }
    CHECK(postfix_to_infix("1 +") == NULL);

    /* round trip: infix -> postfix -> value */
    p = infix_to_postfix("(2 + 3) * (4 - 1) ^ 2", 1);
    CHECK(p != NULL);
    if (p) {
        CHECK_EQ(postfix_eval(p, &v), 0);
        CHECK_NEAR(v, 45.0, 1e-9);       /* 5 * 9 */
        free(p);
    }
}

static void test_josephus(void)
{
    CHECK_EQ(josephus_linked(5, 2), 3);
    CHECK_EQ(josephus_formula(5, 2), 3);
    CHECK_EQ(josephus_linked(7, 3), 4);
    CHECK_EQ(josephus_formula(7, 3), 4);
    CHECK_EQ(josephus_linked(1, 1), 1);
    CHECK_EQ(josephus_linked(0, 3), -1);
    for (int n = 1; n <= 30; ++n)
        for (int m = 1; m <= 12; ++m)
            CHECK_EQ(josephus_linked(n, m), josephus_formula(n, m));
}

static void test_polynomial(void)
{
    double ca[] = {1.0, 2.0, 3.0};         /* 1 + 2x + 3x^2 */
    double cb[] = {-1.0, 1.0};             /* -1 + x        */
    Polynomial *a = poly_create(ca, 2);
    Polynomial *b = poly_create(cb, 1);
    CHECK(a != NULL && b != NULL);
    CHECK_EQ(a->degree, 2);
    CHECK_NEAR(poly_eval(a, 2.0), 17.0, 1e-9);

    Polynomial *sum = poly_add(a, b);
    CHECK(sum != NULL);
    CHECK_EQ(sum->degree, 2);
    CHECK_NEAR(sum->coeff[0], 0.0, 1e-12);
    CHECK_NEAR(sum->coeff[1], 3.0, 1e-12);
    CHECK_NEAR(sum->coeff[2], 3.0, 1e-12);

    Polynomial *prod = poly_mul(a, b);
    CHECK(prod != NULL);
    CHECK_EQ(prod->degree, 3);
    CHECK_NEAR(prod->coeff[0], -1.0, 1e-12);
    CHECK_NEAR(prod->coeff[1], -1.0, 1e-12);
    CHECK_NEAR(prod->coeff[2], -1.0, 1e-12);
    CHECK_NEAR(prod->coeff[3], 3.0, 1e-12);
    CHECK_NEAR(poly_eval(prod, 2.0), 17.0, 1e-9);

    double cz[] = {0.0, 0.0, 0.0};
    Polynomial *z = poly_create(cz, 2);
    CHECK(z != NULL);
    CHECK_EQ(z->degree, 0);                /* leading zeros are trimmed */
    CHECK_NEAR(poly_eval(z, 5.0), 0.0, 1e-12);

    poly_dispose(a);
    poly_dispose(b);
    poly_dispose(sum);
    poly_dispose(prod);
    poly_dispose(z);
}

static void test_sorted_set_ops(void)
{
    int a[] = {1, 3, 5, 7, 9};
    int b[] = {3, 4, 5, 5, 10};
    int out[16];
    int want[] = {1, 3, 4, 5, 7, 9, 10};

    int k = sorted_intersect(a, 5, b, 5, out);
    CHECK_EQ(k, 2);
    CHECK_EQ(out[0], 3);
    CHECK_EQ(out[1], 5);
    CHECK_EQ(sorted_intersect(a, 5, NULL, 0, out), 0);
    CHECK_EQ(sorted_intersect(NULL, 0, b, 5, out), 0);

    k = sorted_union(a, 5, b, 5, out);
    CHECK_EQ(k, 7);
    for (int i = 0; i < k && i < 7; ++i)
        CHECK_EQ(out[i], want[i]);
    CHECK_EQ(sorted_union(a, 5, NULL, 0, out), 5);
}

/* 习题「带 FindMin 的栈」（英文 2e 3.25 a / 中文版 3.22 a）：
 * 只读的 FindMin 可以 O(1)，因为栈是 LIFO；这里与一个暴力扫描的参考实现对拍。 */
static unsigned long ms_state = 7UL;
static int ms_next(int lo, int hi)
{
    ms_state = ms_state * 1103515245UL + 12345UL;
    return lo + (int)((ms_state >> 16) % (unsigned long)(hi - lo + 1));
}

static void test_min_stack(void)
{
    MinStack *s = mstk_create();
    CHECK(s != NULL);
    CHECK(mstk_is_empty(s));
    CHECK_EQ(mstk_size(s), 0);
    CHECK_EQ(mstk_find_min(s), ELEMENT_NOT_FOUND);
    CHECK_EQ(mstk_pop(s), ELEMENT_NOT_FOUND);

    int shadow[256];
    int n = 0;
    for (int step = 0; step < 2000; ++step) {
        if (n == 0 || ms_next(0, 1) == 0) {          /* push */
            int v = ms_next(-50, 50);
            CHECK_EQ(mstk_push(v, s), 0);
            shadow[n++] = v;
        } else {                                      /* pop */
            CHECK_EQ(mstk_pop(s), shadow[n - 1]);
            --n;
        }
        int want_min = ELEMENT_NOT_FOUND;             /* O(N) reference scan */
        for (int i = 0; i < n; ++i)
            if (i == 0 || shadow[i] < want_min)
                want_min = shadow[i];
        CHECK_EQ(mstk_find_min(s), want_min);
        CHECK_EQ(mstk_size(s), n);
        CHECK_EQ(mstk_is_empty(s), n == 0);
    }
    mstk_dispose(s);

    /* 重复的最小值：FindMin 必须一直返回 3，直到两个 3 都被弹出 */
    MinStack *d = mstk_create();
    CHECK_EQ(mstk_push(5, d), 0);
    CHECK_EQ(mstk_push(3, d), 0);
    CHECK_EQ(mstk_push(3, d), 0);
    CHECK_EQ(mstk_push(7, d), 0);
    CHECK_EQ(mstk_find_min(d), 3);
    CHECK_EQ(mstk_pop(d), 7);
    CHECK_EQ(mstk_find_min(d), 3);
    CHECK_EQ(mstk_pop(d), 3);
    CHECK_EQ(mstk_find_min(d), 3);                    /* 另一个 3 还在 */
    CHECK_EQ(mstk_pop(d), 3);
    CHECK_EQ(mstk_find_min(d), 5);
    CHECK_EQ(mstk_pop(d), 5);
    CHECK_EQ(mstk_find_min(d), ELEMENT_NOT_FOUND);
    CHECK(mstk_is_empty(d));
    mstk_dispose(d);

    /* 单调递减压栈：最小值总是栈顶 */
    MinStack *dec = mstk_create();
    for (int i = 10; i >= 1; --i) {
        CHECK_EQ(mstk_push(i, dec), 0);
        CHECK_EQ(mstk_find_min(dec), i);
    }
    CHECK_EQ(mstk_size(dec), 10);
    mstk_dispose(dec);

    /* 单调递增压栈：最小值永远是第一个元素 */
    MinStack *inc = mstk_create();
    for (int i = 1; i <= 10; ++i) {
        CHECK_EQ(mstk_push(i, inc), 0);
        CHECK_EQ(mstk_find_min(inc), 1);
    }
    CHECK_EQ(mstk_size(inc), 10);
    mstk_dispose(inc);
}

int main(void)
{
    test_list_array();
    test_list_linked();
    test_list_cursor();
    test_list_doubly();
    test_stacks();
    test_min_stack();
    test_queue();
    test_two_stacks();
    test_balanced_symbols();
    test_expressions();
    test_josephus();
    test_polynomial();
    test_sorted_set_ops();
    return TEST_REPORT();
}
