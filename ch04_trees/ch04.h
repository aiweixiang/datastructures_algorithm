/* 第 4 章 树：本章全部习题的公共接口。
 * 编号按你的版本（4.1 ~ 4.46），编号映射见 ../NUMBERING.md。
 */
#ifndef CH04_H
#define CH04_H

#include <stdio.h>

/* ============================ 通用二叉树 ============================ */
/* sz 只在"由 BST 操作构造出来的树"里被维护（4.44 FindKth 需要它）。 */
typedef struct TreeNode {
    int              elem;
    int              sz;            /* 子树结点数 */
    struct TreeNode *left, *right;
} TreeNode;

TreeNode *tree_node(int elem);
void      tree_dispose(TreeNode *t);

/* 4.3 深度 */
int tree_depth(const TreeNode *t);
/* 4.28 一次遍历同时得出三个统计量；下面三个是单独版本 */
void tree_stats(const TreeNode *t, int *nodes, int *leaves, int *full);
int  tree_count_nodes(const TreeNode *t);
int  tree_count_leaves(const TreeNode *t);
int  tree_count_full(const TreeNode *t);

/* 4.41 相似（形状完全相同）/ 4.42 同构（允许交换左右子树） */
int tree_similar(const TreeNode *a, const TreeNode *b);
int tree_isomorphic(const TreeNode *a, const TreeNode *b);

/* 4.8 遍历 */
void tree_preorder(const TreeNode *t, FILE *fp);
void tree_inorder(const TreeNode *t, FILE *fp);
void tree_postorder(const TreeNode *t, FILE *fp);
void tree_level_order(const TreeNode *t, FILE *fp);      /* 4.35，用队列 */

/* 4.8 表达式树 */
TreeNode *expr_tree_from_postfix(const char *expr);
int       expr_tree_eval(const TreeNode *t, int *ok);
/* order: 0=前序 1=中序 2=后序；运算符按字符打印 */
void      expr_tree_print(const TreeNode *t, FILE *fp, int order);

/* 4.33 / 4.34 给每个结点分配坐标并画树。
 * 按前序把 (x, y) 写入数组；x 是中序序号，y 是深度。返回结点数（或 -1 容量不够）。 */
int  tree_assign_xy(const TreeNode *t, int x[], int y[], int max);
void tree_print_grid(const TreeNode *t, FILE *fp);
void tree_print_indented(const TreeNode *t, FILE *fp);

/* ============================ 4.39 / 4.40 儿子-兄弟表示 ============================ */
typedef struct CSNode {
    int             elem;
    struct CSNode  *first_child;
    struct CSNode  *next_sibling;
} CSNode;

/* parent[i] 是结点 i（0..n-1）的父亲，-1 表示根 */
CSNode *cs_from_parents(int n, const int parent[]);
void    cs_preorder(CSNode *t, FILE *fp);          /* 无论用哪种表示，前序序列相同 */
void    cs_postorder(CSNode *t, FILE *fp);
void    cs_dispose(CSNode *t);
int     cs_depth(CSNode *t);

/* ============================ 4.10 / 4.11 二叉查找树 ============================ */
typedef struct {
    TreeNode *root;
    int       size;
} BST;

BST      *bst_create(void);
void      bst_dispose(BST *t);
void      bst_make_empty(BST *t);
TreeNode *bst_insert(int x, BST *t);
TreeNode *bst_find(int x, const BST *t);
TreeNode *bst_find_min(const BST *t);
TreeNode *bst_find_max(const BST *t);

/* 4.13 删除两子结点时的三种做法 */
int bst_delete_right_min(int x, BST *t);    /* 用右子树最小元替换 */
int bst_delete_left_max(int x, BST *t);     /* 用左子树最大元替换 */
int bst_delete_merge(int x, BST *t);        /* 把左子树挂到右子树最左端（合并法） */

int bst_size(const BST *t);
int bst_height(const BST *t);
int bst_is_bst(const BST *t);
int bst_count_nodes(const BST *t);
int bst_count_leaves(const BST *t);
int bst_count_full(const BST *t);
int bst_internal_path_length(const BST *t);      /* 4.14 内部路径长 */
int bst_find_kth(int k, const BST *t);           /* 4.44，1 起始；-1 表示越界 */
void bst_print_range(int k1, int k2, const BST *t, FILE *fp);   /* 4.32 */

BST *bst_random(int n, unsigned long *state);            /* 4.29 随机插入 */
BST *bst_from_array(const int a[], int n);              /* 逐个插入 */
BST *bst_ideal_from_sorted(const int sorted[], int n);  /* 4.31 理想平衡 */
void bst_draw(const BST *t, FILE *fp);                   /* 4.33/4.34 */

/* ============================ 4.15 ~ 4.22 AVL 树 ============================ */
typedef struct AvlNode {
    int              elem;
    int              height;        /* 4.21：只需要 1 个字节甚至更少 */
    struct AvlNode  *left, *right;
} AvlNode;

typedef struct {
    AvlNode *root;
    int      size;
} AVLTree;

