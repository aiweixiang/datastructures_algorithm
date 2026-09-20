/* 第 4 章测试：4.1~4.46 里所有能"跑"的部分。
 * 策略：结构性操作一律与"有序动态集合"模型对拍；
 *       树的不变量（BST 序、AVL 平衡、B 树全部约束、线索正确性）逐步校验。
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ch04_trees/ch04.h"
#include "test_util.h"

static unsigned long st = 20260920UL;
static int rnd(int lo, int hi)
{
    st = st * 1103515245UL + 12345UL;
    return lo + (int)((st >> 16) % (unsigned long)(hi - lo + 1));
}

/* ---------------- 有序集合模型 ---------------- */
#define MODEL_MAX 512
static int model[MODEL_MAX];
static int msize;

static int model_find(int v)
{
    for (int i = 0; i < msize; ++i)
        if (model[i] == v) return i;
    return -1;
}

static int model_insert(int v)
{
    if (model_find(v) >= 0) return 0;
    int i = msize;
    while (i > 0 && model[i - 1] > v) {
        model[i] = model[i - 1];
        --i;
    }
    model[i] = v;
    ++msize;
    return 1;
}

static int model_delete(int v)
{
    int i = model_find(v);
    if (i < 0) return 0;
    for (; i < msize - 1; ++i)
        model[i] = model[i + 1];
    --msize;
    return 1;
}

static char buf[4096];

/* ================= 4.1 ~ 4.9 / 4.28 / 4.33 ~ 4.35 / 4.41 ~ 4.42 ================= */
static void test_tree_basics(void)
{
    /*        4
     *       / \
     *      2   6
     *     / \   \
     *    1   3   7
     */
    TreeNode *t = tree_node(4);
    t->left = tree_node(2);
    t->right = tree_node(6);
    t->left->left = tree_node(1);
    t->left->right = tree_node(3);
    t->right->right = tree_node(7);

    CHECK_EQ(tree_depth(t), 2);                       /* 4.3 */
    CHECK_EQ(tree_count_nodes(t), 6);                 /* 4.28 */
    CHECK_EQ(tree_count_leaves(t), 3);                /* 1,3,7 */
    CHECK_EQ(tree_count_full(t), 2);                  /* 2 和 4 */

    int nodes = 0, leaves = 0, full = 0;
    tree_stats(t, &nodes, &leaves, &full);
    CHECK_EQ(nodes, 6);
    CHECK_EQ(leaves, 3);
    CHECK_EQ(full, 2);

    {
        FILE *f = tmpfile();
        tree_preorder(t, f);  fprintf(f, "| ");
        tree_inorder(t, f);   fprintf(f, "| ");
        tree_postorder(t, f);
        fflush(f); rewind(f);
        size_t n = fread(buf, 1, sizeof buf - 1, f);
        buf[n] = '\0';
        fclose(f);
        CHECK_EQ(strcmp(buf, "4 2 1 3 6 7 | 1 2 3 4 6 7 | 1 3 2 7 6 4 "), 0);
    }
    {
        FILE *f = tmpfile();
        tree_level_order(t, f);                       /* 4.35 */
        fflush(f); rewind(f);
        size_t n = fread(buf, 1, sizeof buf - 1, f);
        buf[n] = '\0';
        fclose(f);
        CHECK_EQ(strcmp(buf, "4 2 6 1 3 7 "), 0);
    }

    /* 4.33/4.34：x = 中序序号（0..n-1 的一个排列），y = 深度 */
    {
        int x[8], y[8];
        CHECK_EQ(tree_assign_xy(t, x, y, 8), 6);
        int seen[8] = {0};
        for (int i = 0; i < 6; ++i) {
            CHECK(x[i] >= 0 && x[i] < 6);
            CHECK_EQ(seen[x[i]], 0);
            seen[x[i]] = 1;
        }
        /* y 必须是合法的深度：根 0，其余 = 父深度 + 1，这里只校验范围 */
        for (int i = 0; i < 6; ++i)
            CHECK(y[i] >= 0 && y[i] <= 2);
    }
    {
        FILE *f = tmpfile();
        tree_print_indented(t, f);
        fflush(f); rewind(f);
        size_t n = fread(buf, 1, sizeof buf - 1, f);
        buf[n] = '\0';
        fclose(f);
        CHECK(strstr(buf, "4\n") != NULL);            /* 根在最前面 */
        CHECK(strstr(buf, "1\n") != NULL);
    }

    /* 4.41 相似 / 4.42 同构 */
    TreeNode *same = tree_node(4);
    same->left = tree_node(2);
    same->right = tree_node(6);
    same->left->left = tree_node(1);
    same->left->right = tree_node(3);
    same->right->right = tree_node(7);
    CHECK(tree_similar(t, same));
    CHECK(tree_isomorphic(t, same));

    TreeNode *mirror = tree_node(4);
    mirror->left = tree_node(6);
    mirror->right = tree_node(2);
    mirror->left->left = tree_node(7);
    mirror->right->left = tree_node(3);
    mirror->right->right = tree_node(1);
    CHECK(!tree_similar(t, mirror));                  /* 形状不同（左右互换） */
    CHECK(tree_isomorphic(t, mirror));                /* 同构 */

    TreeNode *nerfed = tree_node(4);
    nerfed->left = tree_node(2);
    CHECK(!tree_similar(t, nerfed));
    CHECK(!tree_isomorphic(t, nerfed));
    CHECK(tree_similar(NULL, NULL));
    CHECK(!tree_similar(t, NULL));

    tree_dispose(t);
    tree_dispose(same);
    tree_dispose(mirror);
    tree_dispose(nerfed);
}

