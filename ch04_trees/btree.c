/* 第 4 章：B 树（4.36 2-3 树、4.37 插入与删除、4.38 B* 树见 ANSWERS.md）。
 *
 * 约定：阶为 M（每个结点最多 M 个孩子、最多 M-1 个关键字），
 *   最少孩子数 t = ⌈M/2⌉，最少关键字数 t-1。
 *   M = 3 就是 2-3 树（结点可以有 1 或 2 个关键字）。
 *
 * 插入：递归下降，写完之后若本结点关键字数达到 M（溢出）就分裂：
 *   左半保留 t-1 个，中位键上升到父结点，右半得到 M-t 个。
 *   父结点可能因此继续溢出，于是自底向上传播；根溢出则树长高一层。
 * 删除：自顶向下。当要下降到的孩子只有 t-1 个关键字时，先从兄弟借，
 *   借不到就把孩子、父结点中的分隔键、右兄弟合并成一个结点；
 *   若目标键在内部结点，则用前驱/后继替换或在合并后递归删除。
 *
 * 测试策略：与"有序动态集合"模型做随机插入/删除对拍，并用 btree_check
 * 校验 B 树的全部不变量（叶子同深度、关键字上下界、孩子数 = 关键字数+1、
 * 区间有序），这样任何一处算法错误都会被抓住。
 */
#include <stdlib.h>
#include "ch04.h"

static int min_children(const BTree *T) { return (T->M + 1) / 2; }
static int min_keys(const BTree *T) { return min_children(T) - 1; }

static BTreeNode *new_node(int M, int leaf)
{
    BTreeNode *n = malloc(sizeof *n);
    if (n == NULL)
        return NULL;
    n->n = 0;
    n->leaf = leaf;
    for (int i = 0; i <= M; ++i)
        n->child[i] = NULL;
    for (int i = 0; i < M; ++i)
        n->key[i] = 0;
    return n;
}

BTree *btree_create(int M)
{
    if (M < 3 || M > BTREE_MAX_M)
        return NULL;
    BTree *t = malloc(sizeof *t);
    if (t == NULL)
        return NULL;
    t->M = M;
    t->size = 0;
    t->root = new_node(M, 1);
    if (t->root == NULL) {
        free(t);
        return NULL;
    }
    return t;
}

BTree *tree23_create(void)
{
    return btree_create(3);
}

static void dispose_rec(BTreeNode *n, int M)
{
    if (n == NULL)
        return;
    if (!n->leaf)
        for (int i = 0; i <= n->n; ++i)
            dispose_rec(n->child[i], M);
    free(n);
}

void btree_dispose(BTree *t)
{
    if (t == NULL)
        return;
    dispose_rec(t->root, t->M);
    free(t);
}

static int contains_rec(const BTreeNode *n, int key)
{
    int i = 0;
    while (i < n->n && key > n->key[i])
        ++i;
    if (i < n->n && n->key[i] == key)
        return 1;
    if (n->leaf)
        return 0;
    return contains_rec(n->child[i], key);
}

int btree_contains(int key, const BTree *t)
{
    return (t != NULL && t->root != NULL) ? contains_rec(t->root, key) : 0;
}

int btree_size(const BTree *t) { return t ? t->size : 0; }

/* 插入：返回 1 表示本结点分裂，并把中位键与右兄弟通过出参返回 */
static int insert_rec(const BTree *T, BTreeNode *n, int key,
                      int *median, BTreeNode **right_out)
{
    if (n->leaf) {
        int i = n->n - 1;
        while (i >= 0 && key < n->key[i]) {
            n->key[i + 1] = n->key[i];
            --i;
        }
        n->key[i + 1] = key;
        ++n->n;
    } else {
        int i = 0;
        while (i < n->n && key > n->key[i])
            ++i;
        int med = 0;
        BTreeNode *right = NULL;
        if (insert_rec(T, n->child[i], key, &med, &right)) {
            /* 把 (med, right) 插到关键字下标 i 处 */
            for (int j = n->n; j > i; --j) {
                n->key[j] = n->key[j - 1];
                n->child[j + 1] = n->child[j];
            }
            n->key[i] = med;
            n->child[i + 1] = right;
            ++n->n;
        }
    }

    if (n->n == T->M) {                        /* 溢出：分裂 */
        int t = min_children(T);
        int m = t - 1;                         /* 中位键下标 */
        BTreeNode *right = new_node(T->M, n->leaf);
        if (right == NULL)
            return 0;
        right->n = T->M - 1 - m;
        for (int j = 0; j < right->n; ++j)
            right->key[j] = n->key[m + 1 + j];
        if (!n->leaf)
            for (int j = 0; j <= right->n; ++j)
                right->child[j] = n->child[m + 1 + j];
        *median = n->key[m];
        n->n = m;
        *right_out = right;
        return 1;
    }
    return 0;
}

