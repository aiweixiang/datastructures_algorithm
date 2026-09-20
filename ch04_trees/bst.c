/* 第 4 章：二叉查找树（4.10 ~ 4.14、4.28 ~ 4.35、4.44）。
 * 每个结点维护子树大小 sz，因此 FindKth 是 O(log N)（期望）。
 */
#include <stdlib.h>
#include "ch04.h"

BST *bst_create(void)
{
    BST *t = malloc(sizeof *t);
    if (t == NULL)
        return NULL;
    t->root = NULL;
    t->size = 0;
    return t;
}

void bst_make_empty(BST *t)
{
    if (t == NULL)
        return;
    tree_dispose(t->root);
    t->root = NULL;
    t->size = 0;
}

void bst_dispose(BST *t)
{
    if (t == NULL)
        return;
    bst_make_empty(t);
    free(t);
}

static int node_sz(const TreeNode *n)
{
    return n == NULL ? 0 : n->sz;
}

static void fix_sz(TreeNode *n)
{
    if (n != NULL)
        n->sz = 1 + node_sz(n->left) + node_sz(n->right);
}

/* 递归插入：在回溯（自底向上）时修 sz。
 * 注意：sz 必须自底向上更新——若从根往下 update，父结点会用子结点
 * 尚未更新的旧 sz 计算，路径上除最底层外全部错位，FindKth 随之错位。 */
static TreeNode *insert_rec(TreeNode *t, int x, int *inserted)
{
    if (t == NULL) {
        TreeNode *n = tree_node(x);
        if (n != NULL)
            *inserted = 1;
        return n;
    }
    if (x < t->elem)
        t->left = insert_rec(t->left, x, inserted);
    else if (x > t->elem)
        t->right = insert_rec(t->right, x, inserted);
    fix_sz(t);
    return t;
}

TreeNode *bst_insert(int x, BST *t)
{
    if (t == NULL)
        return NULL;
    int inserted = 0;
    t->root = insert_rec(t->root, x, &inserted);
    if (inserted)
        ++t->size;
    return bst_find(x, t);      /* 重复元素返回已有结点；分配失败返回 NULL */
}

TreeNode *bst_find(int x, const BST *t)
{
    if (t == NULL)
        return NULL;
    TreeNode *p = t->root;
    while (p != NULL) {
        if (x < p->elem) p = p->left;
        else if (x > p->elem) p = p->right;
        else return p;
    }
    return NULL;
}

TreeNode *bst_find_min(const BST *t)
{
    if (t == NULL || t->root == NULL)
        return NULL;
    TreeNode *p = t->root;
    while (p->left != NULL)
        p = p->left;
    return p;
}

TreeNode *bst_find_max(const BST *t)
{
    if (t == NULL || t->root == NULL)
        return NULL;
    TreeNode *p = t->root;
    while (p->right != NULL)
        p = p->right;
    return p;
}

/* 4.13 方法一：用右子树的最小元替换（教材采用的做法） */
static TreeNode *delete_right_min(TreeNode *t, int x, int *found)
{
    if (t == NULL)
        return NULL;
    if (x < t->elem) {
        t->left = delete_right_min(t->left, x, found);
    } else if (x > t->elem) {
        t->right = delete_right_min(t->right, x, found);
    } else {
        *found = 1;
        if (t->left != NULL && t->right != NULL) {
            TreeNode *m = t->right;
            while (m->left != NULL)
                m = m->left;
            t->elem = m->elem;
            int dummy = 0;
            t->right = delete_right_min(t->right, m->elem, &dummy);
        } else {
            TreeNode *child = (t->left != NULL) ? t->left : t->right;
            free(t);
            return child;
        }
    }
    fix_sz(t);
    return t;
}

/* 4.13 方法二：用左子树的最大元替换 */
static TreeNode *delete_left_max(TreeNode *t, int x, int *found)
{
    if (t == NULL)
        return NULL;
    if (x < t->elem) {
        t->left = delete_left_max(t->left, x, found);
    } else if (x > t->elem) {
        t->right = delete_left_max(t->right, x, found);
    } else {
        *found = 1;
        if (t->left != NULL && t->right != NULL) {
            TreeNode *m = t->left;
            while (m->right != NULL)
                m = m->right;
            t->elem = m->elem;
            int dummy = 0;
            t->left = delete_left_max(t->left, m->elem, &dummy);
        } else {
            TreeNode *child = (t->left != NULL) ? t->left : t->right;
            free(t);
            return child;
        }
    }
    fix_sz(t);
    return t;
}

/* 4.13 方法三：合并法——把左子树整体挂到右子树的最小元上（可能让树变高） */
static TreeNode *delete_merge(TreeNode *t, int x, int *found)
{
    if (t == NULL)
        return NULL;
    if (x < t->elem) {
        t->left = delete_merge(t->left, x, found);
    } else if (x > t->elem) {
        t->right = delete_merge(t->right, x, found);
    } else {
        *found = 1;
        if (t->left != NULL && t->right != NULL) {
            TreeNode *r = t->right;
            while (r->left != NULL)
                r = r->left;
            r->left = t->left;
            TreeNode *nr = t->right;
            free(t);
            t = nr;
        } else {
            TreeNode *child = (t->left != NULL) ? t->left : t->right;
            free(t);
            return child;
        }
    }
    fix_sz(t);
    return t;
}