AVLTree  *avl_create(void);
void      avl_dispose(AVLTree *t);
AvlNode  *avl_insert(int x, AVLTree *t);        /* 4.18 单旋 + 双旋 */
AvlNode  *avl_insert_iter(int x, AVLTree *t);   /* 4.19 非递归 */
int       avl_delete(int x, AVLTree *t);        /* 4.20 */
AvlNode  *avl_find(int x, const AVLTree *t);
int       avl_size(const AVLTree *t);
int       avl_height(const AVLTree *t);
int       avl_is_balanced(const AVLTree *t);    /* 每个结点平衡因子 ≤ 1 */
int       avl_is_perfect(const AVLTree *t);     /* 完全平衡（4.17） */
void      avl_preorder(const AVLTree *t, FILE *fp);
void      avl_inorder(const AVLTree *t, FILE *fp);
int       avl_min_nodes(int h);                 /* 4.15：N(h) = N(h-1) + N(h-2) + 1 */
AVLTree  *avl_min_tree(int h);                  /* 4.30 生成最少结点的 AVL 树 */
int       avl_height_bits(int n);               /* 4.21：n 个结点时高度域需要几位 */

/* ============================ 4.23 ~ 4.27 伸展树 ============================ */
typedef struct SplayNode {
    int               elem;
    struct SplayNode *left, *right, *parent;
} SplayNode;

typedef struct {
    SplayNode *root;
    int        size;
    long long  rotations;      /* 4.27 统计旋转次数 */
} SplayTree;

SplayTree *splay_create(void);
void       splay_dispose(SplayTree *t);
int        splay_insert(int x, SplayTree *t);
SplayNode *splay_access(int x, SplayTree *t);   /* 4.23 访问并伸展到根 */
int        splay_delete(int x, SplayTree *t);   /* 4.24 */
int        splay_contains(int x, SplayTree *t);
int        splay_size(const SplayTree *t);
int        splay_height(const SplayTree *t);
int        splay_is_bst(const SplayTree *t);
long long  splay_rotations(const SplayTree *t);
void       splay_preorder(const SplayTree *t, FILE *fp);
void       splay_inorder(const SplayTree *t, FILE *fp);
/* 4.25 / 4.26 实验：只有左儿子的链式结构、顺序访问 */
int        splay_chain_path_length(int n);          /* 插入 1..n 后访问最小值 */
long long  splay_sequential_access_rotations(int n);/* 顺序访问 1..n */

/* ============================ 4.36 / 4.37 / 4.38 B 树 ============================ */
#define BTREE_MAX_M 7
typedef struct BTreeNode {
    int  n;                       /* 当前关键字个数 */
    int  leaf;
    int  key[BTREE_MAX_M];        /* 关键字，最多 M-1 个 */
    struct BTreeNode *child[BTREE_MAX_M + 1];
} BTreeNode;

typedef struct {
    int        M;                 /* 阶：每个结点最多 M-1 个关键字 */
    BTreeNode *root;
    int        size;
} BTree;

BTree *btree_create(int M);
void   btree_dispose(BTree *t);
int    btree_insert(int key, BTree *t);      /* 4.37 插入（含分裂） */
int    btree_delete(int key, BTree *t);      /* 4.37 删除（借位/合并） */
int    btree_contains(int key, const BTree *t);
int    btree_size(const BTree *t);
int    btree_height(const BTree *t);
int    btree_check(const BTree *t);          /* 校验 B 树全部不变量 */
void   btree_preorder(const BTree *t, FILE *fp);

/* 4.36 2-3 树 = M = 3 的 B 树 */
BTree *tree23_create(void);

/* ============================ 4.45 线索二叉树 ============================ */
typedef struct ThreadNode {
    int                elem;
    int                right_thread;    /* 1 表示 right 是线索（指向中序后继） */
    struct ThreadNode *left, *right;
} ThreadNode;

typedef struct {
    ThreadNode *root;
    int         size;
} ThreadTree;

ThreadTree *thread_create(void);
void        thread_dispose(ThreadTree *t);
int         thread_insert(int x, ThreadTree *t);
int         thread_delete(int x, ThreadTree *t);
ThreadNode *thread_find(int x, const ThreadTree *t);
int         thread_size(const ThreadTree *t);
void        thread_inorder(const ThreadTree *t, FILE *fp);  /* 不用栈、不用递归 */
int         thread_height(const ThreadTree *t);

/* ============================ 4.46 k-d 树（这里实现 2-d 树） ============================ */
typedef struct KDNode {
    int             pt[2];
    struct KDNode  *left, *right;
} KDNode;

typedef struct {
    KDNode *root;
    int     size;
} KDTree;

KDTree *kd_create(void);
void    kd_dispose(KDTree *t);
int     kd_insert(int x, int y, KDTree *t);
int     kd_contains(int x, int y, const KDTree *t);
/* 矩形范围查找：返回 [x1,x2] × [y1,y2] 内的点数，并把点写入 out（最多 max 个） */
int     kd_range(const KDTree *t, int x1, int x2, int y1, int y2,
                 int out[][2], int max);
int     kd_size(const KDTree *t);
int     kd_nearest(const KDTree *t, int x, int y, int *best);   /* 最近邻，返回距离平方 */

#endif /* CH04_H */
