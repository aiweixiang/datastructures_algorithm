/* 第 4 章：伸展树（4.23 ~ 4.27）。自底向上实现，结点带 parent 指针。 */
#include <stdlib.h>
#include "ch04.h"

static SplayNode *new_node(int x)
{
    SplayNode *n = malloc(sizeof *n);
    if (n == NULL)
        return NULL;
    n->elem = x;
    n->left = n->right = n->parent = NULL;
    return n;
}

SplayTree *splay_create(void)
{
    SplayTree *t = malloc(sizeof *t);
    if (t == NULL)
        return NULL;
    t->root = NULL;
    t->size = 0;
    t->rotations = 0;
    return t;
}

void splay_dispose(SplayTree *t)
{
    if (t == NULL)
        return;
    SplayNode **stack = malloc(sizeof *stack * (size_t)(t->size * 2 + 16));
    int sp = 0;
    if (stack == NULL) { free(t); return; }
    if (t->root != NULL)
        stack[sp++] = t->root;
    while (sp > 0) {
        SplayNode *n = stack[--sp];
        if (n->left)  stack[sp++] = n->left;
        if (n->right) stack[sp++] = n->right;
        free(n);
    }
    free(stack);
    free(t);
}

/* 以 x 为轴左旋（x 必须有右孩子） */
static void rotate_left(SplayTree *t, SplayNode *x)
{
    SplayNode *y = x->right;
    x->right = y->left;
    if (y->left != NULL)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == NULL)
        t->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
    ++t->rotations;
}

static void rotate_right(SplayTree *t, SplayNode *x)
{
    SplayNode *y = x->left;
    x->left = y->right;
    if (y->right != NULL)
        y->right->parent = x;
    y->parent = x->parent;
    if (x->parent == NULL)
        t->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->right = x;
    x->parent = y;
    ++t->rotations;
}

/* 把 x 通过 zig / zig-zig / zig-zag 旋转到根（4.23） */
static void splay_to_root(SplayTree *t, SplayNode *x)
{
    while (x->parent != NULL) {
        SplayNode *p = x->parent;
        SplayNode *g = p->parent;
        if (g == NULL) {                                  /* zig */
            if (x == p->left) rotate_right(t, p);
            else              rotate_left(t, p);
        } else if (x == p->left && p == g->left) {         /* zig-zig */
            rotate_right(t, g);
            rotate_right(t, p);
        } else if (x == p->right && p == g->right) {       /* zig-zig（对称） */
            rotate_left(t, g);
            rotate_left(t, p);
        } else if (x == p->left && p == g->right) {        /* zig-zag */
            rotate_right(t, p);
            rotate_left(t, g);
        } else {                                           /* zig-zag（对称） */
            rotate_left(t, p);
            rotate_right(t, g);
        }
    }
}

/* 4.23：访问 x —— 命中则把该结点伸展到根；未命中则把"最后访问到的结点"伸展到根 */
SplayNode *splay_access(int x, SplayTree *t)
{
    if (t == NULL || t->root == NULL)
        return NULL;
    SplayNode *p = t->root, *last = t->root;
    while (p != NULL) {
        last = p;
        if (x < p->elem) p = p->left;
        else if (x > p->elem) p = p->right;
        else break;
    }
    splay_to_root(t, last);
    return (last->elem == x) ? last : NULL;
}

int splay_contains(int x, SplayTree *t)
{
    return splay_access(x, t) != NULL;
}

int splay_insert(int x, SplayTree *t)
{
    if (t == NULL)
        return -1;
    if (t->root == NULL) {
        t->root = new_node(x);
        if (t->root == NULL)
            return -1;
        ++t->size;
        return 0;
    }
    SplayNode *p = t->root, *parent = NULL;
    while (p != NULL) {
        parent = p;
        if (x < p->elem) p = p->left;
        else if (x > p->elem) p = p->right;
        else {                                   /* 已存在：伸展即可 */
            splay_to_root(t, p);
            return -1;
        }
    }
    SplayNode *n = new_node(x);
    if (n == NULL)
        return -1;
    n->parent = parent;
    if (x < parent->elem) parent->left = n;
    else                  parent->right = n;
    ++t->size;
    splay_to_root(t, n);                         /* 新结点伸展到根 */
    return 0;
}