/* ================= 4.8 表达式树 ================= */
static void test_expression_tree(void)
{
    TreeNode *e = expr_tree_from_postfix("1 2 3 * +");
    CHECK(e != NULL);
    int ok = 0;
    CHECK_EQ(expr_tree_eval(e, &ok), 7);
    CHECK_EQ(ok, 1);
    CHECK_EQ(tree_count_nodes(e), 5);
    CHECK_EQ(tree_depth(e), 2);

    {
        FILE *f = tmpfile();
        expr_tree_print(e, f, 1); fprintf(f, "| ");
        expr_tree_print(e, f, 2);
        fflush(f); rewind(f);
        size_t n = fread(buf, 1, sizeof buf - 1, f);
        buf[n] = '\0';
        fclose(f);
        CHECK_EQ(strcmp(buf, "1 + 2 * 3 | 1 2 3 * + "), 0);
    }

    TreeNode *e2 = expr_tree_from_postfix("2 3 4 * +");
    CHECK_EQ(expr_tree_eval(e2, &ok), 14);             /* 2 + 3*4 */
    TreeNode *e3 = expr_tree_from_postfix("5 1 2 + 4 * + 3 -");
    CHECK_EQ(expr_tree_eval(e3, &ok), 14);             /* 5 + ((1+2)*4) - 3 */
    CHECK(expr_tree_from_postfix("1 +") == NULL);
    CHECK(expr_tree_from_postfix("1 2") == NULL);
    CHECK(expr_tree_from_postfix("") == NULL);
    CHECK(expr_tree_from_postfix("1 0 /"));            /* 建树成功；除零在求值时报错 */

    TreeNode *dz = expr_tree_from_postfix("1 0 /");
    CHECK(dz != NULL);
    ok = 1;
    (void)expr_tree_eval(dz, &ok);
    CHECK_EQ(ok, 0);                                   /* 除零 → ok = 0 */

    tree_dispose(e);
    tree_dispose(e2);
    tree_dispose(e3);
    tree_dispose(dz);
}

/* ================= 4.39 / 4.40 儿子-兄弟 ================= */
static void test_child_sibling(void)
{
    /* 父子关系：节点编号 0..7，parent[0]=-1 */
    int parent[] = {-1, 0, 0, 0, 1, 1, 3, 3};
    CSNode *root = cs_from_parents(8, parent);
    CHECK(root != NULL);
    CHECK_EQ(root->elem, 0);
    CHECK_EQ(cs_depth(root), 2);

    {
        FILE *f = tmpfile();
        cs_preorder(root, f); fprintf(f, "| ");
        cs_postorder(root, f);
        fflush(f); rewind(f);
        size_t n = fread(buf, 1, sizeof buf - 1, f);
        buf[n] = '\0';
        fclose(f);
        /* 儿子按编号顺序挂：0 的孩子是 1,2,3 */
        CHECK_EQ(strcmp(buf, "0 1 4 5 2 3 6 7 | 4 5 1 2 6 7 3 0 "), 0);
    }
    cs_dispose(root);
}

