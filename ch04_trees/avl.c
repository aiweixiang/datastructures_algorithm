/* 第 4 章：AVL 树（4.15 ~ 4.22）。
 * 每个结点存高度域（4.21）。插入用单旋（LL/RR）与双旋（LR/RL），
 * 并额外提供非递归插入（4.19）与删除（4.20）。
 */
#include <stdlib.h>
#include "ch04.h"

static int h(const AvlNode *n) { return n == NULL ? -1 : n->height; }
static int imax(int a, int b) { return a > b ? a : b; }
static int iabs(int a) { return a < 0 ? -a : a; }

static void fix_h(AvlNode *n)
{
    if (n != NULL)
        n->height = imax(h(n->left), h(n->right)) + 1;
}

static AvlNode *new_node(int x)
{
    AvlNode *n = malloc(sizeof *n);
    if (n == NULL)
        return NULL;
    n->elem = x;
    n->height = 0;
    n->left = n->right = NULL;
    return n;
}

/* 单旋转：LL 情形 → 右旋 */
static AvlNode *rotate_right(AvlNode *k2)
{
    AvlNode *k1 = k2->left;
    k2->left = k1->right;
    k1->right = k2;
    fix_h(k2);
    fix_h(k1);
    return k1;
}

/* 单旋转：RR 情形 → 左旋 */
static AvlNode *rotate_left(AvlNode *k1)
{
    AvlNode *k2 = k1->right;
    k1->right = k2->left;
    k2->left = k1;
    fix_h(k1);
    fix_h(k2);
    return k2;
}

/* 双旋转：LR 情形（先对左孩子左旋，再对自己右旋） */
static AvlNode *double_lr(AvlNode *k3)
{
    k3->left = rotate_left(k3->left);
    return rotate_right(k3);
}

/* 双旋转：RL 情形 */
static AvlNode *double_rl(AvlNode *k1)
{
    k1->right = rotate_right(k1->right);
    return rotate_left(k1);
}

/* 4.22：为什么双旋转不写成"两次单旋转"？因为两次单旋转会多算高度、
 * 且中间的临时状态不满足 AVL 条件；代码上二者对 LR/RL 的结果相同
 * （上面的 double_lr 正是两次旋转），但"识别出 LR 并一次完成"更清晰。 */

AVLTree *avl_create(void)
{
    AVLTree *t = malloc(sizeof *t);
    if (t == NULL)
        return NULL;
    t->root = NULL;
    t->size = 0;
    return t;
}

void avl_dispose(AVLTree *t)
{
    if (t == NULL)
        return;
    /* 复用通用二叉树的释放逻辑：AvlNode 与 TreeNode 的头两个字段布局一致，
     * 但为了不依赖布局，这里单独写一个递归释放。 */
    struct stack_frame { AvlNode *n; };
    (void)sizeof(struct stack_frame);

    AvlNode **stack = malloc(sizeof *stack * (size_t)(t->size + 16));
    int sp = 0;
    if (t->root != NULL)
        stack[sp++] = t->root;
    while (sp > 0) {
        AvlNode *n = stack[--sp];
        if (n->left) stack[sp++] = n->left;
        if (n->right) stack[sp++] = n->right;
        free(n);
    }
    free(stack);
    free(t);
}

/* 4.18 递归插入：单旋 + 双旋，每个结点最多一次旋转 */
static AvlNode *insert_rec(AvlNode *t, int x, int *inserted)
{
    if (t == NULL) {
        *inserted = 1;
        return new_node(x);
    }
    if (x < t->elem) {
        t->left = insert_rec(t->left, x, inserted);
        if (h(t->left) - h(t->right) == 2) {
            if (x < t->left->elem)
                t = rotate_right(t);          /* 左-左：单旋 */
            else
                t = double_lr(t);             /* 左-右：双旋 */
        }
    } else if (x > t->elem) {
        t->right = insert_rec(t->right, x, inserted);
        if (h(t->right) - h(t->left) == 2) {
            if (x > t->right->elem)
                t = rotate_left(t);           /* 右-右：单旋 */
            else
                t = double_rl(t);             /* 右-左：双旋 */
        }
    }
    fix_h(t);
    return t;
}