/* 4.24 删除：先访问（把目标伸展到根），再合并左右子树 */
int splay_delete(int x, SplayTree *t)
{
    if (t == NULL || t->root == NULL)
        return -1;
    SplayNode *n = splay_access(x, t);
    if (n == NULL)
        return -1;
    SplayNode *L = n->left, *R = n->right;
    if (L != NULL) L->parent = NULL;
    if (R != NULL) R->parent = NULL;
    free(n);

    if (L == NULL) {
        t->root = R;
    } else {
        /* 把 L 中最大元伸展到 L 的根，再挂上 R */
        SplayNode *m = L;
        while (m->right != NULL)
            m = m->right;
        t->root = L;
        splay_to_root(t, m);
        t->root->right = R;
        if (R != NULL)
            R->parent = t->root;
    }
    --t->size;
    return 0;
}

int splay_size(const SplayTree *t) { return t ? t->size : 0; }

static int height_rec(const SplayNode *n)
{
    if (n == NULL)
        return -1;
    int l = height_rec(n->left), r = height_rec(n->right);
    return (l > r ? l : r) + 1;
}

int splay_height(const SplayTree *t) { return t ? height_rec(t->root) : -1; }

static int bst_ok(const SplayNode *n, const int *lo, const int *hi, const SplayNode *parent)
{
    if (n == NULL)
        return 1;
    if (n->parent != parent)
        return 0;                       /* parent 指针也必须正确 */
    if (lo != NULL && n->elem <= *lo) return 0;
    if (hi != NULL && n->elem >= *hi) return 0;
    return bst_ok(n->left, lo, &n->elem, n) && bst_ok(n->right, &n->elem, hi, n);
}

int splay_is_bst(const SplayTree *t)
{
    return t == NULL ? 1 : bst_ok(t->root, NULL, NULL, NULL);
}

long long splay_rotations(const SplayTree *t) { return t ? t->rotations : 0; }

static void pre_rec(const SplayNode *n, FILE *fp)
{
    if (n == NULL) return;
    fprintf(fp, "%d ", n->elem);
    pre_rec(n->left, fp);
    pre_rec(n->right, fp);
}

static void in_rec(const SplayNode *n, FILE *fp)
{
    if (n == NULL) return;
    in_rec(n->left, fp);
    fprintf(fp, "%d ", n->elem);
    in_rec(n->right, fp);
}

void splay_preorder(const SplayTree *t, FILE *fp) { if (t) pre_rec(t->root, fp); }
void splay_inorder(const SplayTree *t, FILE *fp) { if (t) in_rec(t->root, fp); }

/* 4.25：手工构造 n 个结点的**左链**（每层只有左儿子），返回访问最小元的路径长度，
 * 并把路径上的结点依次伸展（统计旋转次数）。用来展示"单次访问可能是 O(N)"。 */
int splay_chain_path_length(int n)
{
    if (n <= 0)
        return 0;
    SplayTree *t = splay_create();
    if (t == NULL)
        return 0;
    /* 递增插入 1..n：每个新结点都是当前最大元并被伸展到根，
     * 于是更小的元素全部落在左侧，最终形成一条长度为 n 的左链。
     * （若递减插入，新结点是最小元且被伸展到根，留下的是一条右链。） */
    for (int i = 1; i <= n; ++i)
        splay_insert(i, t);

    int path = 0;
    SplayNode *p = t->root;
    while (p != NULL) {                  /* 走最左路径 = 访问最小值 */
        ++path;
        p = p->left;
    }
    (void)splay_access(1, t);            /* 访问最小值并伸展 */
    splay_dispose(t);
    return path;
}

/* 4.26：在一棵随机建立的伸展树上顺序访问 1..n，返回总旋转次数。
 * 顺序访问界（Tarjan）说这是 O(n)，测试用它做经验验证。 */
long long splay_sequential_access_rotations(int n)
{
    SplayTree *t = splay_create();
    if (t == NULL)
        return 0;
    unsigned long st = 12345UL;
    for (int i = 0; i < n; ++i) {
        st = (st * 1103515245UL + 12345UL) & 0x7fffffffUL;
        splay_insert((int)(st % (unsigned long)(n * 4 + 1)), t);
    }
    t->rotations = 0;                    /* 只统计访问阶段的旋转 */
    for (int i = 1; i <= n; ++i)
        splay_access(i, t);
    long long r = t->rotations;
    splay_dispose(t);
    return r;
}
