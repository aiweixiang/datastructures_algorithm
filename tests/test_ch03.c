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

/* test_ch03 内部的小别名（不依赖第 2 章的 rng.c） */
static int rng_range(int lo, int hi)
{
    return ms_next(lo, hi);
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

/* ============ 3.2 PrintLots ============ */
static char lots_buf[512];

static const char *lots_output(int linear, int *skipped)
{
    ListLinked *l = lil_create();
    for (int i = 1; i <= 8; ++i)
        lil_insert(i * 10, i - 1, l);            /* (10,20,...,80) */
    int pos[] = {2, 4, 8};
    FILE *f = tmpfile();
    if (f == NULL) {
        lil_dispose(l);
        return NULL;
    }
    if (linear)
        print_lots_linear(l, pos, 3, f, skipped);
    else
        print_lots_simple(l, pos, 3, f, skipped);
    fflush(f);
    rewind(f);
    size_t n = fread(lots_buf, 1, sizeof lots_buf - 1, f);
    lots_buf[n] = '\0';
    fclose(f);
    lil_dispose(l);
    return lots_buf;
}

static void test_print_lots(void)
{
    int skipped = -1;
    CHECK_EQ(strcmp(lots_output(0, &skipped), "20 40 80"), 0);
    CHECK_EQ(skipped, 0);
    CHECK_EQ(strcmp(lots_output(1, &skipped), "20 40 80"), 0);
    CHECK_EQ(skipped, 0);

    /* 越界位置必须被跳过并计数，且两种实现输出一致 */
    ListLinked *l = lil_create();
    for (int i = 1; i <= 3; ++i)
        lil_insert(i, i - 1, l);
    int pos[] = {1, 2, 5, 9};        /* linear 版要求升序 */
    char b1[64], b2[64];
    FILE *f = tmpfile();
    int s1 = 0;
    print_lots_simple(l, pos, 4, f, &s1);
    fflush(f); rewind(f);
    size_t n1 = fread(b1, 1, sizeof b1 - 1, f); b1[n1] = '\0';
    fclose(f);
    f = tmpfile();
    int s2 = 0;
    print_lots_linear(l, pos, 4, f, &s2);
    fflush(f); rewind(f);
    size_t n2 = fread(b2, 1, sizeof b2 - 1, f); b2[n2] = '\0';
    fclose(f);
    CHECK_EQ(strcmp(b1, "1 2"), 0);
    CHECK_EQ(strcmp(b1, b2), 0);
    CHECK_EQ(s1, 2);
    CHECK_EQ(s2, 2);    lil_dispose(l);
}

/* ============ 3.8 多项式的幂 ============ */
static void test_poly_pow(void)
{
    double c[] = {1.0, 1.0};                 /* 1 + x */
    Polynomial *p = poly_create(c, 1);
    CHECK(p != NULL);

    Polynomial *sq = poly_pow(p, 2);         /* 1 + 2x + x^2 */
    CHECK(sq != NULL);
    CHECK_EQ(sq->degree, 2);
    CHECK_NEAR(sq->coeff[0], 1.0, 1e-12);
    CHECK_NEAR(sq->coeff[1], 2.0, 1e-12);
    CHECK_NEAR(sq->coeff[2], 1.0, 1e-12);

    Polynomial *z = poly_pow(p, 0);          /* p^0 == 1 */
    CHECK(z != NULL);
    CHECK_EQ(z->degree, 0);
    CHECK_NEAR(z->coeff[0], 1.0, 1e-12);

    Polynomial *one = poly_pow(p, 1);        /* p^1 == p */
    CHECK(one != NULL);
    CHECK_EQ(one->degree, 1);
    CHECK_NEAR(poly_eval(one, 3.0), 4.0, 1e-12);

    /* 与"反复相乘"的朴素结果对拍，并用二项式系数校验 */
    for (int n = 0; n <= 12; ++n) {
        Polynomial *fast = poly_pow(p, n);
        Polynomial *slow = poly_one();
        for (int i = 0; i < n; ++i) {
            Polynomial *t = poly_mul(slow, p);
            poly_dispose(slow);
            slow = t;
        }
        CHECK(fast != NULL && slow != NULL);
        CHECK_EQ(fast->degree, slow->degree);
        int same = 1;
        for (int i = 0; i <= fast->degree; ++i)
            if (fast->coeff[i] != slow->coeff[i]) same = 0;
        CHECK(same);
        CHECK_NEAR(poly_eval(fast, 1.0), (double)(1 << n), 1e-9);  /* (1+1)^n */
        poly_dispose(fast);
        poly_dispose(slow);
    }

    poly_dispose(p);
    poly_dispose(sq);
    poly_dispose(z);
    poly_dispose(one);
}

/* ============ 3.9 任意精度整数 ============ */
static void test_bignum(void)
{
    /* 2^100 = 1267650600228229401496703205376 */
    BigInt *p = big_from_ll(1);
    BigInt *two = big_from_ll(2);
    for (int i = 0; i < 100; ++i) {
        BigInt *t = big_mul(p, two);
        big_dispose(p);
        p = t;
    }
    char *s = big_to_str(p);
    CHECK_EQ(strcmp(s, "1267650600228229401496703205376"), 0);
    free(s);

    /* 20! 与 64 位算术对拍 */
    BigInt *f = big_from_ll(1);
    unsigned long long ref = 1;
    for (int i = 2; i <= 20; ++i) {
        BigInt *m = big_from_ll(i);
        BigInt *t = big_mul(f, m);
        big_dispose(f); big_dispose(m);
        f = t;
        ref *= (unsigned long long)i;
    }
    s = big_to_str(f);
    CHECK_EQ(strtoull(s, NULL, 10), 2432902008176640000ULL);
    CHECK_EQ(ref, 2432902008176640000ULL);
    free(s);

    /* 100! 的尾零个数应为 24 = ⌊100/5⌋ + ⌊100/25⌋ */
    BigInt *h = big_from_ll(1);
    for (int i = 2; i <= 100; ++i) {
        BigInt *m = big_from_ll(i);
        BigInt *t = big_mul(h, m);
        big_dispose(h); big_dispose(m);
        h = t;
    }
    s = big_to_str(h);
    size_t len = strlen(s);
    int zeros = 0;
    while (zeros < (int)len && s[len - 1 - (size_t)zeros] == '0')
        ++zeros;
    CHECK_EQ(zeros, 24);
    free(s);

    /* 加减法与符号 */
    BigInt *a = big_from_str("-123456789012345678901234567890");
    BigInt *b = big_from_str("123456789012345678901234567890");
    BigInt *sum = big_add(a, b);
    BigInt *diff = big_sub(b, a);
    char *ss = big_to_str(sum);
    char *ds = big_to_str(diff);
    CHECK_EQ(strcmp(ss, "0"), 0);
    CHECK_EQ(strcmp(ds, "246913578024691357802469135780"), 0);
    CHECK_EQ(big_cmp(a, b), -1);
    CHECK_EQ(big_cmp(b, a), 1);
    CHECK_EQ(big_cmp(a, a), 0);
    free(ss); free(ds);
    big_dispose(sum); big_dispose(diff);

    /* 与 long long 随机对拍 */
    for (int trial = 0; trial < 300; ++trial) {
        long long x = (long long)ms_next(0, 1000000000) * (ms_next(0, 1) ? 1 : -1);
        long long y = (long long)ms_next(0, 1000000000) * (ms_next(0, 1) ? 1 : -1);
        BigInt *bx = big_from_ll(x), *by = big_from_ll(y);
        BigInt *badd = big_add(bx, by);
        BigInt *bsub = big_sub(bx, by);
        BigInt *bmul = big_mul(bx, by);
        char *xs = big_to_str(badd);
        char *ys = big_to_str(bsub);
        char *zs = big_to_str(bmul);
        char buf[64];
        snprintf(buf, sizeof buf, "%lld", x + y);
        CHECK_EQ(strcmp(xs, buf), 0);
        snprintf(buf, sizeof buf, "%lld", x - y);
        CHECK_EQ(strcmp(ys, buf), 0);
        snprintf(buf, sizeof buf, "%lld", x * y);
        CHECK_EQ(strcmp(zs, buf), 0);
        free(xs); free(ys); free(zs);
        big_dispose(bx); big_dispose(by);
        big_dispose(badd); big_dispose(bsub); big_dispose(bmul);
    }

    big_dispose(p); big_dispose(two); big_dispose(f); big_dispose(h);
    big_dispose(a); big_dispose(b);
    CHECK(big_from_str("12a") == NULL);      /* 非法输入 */
    CHECK(big_from_str("") == NULL);
}

/* ============ 3.11 递归查找 ============ */
static void test_recursive_find(void)
{
    ListLinked *l = lil_create();
    for (int i = 0; i < 50; ++i)
        lil_insert(i * 7, i, l);

    for (int v = 0; v < 50; ++v) {
        PtrToNode it = lil_find(v * 7, l);
        PtrToNode rc = lil_find_recursive(v * 7, l);
        CHECK(it == rc);                     /* 两个版本返回同一个结点 */
        CHECK(rc != NULL);
    }
    CHECK(lil_find_recursive(-1, l) == NULL);
    CHECK(lil_find_recursive(999, l) == NULL);
    CHECK(lil_find_recursive(3, NULL) == NULL);
    lil_dispose(l);
}

/* ============ 3.13 基数排序学生记录 ============ */
static void test_radix_records(void)
{
    /* 稳定性：相同 key 的记录必须保持输入顺序 */
    StudentRecord r[] = {
        {17, "a"}, {5, "b"}, {17, "c"}, {1000, "d"}, {5, "e"}, {0, "f"}, {999, "g"}
    };
    int n = (int)(sizeof r / sizeof r[0]);
    radix_sort_records(r, n);

    for (int i = 1; i < n; ++i)
        CHECK(r[i - 1].key <= r[i].key);

    int stable = 1;
    for (int i = 1; i < n; ++i)
        if (r[i - 1].key == r[i].key && strcmp(r[i - 1].name, r[i].name) >= 0)
            stable = 0;
    CHECK(stable);
    CHECK_EQ(r[0].key, 0);
    CHECK_EQ(r[n - 1].key, 1000);
    {
        int fives = 0;
        for (int i = 0; i < n; ++i)
            if (r[i].key == 5) { CHECK_EQ(r[i].name[0], fives == 0 ? 'b' : 'e'); ++fives; }
        CHECK_EQ(fives, 2);
    }

    /* 随机对拍（只比 key 是否有序） */
    for (int trial = 0; trial < 200; ++trial) {
        StudentRecord t[64];
        int m = rng_range(1, 64);
        for (int i = 0; i < m; ++i) {
            t[i].key = rng_range(0, 99999);
            t[i].name[0] = (char)('A' + (i % 26));
            t[i].name[1] = '\0';
        }
        radix_sort_records(t, m);
        for (int i = 1; i < m; ++i)
            CHECK(t[i - 1].key <= t[i].key);
    }
}

/* ============ 3.14 图 → 邻接表 ============ */
static void test_adjacency(void)
{
    /* 4 个顶点、5 条边的无向图，故意用乱序输入 */
    const char *text =
        "# test graph\n"
        "1 2\n"
        "2 3 7\n"
        "4 1 5\n"
        "3 4\n"
        "1 3 2\n";
    FILE *f = tmpfile();
    CHECK(f != NULL);
    fputs(text, f);
    rewind(f);
    AdjGraph *g = adj_read(f, 4);
    fclose(f);
    CHECK(g != NULL);
    CHECK_EQ(g->n, 4);
    CHECK_EQ(g->m, 10);                  /* 5 条无向边 = 10 条有向边 */

    int deg[5] = {0};
    for (int v = 1; v <= 4; ++v)
        for (AdjNode *p = g->head[v]; p != NULL; p = p->next)
            ++deg[v];
    CHECK_EQ(deg[1], 3);                 /* 1-2, 1-4, 1-3 */
    CHECK_EQ(deg[2], 2);
    CHECK_EQ(deg[3], 3);
    CHECK_EQ(deg[4], 2);

    /* 游标版必须与链表版完全一致（逐邻接项比对，含权重） */
    CursorAdjGraph *c = cadg_from_adj(g);
    CHECK(c != NULL);
    CHECK_EQ(c->n, g->n);
    CHECK_EQ(c->m, g->m);
    for (int v = 1; v <= g->n; ++v) {
        AdjNode *p = g->head[v];
        int e = c->head[v];
        int cnt_a = 0, cnt_c = 0;
        while (p != NULL || e != CURSOR_NULL) {
            CHECK(p != NULL && e != CURSOR_NULL);
            if (p == NULL || e == CURSOR_NULL)
                break;
            CHECK_EQ(p->vertex, c->edges[e].vertex);
            CHECK_EQ(p->weight, c->edges[e].weight);
            p = p->next;
            e = c->edges[e].next;
            ++cnt_a; ++cnt_c;
        }
        CHECK_EQ(cnt_a, cnt_c);
        CHECK_EQ(cnt_a, deg[v]);
    }

    /* 游标版空间用尽必须报错（这是它与链表版的关键差别） */
    CursorAdjGraph *small = cadg_create(3, 2);
    CHECK_EQ(cadg_add_edge(small, 1, 2, 1, 1), 0);
    CHECK_EQ(cadg_add_edge(small, 2, 3, 1, 1), -1);      /* 需要 2 个槽，只剩 0 */
    cadg_dispose(small);

    /* 越界顶点应被拒绝 */
    CHECK_EQ(adj_add_edge(g, 1, 9, 1, 0), -1);
    CHECK_EQ(adj_add_edge(g, 0, 1, 1, 0), -1);

    cadg_dispose(c);
    adj_dispose(g);
}

/* ============ 3.16 数组去重 ============ */
static void test_array_dedup(void)
{
    int a[] = {3, 1, 3, 2, 1, 3, 5};
    int b[7];
    memcpy(b, a, sizeof a);

    int m1 = remove_duplicates_slow(a, 7);      /* 保持首次出现顺序 */
    CHECK_EQ(m1, 4);
    CHECK_EQ(a[0], 3);
    CHECK_EQ(a[1], 1);
    CHECK_EQ(a[2], 2);
    CHECK_EQ(a[3], 5);

    int m2 = remove_duplicates_sort(b, 7);      /* 排序后去重 */
    CHECK_EQ(m2, 4);
    CHECK_EQ(b[0], 1);
    CHECK_EQ(b[1], 2);
    CHECK_EQ(b[2], 3);
    CHECK_EQ(b[3], 5);

    CHECK_EQ(remove_duplicates_slow(NULL, 0), 0);
    CHECK_EQ(remove_duplicates_sort(NULL, 0), 0);
    {
        int one[] = {7};
        CHECK_EQ(remove_duplicates_slow(one, 1), 1);
    }

    /* 随机对拍：两个版本得到的集合必须相同 */
    for (int trial = 0; trial < 300; ++trial) {
        int v[40], w[40];
        int n = rng_range(1, 40);
        for (int i = 0; i < n; ++i) {
            v[i] = w[i] = rng_range(0, 12);
        }
        int c1 = remove_duplicates_slow(v, n);
        int c2 = remove_duplicates_sort(w, n);
        CHECK_EQ(c1, c2);
        for (int i = 0; i < c1; ++i) {
            int hit = 0;
            for (int j = 0; j < c2; ++j)
                if (v[i] == w[j]) hit = 1;
            CHECK(hit);
        }
    }
}

/* ============ 3.17 懒惰删除 ============ */
static void test_lazy_deletion(void)
{
    LazyList *l = lz_create(4);
    CHECK(l != NULL);
    CHECK_EQ(lz_live(l), 0);
    CHECK_EQ(lz_deleted(l), 0);
    CHECK_EQ(lz_find(1, l), ELEMENT_NOT_FOUND);

    for (int i = 1; i <= 10; ++i)
        CHECK_EQ(lz_insert(i, l), 0);            /* 会扩容 */
    CHECK_EQ(lz_live(l), 10);
    CHECK_EQ(lz_deleted(l), 0);

    CHECK_EQ(lz_delete(4, l), 0);
    CHECK_EQ(lz_delete(7, l), 0);
    CHECK_EQ(lz_delete(4, l), -1);               /* 已删除的不能再删 */
    CHECK_EQ(lz_find(4, l), ELEMENT_NOT_FOUND);  /* 逻辑上已消失 */
    CHECK_EQ(lz_find(5, l) >= 0, 1);
    CHECK_EQ(lz_live(l), 8);

    /* 插入应优先复用已删除槽位（不增加槽位总数） */
    int slots_before = lz_slots(l);
    CHECK_EQ(lz_insert(99, l), 0);
    CHECK_EQ(lz_slots(l), slots_before);
    CHECK_EQ(lz_live(l), 9);
    CHECK(lz_find(99, l) >= 0);

    /* 大量删除必须触发整理，且整理后垃圾不再残留 */
    for (int i = 1; i <= 10; ++i)
        lz_delete(i, l);
    CHECK_EQ(lz_deleted(l), 0);                  /* 整理已发生 */
    CHECK(lz_compactions(l) >= 1);
    CHECK(lz_slots(l) <= 2);

    lz_dispose(l);

    /* 行为等价性：与朴素数组模型对拍 */
    LazyList *q = lz_create(2);
    int model[1024];
    int nm = 0;
    for (int step = 0; step < 1500; ++step) {
        if (nm == 0 || (nm < 1000 && rng_range(0, 2) != 0)) {
            int v = rng_range(0, 20);
            CHECK_EQ(lz_insert(v, q), 0);
            model[nm++] = v;
        } else {
            int idx = rng_range(0, nm - 1);
            int v = model[idx];
            int expect = -1;
            for (int i = 0; i < nm; ++i)
                if (model[i] == v) { expect = 0; break; }
            CHECK_EQ(lz_delete(v, q), expect);
            if (expect == 0) {                    /* 删掉一个 v 并维护模型 */
                for (int i = 0; i < nm; ++i)
                    if (model[i] == v) {
                        for (int j = i; j < nm - 1; ++j) model[j] = model[j + 1];
                        --nm;
                        break;
                    }
            }
        }
        CHECK_EQ(lz_live(q), nm);
        for (int i = 0; i < nm; ++i)
            CHECK(lz_find(model[i], q) >= 0);
    }
    lz_dispose(q);
}

/* ============ 3.23 一个数组实现三个栈 ============ */
static void test_three_stacks(void)
{
    ThreeStacks *t = ts3_create(9);              /* 每栈 3 个槽位 */
    CHECK(t != NULL);
    for (int k = 0; k < 3; ++k) {
        CHECK_EQ(ts3_capacity_share(t, k), 3);
        CHECK(ts3_is_empty(k, t));
        CHECK_EQ(ts3_size(k, t), 0);
    }

    for (int k = 0; k < 3; ++k)
        for (int i = 0; i < 3; ++i)
            CHECK_EQ(ts3_push(k, k * 10 + i, t), 0);

    for (int k = 0; k < 3; ++k) {
        CHECK(ts3_is_full(k, t));
        CHECK_EQ(ts3_size(k, t), 3);
        CHECK_EQ(ts3_push(k, 999, t), -1);       /* 本段已满 */
    }

    element_type v = 0;
    CHECK_EQ(ts3_pop(1, t, &v), 0);
    CHECK_EQ(v, 12);
    CHECK_EQ(ts3_size(1, t), 2);
    CHECK_EQ(ts3_push(1, 77, t), 0);
    CHECK(ts3_is_full(1, t));

    while (ts3_pop(0, t, &v) == 0) { }
    CHECK(ts3_is_empty(0, t));
    CHECK_EQ(ts3_pop(0, t, &v), -1);
    CHECK_EQ(ts3_pop(9, t, &v), -1);             /* 非法栈号 */
    CHECK_EQ(ts3_push(9, 1, t), -1);

    /* 交错压弹：三段互不干扰 */
    ThreeStacks *u = ts3_create(12);
    for (int step = 0; step < 500; ++step) {
        int k = rng_range(0, 2);
        if (rng_range(0, 1) || ts3_is_empty(k, u)) {
            int full = ts3_is_full(k, u);        /* 必须在 push 前取，否则参数求值顺序未定义 */
            CHECK_EQ(ts3_push(k, step, u), full ? -1 : 0);
        } else {
            CHECK_EQ(ts3_pop(k, u, &v), 0);
        }
        for (int j = 0; j < 3; ++j)
            CHECK(ts3_size(j, u) >= 0 && ts3_size(j, u) <= ts3_capacity_share(u, j));
    }
    ts3_dispose(u);
    ts3_dispose(t);
}

/* ============ 3.24 斐波那契递归的栈空间 ============ */
static void test_fib_stack_space(void)
{
    for (int n = 0; n <= 25; ++n)
        CHECK_EQ(fib_max_depth(n), n > 0 ? n : 1);   /* 栈帧数：n≥1 时为 n */

    /* 调用次数 = 2F(n+1) − 1，与直接数出的斐波那契对拍 */
    for (int n = 0; n <= 40; ++n) {
        unsigned long long f0 = 0, f1 = 1;
        for (int i = 0; i < n + 1; ++i) {
            unsigned long long t = f0 + f1;
            f0 = f1;
            f1 = t;
        }
        CHECK_EQ(fib_call_count(n), 2 * f0 - 1);  /* f0 = F(n+1) */
    }
    CHECK_EQ(fib_call_count(0), 1);
    CHECK_EQ(fib_call_count(1), 1);
    CHECK_EQ(fib_call_count(2), 3);
    CHECK_EQ(fib_call_count(3), 5);
    CHECK_EQ(fib_max_depth(-1), 0);
}

/* ============ 3.26 双端队列 ============ */
static void test_deque(void)
{
    Deque *d = deq_create(2);
    CHECK(deq_is_empty(d));
    CHECK_EQ(deq_size(d), 0);
    CHECK_EQ(deq_front(d), ELEMENT_NOT_FOUND);
    CHECK_EQ(deq_back(d), ELEMENT_NOT_FOUND);
    CHECK_EQ(deq_pop_front(d), ELEMENT_NOT_FOUND);
    CHECK_EQ(deq_pop_back(d), ELEMENT_NOT_FOUND);

    deq_push_back(1, d);
    deq_push_front(0, d);            /* [0, 1] */
    deq_push_back(2, d);
    deq_push_front(-1, d);           /* [-1, 0, 1, 2] */
    CHECK_EQ(deq_size(d), 4);
    CHECK_EQ(deq_front(d), -1);
    CHECK_EQ(deq_back(d), 2);
    CHECK_EQ(deq_pop_front(d), -1);  /* [0, 1, 2] */
    CHECK_EQ(deq_pop_back(d), 2);    /* [0, 1] */
    CHECK_EQ(deq_front(d), 0);
    CHECK_EQ(deq_back(d), 1);

    /* 环形回绕：反复从头部弹、从尾部压 */
    Deque *r = deq_create(4);
    for (int i = 0; i < 4; ++i) deq_push_back(i, r);
    for (int i = 0; i < 100; ++i) {
        CHECK_EQ(deq_pop_front(r), i);
        CHECK_EQ(deq_push_back(i + 4, r), 0);
        CHECK_EQ(deq_size(r), 4);
        CHECK_EQ(deq_back(r), i + 4);
    }
    for (int i = 0; i < 4; ++i) CHECK_EQ(deq_pop_front(r), 100 + i);
    CHECK(deq_is_empty(r));

    /* 与朴素数组模型对拍（两端操作混合） */
    for (int trial = 0; trial < 50; ++trial) {
        Deque *q = deq_create(1);
        int model[700];
        int lo = 350, hi = 350;              /* model[lo..hi) 为当前内容 */
        for (int step = 0; step < 300; ++step) {
            int op = rng_range(0, 3);
            if (op == 0 || lo == hi) {
                int v = rng_range(-99, 99);
                CHECK_EQ(deq_push_back(v, q), 0);
                model[hi++] = v;
            } else if (op == 1) {
                int v = rng_range(-99, 99);
                CHECK_EQ(deq_push_front(v, q), 0);
                model[--lo] = v;
            } else if (op == 2) {
                CHECK_EQ(deq_front(q), model[lo]);
                CHECK_EQ(deq_pop_front(q), model[lo++]);
            } else {
                CHECK_EQ(deq_back(q), model[hi - 1]);
                CHECK_EQ(deq_pop_back(q), model[--hi]);
            }
            CHECK_EQ(deq_size(q), hi - lo);
        }
        deq_dispose(q);
    }
    deq_make_empty(d);
    CHECK(deq_is_empty(d));
    deq_dispose(d);
    deq_dispose(r);
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
    test_print_lots();
    test_poly_pow();
    test_bignum();
    test_recursive_find();
    test_radix_records();
    test_adjacency();
    test_array_dedup();
    test_lazy_deletion();
    test_three_stacks();
    test_fib_stack_space();
    test_deque();
    return TEST_REPORT();
}