static int delete_with(TreeNode **root, int x, int *size,
                       TreeNode *(*fn)(TreeNode *, int, int *))
{
    if (root == NULL || *root == NULL)
        return -1;
    int found = 0;
    *root = fn(*root, x, &found);
    if (found)
        --*size;
    return found ? 0 : -1;
}

int bst_delete_right_min(int x, BST *t)
{
    return t == NULL ? -1 : delete_with(&t->root, x, &t->size, delete_right_min);
}

int bst_delete_left_max(int x, BST *t)
{
    return t == NULL ? -1 : delete_with(&t->root, x, &t->size, delete_left_max);
}

int bst_delete_merge(int x, BST *t)
{
    return t == NULL ? -1 : delete_with(&t->root, x, &t->size, delete_merge);
}

int bst_size(const BST *t) { return t ? t->size : 0; }
int bst_height(const BST *t) { return t ? tree_depth(t->root) : -1; }
int bst_count_nodes(const BST *t) { return tree_count_nodes(t ? t->root : NULL); }
int bst_count_leaves(const BST *t) { return tree_count_leaves(t ? t->root : NULL); }
int bst_count_full(const BST *t) { return tree_count_full(t ? t->root : NULL); }

static int is_bst_rec(const TreeNode *t, const int *lo, const int *hi)
{
    if (t == NULL)
        return 1;
    if (lo != NULL && t->elem <= *lo) return 0;
    if (hi != NULL && t->elem >= *hi) return 0;
    return is_bst_rec(t->left, lo, &t->elem) && is_bst_rec(t->right, &t->elem, hi);
}

int bst_is_bst(const BST *t)
{
    return t == NULL ? 1 : is_bst_rec(t->root, NULL, NULL);
}

/* 4.14 内部路径长 = 所有结点深度之和（根深度为 0） */
static int ipl_rec(const TreeNode *t, int depth)
{
    if (t == NULL)
        return 0;
    return depth + ipl_rec(t->left, depth + 1) + ipl_rec(t->right, depth + 1);
}

int bst_internal_path_length(const BST *t)
{
    return t == NULL ? 0 : ipl_rec(t->root, 0);
}

/* 4.44 FindKth：用子树大小把查找降到 O(树高) */
int bst_find_kth(int k, const BST *t)
{
    if (t == NULL || k < 1 || k > t->size)
        return -1;
    TreeNode *p = t->root;
    while (p != NULL) {
        int ls = node_sz(p->left);
        if (k <= ls)
            p = p->left;
        else if (k == ls + 1)
            return p->elem;
        else {
            k -= ls + 1;
            p = p->right;
        }
    }
    return -1;
}

/* 4.32 打印 [k1, k2] 中的元素：小于 k1 的子树整棵剪掉 */
static void range_rec(const TreeNode *t, int k1, int k2, FILE *fp)
{
    if (t == NULL)
        return;
    if (t->elem > k1)
        range_rec(t->left, k1, k2, fp);
    if (t->elem >= k1 && t->elem <= k2)
        fprintf(fp, "%d ", t->elem);
    if (t->elem < k2)
        range_rec(t->right, k1, k2, fp);
}

void bst_print_range(int k1, int k2, const BST *t, FILE *fp)
{
    if (t == NULL || fp == NULL || k1 > k2)
        return;
    range_rec(t->root, k1, k2, fp);
}

/* 4.29 随机 BST：随机插入 n 个互不相同的元素 */
BST *bst_random(int n, unsigned long *state)
{
    BST *t = bst_create();
    if (t == NULL)
        return NULL;
    for (int i = 0; i < n; ++i) {
        int x;
        do {
            x = (int)(*state = (*state * 1103515245UL + 12345UL) & 0x7fffffffUL) % (n * 2 + 1);
        } while (bst_find(x, t) != NULL);
        bst_insert(x, t);
    }
    return t;
}

BST *bst_from_array(const int a[], int n)
{
    BST *t = bst_create();
    if (t == NULL)
        return NULL;
    for (int i = 0; i < n; ++i)
        bst_insert(a[i], t);
    return t;
}

/* 4.31 理想平衡 BST：取中位数当根，递归建树 —— 高度恰好 ⌈log2(n+1)⌉-1 */
static TreeNode *ideal_rec(const int a[], int lo, int hi)
{
    if (lo > hi)
        return NULL;
    int mid = lo + (hi - lo) / 2;
    TreeNode *n = tree_node(a[mid]);
    if (n == NULL)
        return NULL;
    n->left = ideal_rec(a, lo, mid - 1);
    n->right = ideal_rec(a, mid + 1, hi);
    n->sz = 1 + node_sz(n->left) + node_sz(n->right);
    return n;
}

BST *bst_ideal_from_sorted(const int sorted[], int n)
{
    BST *t = bst_create();
    if (t == NULL)
        return NULL;
    t->root = ideal_rec(sorted, 0, n - 1);
    t->size = n;
    return t;
}

void bst_draw(const BST *t, FILE *fp)
{
    if (t == NULL)
        return;
    tree_print_indented(t->root, fp);
}