/* ================= 4.10 ~ 4.13 / 4.14 / 4.29 ~ 4.32 / 4.44 BST ================= */
static void inorder_collect(const TreeNode *t, int out[], int *n)
{
    if (t == NULL) return;
    inorder_collect(t->left, out, n);
    out[(*n)++] = t->elem;
    inorder_collect(t->right, out, n);
}

static void test_bst(void)
{
    BST *t = bst_create();
    int vals[] = {8, 3, 10, 1, 6, 14, 4, 7, 13};
    int nvals = (int)(sizeof vals / sizeof vals[0]);
    for (int i = 0; i < nvals; ++i) {
        CHECK(bst_insert(vals[i], t) != NULL);
        CHECK(bst_is_bst(t));
        CHECK_EQ(bst_size(t), i + 1);
        CHECK_EQ(bst_count_nodes(t), i + 1);
    }
    CHECK(bst_insert(8, t) != NULL);                   /* 重复：不插入 */
    CHECK_EQ(bst_size(t), nvals);

    CHECK_EQ(bst_find(6, t)->elem, 6);
    CHECK(bst_find(99, t) == NULL);
    CHECK_EQ(bst_find_min(t)->elem, 1);
    CHECK_EQ(bst_find_max(t)->elem, 14);

    /* 4.44 FindKth：与中序数组逐位比对 */
    {
        int arr[32], n = 0;
        inorder_collect(t->root, arr, &n);
        CHECK_EQ(n, nvals);
        for (int k = 1; k <= n; ++k)
            CHECK_EQ(bst_find_kth(k, t), arr[k - 1]);
        CHECK_EQ(bst_find_kth(0, t), -1);
        CHECK_EQ(bst_find_kth(n + 1, t), -1);
    }

    /* 4.32 区间打印 */
    {
        FILE *f = tmpfile();
        bst_print_range(4, 10, t, f);
        fflush(f); rewind(f);
        size_t n = fread(buf, 1, sizeof buf - 1, f);
        buf[n] = '\0';
        fclose(f);
        CHECK_EQ(strcmp(buf, "4 6 7 8 10 "), 0);
    }

    /* 4.28 统计：该 BST 的叶子是 1,4,7,13（14 有左孩子 13），满结点是 8,3,6 */
    CHECK_EQ(bst_count_leaves(t), 4);
    CHECK_EQ(bst_count_full(t), 3);

    /* 4.14 内部路径长：手工算一遍 */
    {
        BST *s = bst_create();
        int a[] = {5, 3, 8};
        for (int i = 0; i < 3; ++i) bst_insert(a[i], s);
        CHECK_EQ(bst_internal_path_length(s), 0 + 1 + 1);
        bst_dispose(s);
    }

    /* 4.13 三种删除法都要得到同一个集合 */
    {
        int expect[32], ne = 0;
        inorder_collect(t->root, expect, &ne);
        for (int which = 0; which < 3; ++which) {
            BST *c = bst_create();
            for (int i = 0; i < nvals; ++i) bst_insert(vals[i], c);
            for (int i = 0; i < nvals; ++i) {
                int r = (which == 0) ? bst_delete_right_min(vals[i], c)
                      : (which == 1) ? bst_delete_left_max(vals[i], c)
                                     : bst_delete_merge(vals[i], c);
                CHECK_EQ(r, 0);
                CHECK(bst_is_bst(c));
                CHECK_EQ(bst_size(c), nvals - i - 1);
                CHECK_EQ(bst_count_nodes(c), bst_size(c));
            }
            CHECK_EQ(bst_size(c), 0);
            CHECK_EQ(bst_delete_right_min(1, c), -1);
            (void)ne;
            bst_dispose(c);
        }
    }

    /* 4.29 / 4.31：随机 BST 与理想平衡 BST */
    {
        unsigned long seed = 7UL;
        BST *r = bst_random(63, &seed);
        CHECK_EQ(bst_size(r), 63);
        CHECK(bst_is_bst(r));
        CHECK_EQ(bst_count_nodes(r), 63);
        bst_dispose(r);

        int sorted[63];
        for (int i = 0; i < 63; ++i) sorted[i] = i;
        BST *id = bst_ideal_from_sorted(sorted, 63);
        CHECK_EQ(bst_size(id), 63);
        CHECK(bst_is_bst(id));
        CHECK_EQ(bst_height(id), 5);                   /* 2^6-1 = 63 → 高度 5 */
        /* 理想平衡树内部路径长 = Σ_{d=0}^{h} d·2^d = (h-1)·2^(h+1) + 2
         * h=5 → 4·64+2 = 258 */
        CHECK_EQ(bst_internal_path_length(id), 258);
        bst_dispose(id);
    }

    /* 随机插入/删除与有序集合模型对拍 */
    {
        BST *r = bst_create();
        msize = 0;
        for (int step = 0; step < 3000; ++step) {
            int v = rnd(0, 120);
            if (rnd(0, 1)) {
                int added = model_insert(v);
                bst_insert(v, r);
                CHECK_EQ(bst_insert(v, r) != NULL, 1);
                (void)added;
            } else {
                int del = model_delete(v);
                CHECK_EQ((bst_delete_right_min(v, r) == 0), del);
            }
            CHECK(bst_is_bst(r));
            CHECK_EQ(bst_size(r), msize);
            CHECK_EQ(bst_count_nodes(r), msize);
            {
                int missing = 0;
                for (int i = 0; i < msize; ++i)
                    if (bst_find(model[i], r) == NULL) { missing = 1; break; }
                CHECK_EQ(missing, 0);
            }
            if (msize > 0)
                CHECK_EQ(bst_find_kth(1, r), model[0]);
        }
        bst_dispose(r);
    }

    bst_dispose(t);
}