int btree_insert(int key, BTree *t)
{
    if (t == NULL)
        return -1;
    if (btree_contains(key, t))
        return -1;                             /* 不插重复键 */
    int median = 0;
    BTreeNode *right = NULL;
    if (insert_rec(t, t->root, key, &median, &right)) {
        BTreeNode *root = new_node(t->M, 0);
        if (root == NULL)
            return -1;
        root->key[0] = median;
        root->child[0] = t->root;
        root->child[1] = right;
        root->n = 1;
        t->root = root;
    }
    ++t->size;
    return 0;
}

/* ---------- 删除 ---------- */

/* 从右兄弟借一个关键字（child i 与 i+1 都是 n 的孩子） */
static void borrow_from_right(const BTree *T, BTreeNode *n, int i)
{
    (void)T;
    BTreeNode *child = n->child[i];
    BTreeNode *sib = n->child[i + 1];
    child->key[child->n] = n->key[i];
    if (!sib->leaf)
        child->child[child->n + 1] = sib->child[0];
    ++child->n;
    n->key[i] = sib->key[0];
    for (int j = 0; j < sib->n - 1; ++j)
        sib->key[j] = sib->key[j + 1];
    if (!sib->leaf)
        for (int j = 0; j < sib->n; ++j)
            sib->child[j] = sib->child[j + 1];
    --sib->n;
}

/* 从左兄弟借一个关键字 */
static void borrow_from_left(const BTree *T, BTreeNode *n, int i)
{
    (void)T;
    BTreeNode *child = n->child[i];
    BTreeNode *sib = n->child[i - 1];
    for (int j = child->n; j > 0; --j)
        child->key[j] = child->key[j - 1];
    if (!child->leaf)
        for (int j = child->n + 1; j > 0; --j)
            child->child[j] = child->child[j - 1];
    child->key[0] = n->key[i - 1];
    n->key[i - 1] = sib->key[sib->n - 1];
    if (!sib->leaf)
        child->child[0] = sib->child[sib->n];
    --sib->n;
    ++child->n;
}

/* 合并 child[i]、key[i]、child[i+1] 到 child[i]，并从 n 中删掉 key[i] */
static void merge_children(BTreeNode *n, int i)
{
    BTreeNode *left = n->child[i];
    BTreeNode *right = n->child[i + 1];
    left->key[left->n] = n->key[i];
    for (int j = 0; j < right->n; ++j)
        left->key[left->n + 1 + j] = right->key[j];
    if (!left->leaf)
        for (int j = 0; j <= right->n; ++j)
            left->child[left->n + 1 + j] = right->child[j];
    left->n += right->n + 1;
    free(right);
    for (int j = i; j < n->n - 1; ++j) {
        n->key[j] = n->key[j + 1];
        n->child[j + 1] = n->child[j + 2];
    }
    --n->n;
}

/* 下降递归返回后，若 child i 真的下溢（关键字数 < min_keys），
 * 先尝试向兄弟借，借不到再与兄弟合并。
 *
 * 关键：合并必须发生在“已经少了一个关键字”之后。此时合并结果为
 *   (min_keys-1) + 1(分隔键) + min_keys = 2*min_keys
 * 而 2*min_keys = 2(ceil(M/2)-1) <= M-1，永远不超上限。
 * 若像以前那样在下降“前”预先合并（child 还有 min_keys 个关键字），
 * 结果就是 2*min_keys+1；对奇数 M（M=3,5,7）会超出 M-1，
 * 写坏 key/child 数组，这就是之前 btree_check 失败 + 段错误的根源。 */
static void fix_child(const BTree *T, BTreeNode *n, int i)
{
    if (n->child[i]->n >= min_keys(T))
        return;
    if (i > 0 && n->child[i - 1]->n > min_keys(T)) {
        borrow_from_left(T, n, i);
    } else if (i < n->n && n->child[i + 1]->n > min_keys(T)) {
        borrow_from_right(T, n, i);
    } else if (i > 0) {
        merge_children(n, i - 1);
    } else {
        merge_children(n, i);
    }
}

/* 子树中的最小/最大关键字：必须一路走到最左/最右叶子。
 * 不能用 child->key[0] / child->key[n-1]——当孩子是内部结点时，
 * 那只是该孩子自己的头/尾关键字，并不是整棵子树的最小/最大值。 */