AvlNode *avl_insert(int x, AVLTree *t)
{
    if (t == NULL)
        return NULL;
    int inserted = 0;
    t->root = insert_rec(t->root, x, &inserted);
    if (inserted)
        ++t->size;
    return t->root;
}

/* 4.19 非递归插入：沿路径记录"指向子结点的指针"，再自底向上修正高度并旋转。
 * 额外空间 O(log N)（路径栈），这也是递归版本来就需要的栈空间。 */
#define AVL_PATH_MAX 256

AvlNode *avl_insert_iter(int x, AVLTree *t)
{
    if (t == NULL)
        return NULL;

    AvlNode **path[AVL_PATH_MAX];
    int depth = 0;
    AvlNode **link = &t->root;
    AvlNode *cur = NULL;

    while (*link != NULL) {
        cur = *link;
        if (x == cur->elem)
            return cur;                        /* 已存在 */
        if (depth >= AVL_PATH_MAX)
            return NULL;
        path[depth++] = link;
        link = (x < cur->elem) ? &cur->left : &cur->right;
    }

    AvlNode *n = new_node(x);
    if (n == NULL)
        return NULL;
    *link = n;
    ++t->size;

    while (depth > 0) {
        AvlNode **plink = path[--depth];
        AvlNode *node = *plink;
        fix_h(node);
        int bal = h(node->left) - h(node->right);
        if (bal == 2) {
            if (h(node->left->left) >= h(node->left->right))
                *plink = rotate_right(node);
            else
                *plink = double_lr(node);
        } else if (bal == -2) {
            if (h(node->right->right) >= h(node->right->left))
                *plink = rotate_left(node);
            else
                *plink = double_rl(node);
        }
    }
    return n;
}

/* 4.20 删除：删除后沿路径重新平衡 */
static AvlNode *delete_rec(AvlNode *t, int x, int *deleted)
{
    if (t == NULL)
        return NULL;

    if (x < t->elem) {
        t->left = delete_rec(t->left, x, deleted);
    } else if (x > t->elem) {
        t->right = delete_rec(t->right, x, deleted);
    } else {
        *deleted = 1;
        if (t->left != NULL && t->right != NULL) {
            AvlNode *m = t->right;               /* 用右子树最小元替换 */
            while (m->left != NULL)
                m = m->left;
            t->elem = m->elem;
            int dummy = 0;
            t->right = delete_rec(t->right, m->elem, &dummy);
        } else {
            AvlNode *child = (t->left != NULL) ? t->left : t->right;
            free(t);
            return child;
        }
    }

    fix_h(t);
    int bal = h(t->left) - h(t->right);
    if (bal == 2) {
        if (h(t->left->left) >= h(t->left->right))
            t = rotate_right(t);
        else
            t = double_lr(t);
    } else if (bal == -2) {
        if (h(t->right->right) >= h(t->right->left))
            t = rotate_left(t);
        else
            t = double_rl(t);
    }
    fix_h(t);
    return t;
}

int avl_delete(int x, AVLTree *t)
{
    if (t == NULL)
        return -1;
    int deleted = 0;
    t->root = delete_rec(t->root, x, &deleted);
    if (deleted)
        --t->size;
    return deleted ? 0 : -1;
}

AvlNode *avl_find(int x, const AVLTree *t)
{
    if (t == NULL)
        return NULL;
    AvlNode *p = t->root;
    while (p != NULL) {
        if (x < p->elem) p = p->left;
        else if (x > p->elem) p = p->right;
        else return p;
    }
    return NULL;
}

int avl_size(const AVLTree *t) { return t ? t->size : 0; }
int avl_height(const AVLTree *t) { return t ? h(t->root) : -1; }

static int balanced_rec(const AvlNode *t)
{
    if (t == NULL)
        return 1;
    if (iabs(h(t->left) - h(t->right)) > 1)
        return 0;
    return balanced_rec(t->left) && balanced_rec(t->right);
}

int avl_is_balanced(const AVLTree *t)
{
    return t == NULL ? 1 : balanced_rec(t->root);
}