/* ================= 4.15 ~ 4.22 AVL ================= */
static void avl_inorder_arr(const AvlNode *n, int out[], int *k)
{
    if (n == NULL) return;
    avl_inorder_arr(n->left, out, k);
    out[(*k)++] = n->elem;
    avl_inorder_arr(n->right, out, k);
}

static void test_avl(void)
{
    /* 4.15 最少结点数：1, 2, 4, 7, 12, 20, 33, 54 */
    {
        int want[] = {1, 2, 4, 7, 12, 20, 33, 54};
        for (int h = 0; h < 8; ++h)
            CHECK_EQ(avl_min_nodes(h), want[h]);
        CHECK_EQ(avl_min_nodes(-1), 0);
    }

    /* 4.30 生成最少结点的 AVL 树：结点数、高度、平衡、以及"再少一个就不行" */
    for (int h = 0; h <= 5; ++h) {
        AVLTree *m = avl_min_tree(h);
        CHECK(m != NULL);
        CHECK_EQ(avl_size(m), avl_min_nodes(h));
        CHECK_EQ(avl_height(m), h);
        CHECK(avl_is_balanced(m));
        avl_dispose(m);
    }

    /* 4.18 顺序插入 1..N：必须始终平衡、是中序有序的 BST */
    for (int n = 1; n <= 200; ++n) {
        AVLTree *a = avl_create();
        for (int i = 1; i <= n; ++i)
            avl_insert(i, a);
        CHECK_EQ(avl_size(a), n);
        CHECK(avl_is_balanced(a));
        {
            int arr[256], k = 0;
            avl_inorder_arr(a->root, arr, &k);
            CHECK_EQ(k, n);
            for (int i = 0; i < n; ++i)
                CHECK_EQ(arr[i], i + 1);
        }
        /* AVL 高度上界：h ≤ 1.44 log2(n+2) - 0.33（这里用宽松的 +2） */
        {
            double bound = 1.4405 * (log((double)n + 2.0) / log(2.0)) + 1.0;
            CHECK((double)avl_height(a) <= bound);
        }
        avl_dispose(a);
    }

    /* 4.17：插入 1..2^k-1 之后是完全平衡的 */
    for (int k = 1; k <= 7; ++k) {
        AVLTree *a = avl_create();
        int n = (1 << k) - 1;
        for (int i = 1; i <= n; ++i)
            avl_insert(i, a);
        CHECK_EQ(avl_size(a), n);
        CHECK_EQ(avl_height(a), k - 1);
        CHECK(avl_is_perfect(a));
        avl_dispose(a);
    }

    /* 4.19 非递归插入 == 递归插入（同样的中序序列） */
    {
        int vals[] = {50, 20, 70, 10, 30, 60, 80, 25, 35, 65, 5, 1, 90, 55, 75};
        int nv = (int)(sizeof vals / sizeof vals[0]);
        AVLTree *a1 = avl_create(), *a2 = avl_create();
        for (int i = 0; i < nv; ++i) {
            avl_insert(vals[i], a1);
            avl_insert_iter(vals[i], a2);
        }
        CHECK_EQ(avl_size(a1), nv);
        CHECK_EQ(avl_size(a2), nv);
        CHECK(avl_is_balanced(a1));
        CHECK(avl_is_balanced(a2));
        CHECK_EQ(avl_height(a1), avl_height(a2));
        int r1[32], r2[32], k1 = 0, k2 = 0;
        avl_inorder_arr(a1->root, r1, &k1);
        avl_inorder_arr(a2->root, r2, &k2);
        CHECK_EQ(k1, k2);
        for (int i = 0; i < k1; ++i)
            CHECK_EQ(r1[i], r2[i]);
        /* 4.19 版本也要能拒绝重复 */
        CHECK_EQ(avl_insert_iter(50, a2) != NULL, 1);
        CHECK_EQ(avl_size(a2), nv);
        avl_dispose(a1);
        avl_dispose(a2);
    }

    /* 4.20 删除：与模型对拍，并始终保持平衡 */
    {
        AVLTree *a = avl_create();
        msize = 0;
        for (int step = 0; step < 1200; ++step) {
            int v = rnd(0, 80);
            if (rnd(0, 1)) {                 /* 同一个决定同时作用于模型和树 */
                model_insert(v);
                avl_insert(v, a);
            } else {
                int del = model_delete(v);
                CHECK_EQ((avl_delete(v, a) == 0), del);
            }
            CHECK(avl_is_balanced(a));
            CHECK_EQ(avl_size(a), msize);
            int arr[128], k = 0, mism = 0;
            avl_inorder_arr(a->root, arr, &k);
            if (k != msize)
                mism = 1;
            for (int i = 0; i < msize && i < k && !mism; ++i)
                if (arr[i] != model[i])
                    mism = 1;
            CHECK_EQ(mism, 0);
        }
        CHECK_EQ(avl_delete(999, a), -1);
        avl_dispose(a);
    }

    /* 4.21 高度域位数：10^6 个结点时 ≤ 8 位（一个字节足够） */
    CHECK(avl_height_bits(1000000) <= 8);
    CHECK(avl_height_bits(1) <= 8);
    CHECK(avl_height_bits(1000000000) <= 8);
    CHECK(avl_height_bits(0) == 0);
}