static int subtree_max_key(const BTreeNode *n)
{
    while (!n->leaf)
        n = n->child[n->n];
    return n->key[n->n - 1];
}

static int delete_rec(const BTree *T, BTreeNode *n, int key)
{
    int i = 0;
    while (i < n->n && key > n->key[i])
        ++i;

    if (i < n->n && n->key[i] == key) {
        if (n->leaf) {
            for (int j = i; j < n->n - 1; ++j)
                n->key[j] = n->key[j + 1];
            --n->n;
            return 1;
        }
        /* 内部结点：统一用左子树的最大值（前驱）替换，再从左边删掉它；
         * 若删前驱后左孩子下溢，用 fix_child 事后修复（借或合并）。
         *
         * 不要在孩子"都只剩最小值"时对它们做合并：合并结果为
         *   2*min_keys + 1
         * 个关键字，而递归只是在合并结点里"替换"那个 key（关键字数不变），
         * 于是结点会停在 2*min_keys+1 > M-1，对奇数 M（M=3,5,7）直接超容。 */
        int pred = subtree_max_key(n->child[i]);
        n->key[i] = pred;
        int ok = delete_rec(T, n->child[i], pred);
        if (ok)
            fix_child(T, n, i);
        return ok;
    }

    if (n->leaf)
        return 0;                                /* 不存在 */

    int found = delete_rec(T, n->child[i], key);
    if (found)
        fix_child(T, n, i);                      /* 事后修复下溢，而不是下降前预防 */
    return found;
}

int btree_delete(int key, BTree *t)
{
    if (t == NULL || t->root == NULL)
        return -1;
    if (!delete_rec(t, t->root, key))
        return -1;
    --t->size;
    if (t->root->n == 0 && !t->root->leaf) {     /* 根空了：树降低一层 */
        BTreeNode *old = t->root;
        t->root = old->child[0];
        free(old);
    }
    return 0;
}

int btree_height(const BTree *t)
{
    if (t == NULL || t->root == NULL)
        return -1;
    int h = 0;
    BTreeNode *n = t->root;
    while (!n->leaf) {
        n = n->child[0];
        ++h;
    }
    return h;
}

/* 校验全部不变量：叶子同深度、关键字上下界、关键字数范围、区间有序。
 * lo/hi 是开区间端点（NULL 表示无穷）。 */
static int check_rec(const BTree *T, const BTreeNode *n, int is_root,
                     int *leaf_depth, int depth, const int *lo, const int *hi)
{
    if (n == NULL)
        return 0;
    if (n->n > T->M - 1 || n->n < 0)
        return 0;
    if (!is_root && n->n < min_keys(T))
        return 0;
    if (is_root && !n->leaf && n->n < 1)
        return 0;
    for (int i = 1; i < n->n; ++i)
        if (n->key[i - 1] >= n->key[i])
            return 0;
    if (lo != NULL && n->n > 0 && n->key[0] <= *lo)
        return 0;
    if (hi != NULL && n->n > 0 && n->key[n->n - 1] >= *hi)
        return 0;

    if (n->leaf) {
        if (*leaf_depth < 0)
            *leaf_depth = depth;
        return *leaf_depth == depth;
    }
    for (int i = 0; i <= n->n; ++i) {
        const int *clo = (i == 0) ? lo : &n->key[i - 1];
        const int *chi = (i == n->n) ? hi : &n->key[i];
        if (!check_rec(T, n->child[i], 0, leaf_depth, depth + 1, clo, chi))
            return 0;
    }
    return 1;
}

static int count_keys(const BTreeNode *n)
{
    if (n == NULL)
        return 0;
    int total = n->n;
    if (!n->leaf)
        for (int i = 0; i <= n->n; ++i)
            total += count_keys(n->child[i]);
    return total;
}

int btree_check(const BTree *t)
{
    if (t == NULL || t->root == NULL)
        return 0;
    int leaf_depth = -1;
    if (!check_rec(t, t->root, 1, &leaf_depth, 0, NULL, NULL))
        return 0;
    return count_keys(t->root) == t->size;
}

static void pre_rec(const BTreeNode *n, FILE *fp)
{
    fprintf(fp, "[");
    for (int i = 0; i < n->n; ++i)
        fprintf(fp, "%d%s", n->key[i], i + 1 < n->n ? "," : "");
    fprintf(fp, "]");
    if (!n->leaf)
        for (int i = 0; i <= n->n; ++i)
            pre_rec(n->child[i], fp);
}

void btree_preorder(const BTree *t, FILE *fp)
{
    if (t != NULL && t->root != NULL)
        pre_rec(t->root, fp);
}