/* 4.17 用：完全平衡 = 所有叶子同深度且每个内部结点都有两个孩子 */
static int perfect_rec(const AvlNode *t, int depth, int *leaf_depth, int *seen)
{
    if (t == NULL)
        return 1;
    if (t->left == NULL && t->right == NULL) {
        if (!*seen) {
            *leaf_depth = depth;
            *seen = 1;
        }
        return depth == *leaf_depth;
    }
    if (t->left == NULL || t->right == NULL)
        return 0;
    return perfect_rec(t->left, depth + 1, leaf_depth, seen) &&
           perfect_rec(t->right, depth + 1, leaf_depth, seen);
}

int avl_is_perfect(const AVLTree *t)
{
    if (t == NULL)
        return 0;
    int leaf_depth = -1, seen = 0;
    return perfect_rec(t->root, 0, &leaf_depth, &seen);
}

static void preorder_rec(const AvlNode *t, FILE *fp)
{
    if (t == NULL)
        return;
    fprintf(fp, "%d ", t->elem);
    preorder_rec(t->left, fp);
    preorder_rec(t->right, fp);
}

static void inorder_rec(const AvlNode *t, FILE *fp)
{
    if (t == NULL)
        return;
    inorder_rec(t->left, fp);
    fprintf(fp, "%d ", t->elem);
    inorder_rec(t->right, fp);
}

void avl_preorder(const AVLTree *t, FILE *fp)
{
    if (t != NULL)
        preorder_rec(t->root, fp);
}

void avl_inorder(const AVLTree *t, FILE *fp)
{
    if (t != NULL)
        inorder_rec(t->root, fp);
}

/* 4.15 高度为 h 的 AVL 树最少结点数：N(0)=1, N(1)=2, N(h)=N(h-1)+N(h-2)+1 */
int avl_min_nodes(int h)
{
    if (h < 0)
        return 0;
    if (h == 0)
        return 1;
    long n0 = 1, n1 = 2;
    if (h == 1)
        return (int)n1;
    for (int i = 2; i <= h; ++i) {
        long n = n0 + n1 + 1;
        n0 = n1;
        n1 = n;
        if (n1 > 2147483647L)
            return 2147483647;            /* 饱和到 INT_MAX；必须严格大于调用方
                                           * 可能传入的 n，否则 avl_height_bits
                                           * 的 while 会永不退出（死循环）。 */
    }
    return (int)n1;
}

/* 4.30 生成最少结点的 AVL 树：左子树高度 h-1、右子树高度 h-2 */
static AvlNode *min_tree_rec(int h)
{
    if (h < 0)
        return NULL;
    AvlNode *n = new_node(0);
    if (n == NULL)
        return NULL;
    n->left = min_tree_rec(h - 1);
    n->right = min_tree_rec(h - 2);
    fix_h(n);
    return n;
}

AVLTree *avl_min_tree(int h)
{
    AVLTree *t = avl_create();
    if (t == NULL)
        return NULL;
    t->root = min_tree_rec(h);
    /* 填入 1..n 的中序序值，使其成为合法的 BST */
    int counter = 1;
    AvlNode **stack = malloc(sizeof *stack * (size_t)(avl_min_nodes(h) + 8));
    if (stack == NULL) {
        avl_dispose(t);
        return NULL;
    }
    int sp = 0;
    AvlNode *cur = t->root;
    int size = 0;
    while (cur != NULL || sp > 0) {
        while (cur != NULL) {
            stack[sp++] = cur;
            cur = cur->left;
        }
        cur = stack[--sp];
        cur->elem = counter++;
        ++size;
        cur = cur->right;
    }
    free(stack);
    t->size = size;
    return t;
}

/* 4.21 高度域需要几位：AVL 树高度 h 与最少结点数互为反函数，
 * 对 n 个结点求出可能的最大高度 h_max，再用 ceil(log2(h_max+1)) 位存它。 */
int avl_height_bits(int n)
{
    if (n <= 0)
        return 0;
    int h = 0;
    while (avl_min_nodes(h + 1) <= n)
        ++h;
    int bits = 0;
    unsigned v = (unsigned)h;
    while (v > 0) {
        ++bits;
        v >>= 1;
    }
    return bits == 0 ? 1 : bits;
}