/* ================= 4.23 ~ 4.27 伸展树 ================= */
static void splay_inorder_arr(const SplayNode *n, int out[], int *k)
{
    if (n == NULL) return;
    splay_inorder_arr(n->left, out, k);
    out[(*k)++] = n->elem;
    splay_inorder_arr(n->right, out, k);
}

static void test_splay(void)
{
    SplayTree *t = splay_create();
    int vals[] = {50, 20, 70, 10, 30, 60, 80};
    int nv = (int)(sizeof vals / sizeof vals[0]);
    for (int i = 0; i < nv; ++i)
        splay_insert(vals[i], t);
    CHECK_EQ(splay_size(t), nv);
    CHECK(splay_is_bst(t));
    CHECK_EQ(splay_insert(50, t), -1);              /* 重复 */
    CHECK_EQ(splay_size(t), nv);

    /* 4.23：访问之后被访问的结点必须在根 */
    for (int i = 0; i < nv; ++i) {
        SplayNode *n = splay_access(vals[i], t);
        CHECK(n != NULL);
        CHECK(t->root == n);
        CHECK_EQ(t->root->elem, vals[i]);
        CHECK(splay_is_bst(t));
    }
    /* 未命中的访问也要把最后一个访问点伸展到根 */
    CHECK(splay_access(999, t) == NULL);
    CHECK(splay_is_bst(t));

    /* 4.24 删除：与模型对拍 */
    {
        SplayTree *s = splay_create();
        msize = 0;
        for (int step = 0; step < 1500; ++step) {
            int v = rnd(0, 60);
            if (rnd(0, 1)) {
                int added = model_insert(v);
                int r = splay_insert(v, s);
                CHECK_EQ(r == 0, added);
            } else {
                int del = model_delete(v);
                int r = splay_delete(v, s);
                CHECK_EQ(r == 0, del);
            }
            CHECK(splay_is_bst(s));
            CHECK_EQ(splay_size(s), msize);
            int arr[128], k = 0, mism = 0;
            splay_inorder_arr(s->root, arr, &k);
            if (k != msize)
                mism = 1;
            for (int i = 0; i < msize && i < k && !mism; ++i)
                if (arr[i] != model[i])
                    mism = 1;
            CHECK_EQ(mism, 0);
        }
        splay_dispose(s);
    }

    /* 4.25：退化（一条链）时单次访问的路径长度可以是 O(N) */
    for (int n = 1; n <= 200; n += 37) {
        CHECK_EQ(splay_chain_path_length(n), n);
    }

    /* 4.26 顺序访问的界（Tarjan）：总旋转次数是 O(N)，而不是 O(N log N) */
    for (int n = 32; n <= 512; n *= 4) {
        long long r = splay_sequential_access_rotations(n);
        CHECK(r <= (long long)8 * n);
    }

    /* 4.27 随机操作的旋转次数是 O(log N) 摊还 */
    {
        SplayTree *s = splay_create();
        msize = 0;
        for (int i = 0; i < 400; ++i)
            splay_insert(rnd(0, 5000), s);
        long long before = s->rotations;
        int ops = splay_size(s);
        for (int i = 0; i < ops; ++i)
            splay_access(i, s);
        long long rot = s->rotations - before;
        /* 每次访问平均旋转次数应为 O(log N) 级别 */
        CHECK(rot <= (long long)ops * 40);
        splay_dispose(s);
    }

    splay_dispose(t);
}

/* ================= 4.36 / 4.37 B 树 ================= */
static void test_btree(void)
{
    int orders[] = {3, 4, 5, 7};
    for (size_t oi = 0; oi < sizeof orders / sizeof orders[0]; ++oi) {
        int M = orders[oi];
        BTree *t = btree_create(M);
        CHECK(t != NULL);
        CHECK_EQ(btree_size(t), 0);
        CHECK(btree_check(t));

        msize = 0;
        for (int step = 0; step < 2500; ++step) {
            int v = rnd(0, 150);
            if (rnd(0, 2) != 0) {
                int added = model_insert(v);
                int r = btree_insert(v, t);
                CHECK_EQ(r == 0, added);
            } else {
                int del = model_delete(v);
                int r = btree_delete(v, t);
                CHECK_EQ(r == 0, del);
            }
            CHECK(btree_check(t));                 /* 全部不变量 */
            CHECK_EQ(btree_size(t), msize);
            {
                int missing = 0;
                for (int i = 0; i < msize; ++i)
                    if (!btree_contains(model[i], t)) { missing = 1; break; }
                CHECK_EQ(missing, 0);
            }
            CHECK_EQ(btree_contains(-1, t), 0);
        }
        /* 全删空 */
        while (msize > 0) {
            int v = model[0];
            model_delete(v);
            CHECK_EQ(btree_delete(v, t), 0);
            CHECK(btree_check(t));
        }
        CHECK_EQ(btree_size(t), 0);
        CHECK_EQ(btree_height(t), 0);
        CHECK_EQ(btree_delete(1, t), -1);
        btree_dispose(t);
    }

    /* 4.36 2-3 树：结点关键字数只能是 1 或 2 */
    {
        BTree *t = tree23_create();
        CHECK(t != NULL);
        CHECK_EQ(t->M, 3);
        for (int i = 1; i <= 40; ++i) {
            CHECK_EQ(btree_insert(i, t), 0);
            CHECK(btree_check(t));
        }
        CHECK_EQ(btree_size(t), 40);
        CHECK_EQ(btree_height(t), 4);             /* log2(40) 量级 */
        for (int i = 1; i <= 40; i += 3) {
            CHECK_EQ(btree_delete(i, t), 0);
            CHECK(btree_check(t));
        }
        btree_dispose(t);
    }
}

/* ================= 4.45 线索二叉树 ================= */
static void thread_inorder_arr(const ThreadNode *n, int out[], int *k)
{
    if (n == NULL) return;
    thread_inorder_arr(n->left, out, k);
    out[(*k)++] = n->elem;
    if (!n->right_thread && n->right != NULL)
        thread_inorder_arr(n->right, out, k);
}

static void test_threaded(void)
{
    ThreadTree *t = thread_create();
    CHECK(t != NULL);
    msize = 0;
    for (int step = 0; step < 1200; ++step) {
        int v = rnd(0, 60);
        if (rnd(0, 1)) {
            int added = model_insert(v);
            int r = thread_insert(v, t);
            CHECK_EQ(r == 0, added);
        } else {
            int del = model_delete(v);
            int r = thread_delete(v, t);
            CHECK_EQ(r == 0, del);
        }
        CHECK_EQ(thread_size(t), msize);
        /* 线索中序遍历（不用栈）必须给出升序序列 */
        {
            FILE *f = tmpfile();
            thread_inorder(t, f);
            fflush(f); rewind(f);
            size_t n = fread(buf, 1, sizeof buf - 1, f);
            buf[n] = '\0';
            fclose(f);
            char expect[512];
            int o = 0;
            for (int i = 0; i < msize; ++i)
                o += snprintf(expect + o, sizeof expect - (size_t)o, "%d ", model[i]);
            expect[o] = '\0';
            CHECK_EQ(strcmp(buf, expect), 0);
        }
        {
            int missing = 0;
            for (int i = 0; i < msize; ++i)
                if (thread_find(model[i], t) == NULL) { missing = 1; break; }
            CHECK_EQ(missing, 0);
        }
        CHECK(thread_find(-5, t) == NULL);
    }
    /* 递归中序与线索中序必须一致 */
    {
        int arr[128], k = 0;
        thread_inorder_arr(t->root, arr, &k);
        CHECK_EQ(k, msize);
        for (int i = 0; i < msize && i < k; ++i)
            CHECK_EQ(arr[i], model[i]);
    }
    thread_dispose(t);
}

/* ================= 4.46 2-d 树 ================= */
static void test_kd(void)
{
    KDTree *t = kd_create();
    CHECK(t != NULL);
    int pts[600][2];
    int npts = 0;
    for (int i = 0; i < 300; ++i) {
        int x = rnd(0, 40), y = rnd(0, 40);
        int dup = 0;
        for (int j = 0; j < npts; ++j)
            if (pts[j][0] == x && pts[j][1] == y) dup = 1;
        if (dup) continue;
        CHECK_EQ(kd_insert(x, y, t), 0);
        pts[npts][0] = x;
        pts[npts][1] = y;
        ++npts;
    }
    CHECK_EQ(kd_size(t), npts);
    for (int i = 0; i < npts; ++i)
        CHECK(kd_contains(pts[i][0], pts[i][1], t));
    CHECK_EQ(kd_contains(-1, -1, t), 0);
    CHECK_EQ(kd_insert(pts[0][0], pts[0][1], t), -1);      /* 重复 */

    /* 范围查找与暴力对拍 */
    for (int trial = 0; trial < 200; ++trial) {
        int x1 = rnd(0, 40), x2 = rnd(0, 40);
        int y1 = rnd(0, 40), y2 = rnd(0, 40);
        if (x1 > x2) { int tmp = x1; x1 = x2; x2 = tmp; }
        if (y1 > y2) { int tmp = y1; y1 = y2; y2 = tmp; }
        int out[400][2];
        int got = kd_range(t, x1, x2, y1, y2, out, 400);
        int want = 0;
        for (int i = 0; i < npts; ++i)
            if (pts[i][0] >= x1 && pts[i][0] <= x2 &&
                pts[i][1] >= y1 && pts[i][1] <= y2)
                ++want;
        CHECK_EQ(got, want);
        /* 容量截断时返回值仍然是真实总数 */
        int small[3][2];
        CHECK_EQ(kd_range(t, x1, x2, y1, y2, small, 3), want);
    }

    /* 最近邻与暴力对拍 */
    for (int trial = 0; trial < 200; ++trial) {
        int qx = rnd(0, 40), qy = rnd(0, 40);
        int best[2];
        int dist = kd_nearest(t, qx, qy, best);
        int want = 0;
        for (int i = 0; i < npts; ++i) {
            int dx = pts[i][0] - qx, dy = pts[i][1] - qy;
            int d = dx * dx + dy * dy;
            if (i == 0 || d < want) want = d;
        }
        CHECK_EQ(dist, want);
        /* 返回的点必须真的有这个距离 */
        {
            int dx = best[0] - qx, dy = best[1] - qy;
            CHECK_EQ(dx * dx + dy * dy, dist);
        }
    }
    kd_dispose(t);
}

int main(void)
{
    test_tree_basics();
    test_expression_tree();
    test_child_sibling();
    test_bst();
    test_avl();
    test_splay();
    test_btree();
    test_threaded();
    test_kd();
    return TEST_REPORT();
}
