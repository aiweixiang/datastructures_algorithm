/* 第 4 章：通用二叉树工具（4.3、4.8、4.28、4.33~4.35、4.39~4.42）。 */
#include <stdlib.h>
#include <string.h>
#include "ch04.h"

TreeNode *tree_node(int elem)
{
    TreeNode *n = malloc(sizeof *n);
    if (n == NULL)
        return NULL;
    n->elem = elem;
    n->sz = 1;
    n->left = n->right = NULL;
    return n;
}

void tree_dispose(TreeNode *t)
{
    if (t == NULL)
        return;
    tree_dispose(t->left);
    tree_dispose(t->right);
    free(t);
}

/* 4.3 深度：max(左,右) + 1（空树深度定义为 -1） */
int tree_depth(const TreeNode *t)
{
    if (t == NULL)
        return -1;
    int l = tree_depth(t->left);
    int r = tree_depth(t->right);
    return (l > r ? l : r) + 1;
}

/* 4.28 一次遍历同时得到结点数、叶结点数、满结点数（两个孩子都有） */
void tree_stats(const TreeNode *t, int *nodes, int *leaves, int *full)
{
    if (t == NULL)
        return;
    ++*nodes;
    if (t->left == NULL && t->right == NULL)
        ++*leaves;
    if (t->left != NULL && t->right != NULL)
        ++*full;
    tree_stats(t->left, nodes, leaves, full);
    tree_stats(t->right, nodes, leaves, full);
}

int tree_count_nodes(const TreeNode *t)
{
    int n = 0, l = 0, f = 0;
    tree_stats(t, &n, &l, &f);
    return n;
}

int tree_count_leaves(const TreeNode *t)
{
    int n = 0, l = 0, f = 0;
    tree_stats(t, &n, &l, &f);
    return l;
}

int tree_count_full(const TreeNode *t)
{
    int n = 0, l = 0, f = 0;
    tree_stats(t, &n, &l, &f);
    return f;
}

/* 4.41 相似：形状完全一致 */
int tree_similar(const TreeNode *a, const TreeNode *b)
{
    if (a == NULL && b == NULL)
        return 1;
    if (a == NULL || b == NULL)
        return 0;
    return tree_similar(a->left, b->left) && tree_similar(a->right, b->right);
}

/* 4.42 同构：允许在任意结点交换左右子树 */
int tree_isomorphic(const TreeNode *a, const TreeNode *b)
{
    if (a == NULL && b == NULL)
        return 1;
    if (a == NULL || b == NULL)
        return 0;
    return (tree_isomorphic(a->left, b->left) && tree_isomorphic(a->right, b->right)) ||
           (tree_isomorphic(a->left, b->right) && tree_isomorphic(a->right, b->left));
}

void tree_preorder(const TreeNode *t, FILE *fp)
{
    if (t == NULL)
        return;
    fprintf(fp, "%d ", t->elem);
    tree_preorder(t->left, fp);
    tree_preorder(t->right, fp);
}

void tree_inorder(const TreeNode *t, FILE *fp)
{
    if (t == NULL)
        return;
    tree_inorder(t->left, fp);
    fprintf(fp, "%d ", t->elem);
    tree_inorder(t->right, fp);
}

void tree_postorder(const TreeNode *t, FILE *fp)
{
    if (t == NULL)
        return;
    tree_postorder(t->left, fp);
    tree_postorder(t->right, fp);
    fprintf(fp, "%d ", t->elem);
}

/* 4.35 层序遍历：用队列（这里用可增长数组模拟队尾入、队头出） */
void tree_level_order(const TreeNode *t, FILE *fp)
{
    if (t == NULL)
        return;
    int cap = 16, head = 0, tail = 0;
    const TreeNode **q = malloc((size_t)cap * sizeof *q);
    if (q == NULL)
        return;
    q[tail++] = t;

    while (head < tail) {
        const TreeNode *cur = q[head++];
        fprintf(fp, "%d ", cur->elem);
        if (cur->left != NULL || cur->right != NULL) {
            if (tail + 2 > cap) {
                cap *= 2;
                const TreeNode **nq = realloc(q, (size_t)cap * sizeof *nq);
                if (nq == NULL) {
                    free(q);
                    return;
                }
                q = nq;
            }
            if (cur->left != NULL)
                q[tail++] = cur->left;
            if (cur->right != NULL)
                q[tail++] = cur->right;
        }
    }
    free(q);
}

/* ---------------- 4.8 表达式树 ---------------- */
static int is_operator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/';
}

TreeNode *expr_tree_from_postfix(const char *expr)
{
    if (expr == NULL)
        return NULL;
    size_t len = strlen(expr);
    TreeNode **stack = malloc((len + 1) * sizeof *stack);
    if (stack == NULL)
        return NULL;
    int sp = 0;

    for (size_t i = 0; i < len; ) {
        if (expr[i] == ' ' || expr[i] == '\t') {
            ++i;
            continue;
        }
        if (is_operator(expr[i])) {
            if (sp < 2) {                    /* 非法后缀式 */
                for (int k = 0; k < sp; ++k) tree_dispose(stack[k]);
                free(stack);
                return NULL;
            }
            TreeNode *r = stack[--sp];
            TreeNode *l = stack[--sp];
            TreeNode *op = tree_node(-(unsigned char)expr[i]);  /* 用负的 ASCII 存运算符 */
            if (op == NULL) {
                tree_dispose(l); tree_dispose(r); free(stack);
                return NULL;
            }
            op->left = l;
            op->right = r;
            op->sz = l->sz + r->sz + 1;
            stack[sp++] = op;
            ++i;
        } else if ((expr[i] >= '0' && expr[i] <= '9') || expr[i] == '-') {
            /* 这里 '-' 已被当作运算符处理，故数字只可能是非负整数 */
            int v = 0;
            while (i < len && expr[i] >= '0' && expr[i] <= '9') {
                v = v * 10 + (expr[i] - '0');
                ++i;
            }
            TreeNode *n = tree_node(v);
            if (n == NULL) {
                for (int k = 0; k < sp; ++k) tree_dispose(stack[k]);
                free(stack);
                return NULL;
            }
            stack[sp++] = n;
        } else {
            for (int k = 0; k < sp; ++k) tree_dispose(stack[k]);
            free(stack);
            return NULL;
        }
    }
    if (sp != 1) {
        for (int k = 0; k < sp; ++k) tree_dispose(stack[k]);
        free(stack);
        return NULL;
    }
    TreeNode *root = stack[0];
    free(stack);
    return root;
}

int expr_tree_eval(const TreeNode *t, int *ok)
{
    if (ok != NULL)
        *ok = 1;
    if (t == NULL) {
        if (ok) *ok = 0;
        return 0;
    }
    if (t->elem >= 0)
        return t->elem;                       /* 叶子：操作数 */
    int a = expr_tree_eval(t->left, ok);
    int b = expr_tree_eval(t->right, ok);
    char op = (char)(-(t->elem));
    switch (op) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/':
        if (b == 0) { if (ok) *ok = 0; return 0; }
        return a / b;
    default:
        if (ok) *ok = 0;
        return 0;
    }
}

/* ---------------- 4.33 / 4.34 坐标与画树 ---------------- */
/* 中序遍历给 x（第几个访问到），深度给 y */
static int assign_rec(const TreeNode *t, int elem[], int x[], int y[],
                      int depth, int *counter, int max)
{
    if (t == NULL)
        return 0;
    int count = 0;
    count += assign_rec(t->left, elem, x, y, depth + 1, counter, max);
    if (*counter >= max)
        return count;
    int i = *counter;
    if (elem != NULL) elem[i] = t->elem;
    x[i] = i;                     /* x = 中序序号 */
    y[i] = depth;                 /* y = 深度 */
    ++*counter;
    ++count;
    count += assign_rec(t->right, elem, x, y, depth + 1, counter, max);
    return count;
}

int tree_assign_xy(const TreeNode *t, int x[], int y[], int max)
{
    if (t == NULL || x == NULL || y == NULL || max <= 0)
        return -1;
    int counter = 0;
    return assign_rec(t, NULL, x, y, 0, &counter, max);
}

/* 网格画法：第 y 行按 x 坐标（乘 3 列）打印元素值 */
void tree_print_grid(const TreeNode *t, FILE *fp)
{
    int cap = tree_count_nodes(t);
    if (cap <= 0) {
        fprintf(fp, "(空树)\n");
        return;
    }
    int *elem = malloc((size_t)cap * sizeof *elem);
    int *x = malloc((size_t)cap * sizeof *x);
    int *y = malloc((size_t)cap * sizeof *y);
    if (elem == NULL || x == NULL || y == NULL) {
        free(elem); free(x); free(y);
        return;
    }
    int counter = 0;
    (void)assign_rec(t, elem, x, y, 0, &counter, cap);

    int maxd = tree_depth(t);
    for (int d = 0; d <= maxd; ++d) {
        int printed = 0;
        for (int i = 0; i < cap; ++i) {
            if (y[i] != d)
                continue;
            int col = x[i] * 3;
            if (col > printed)
                fprintf(fp, "%*s", col - printed, "");
            int len = fprintf(fp, "%d", elem[i]);
            printed = col + len;
        }
        fprintf(fp, "\n");
    }
    free(elem);
    free(x);
    free(y);
}

/* 更实用的"旋转 90°"打印：右子树在上、左子树在下，用缩进表示层次 */
static void print_indented_rec(const TreeNode *t, FILE *fp, int depth)
{
    if (t == NULL)
        return;
    print_indented_rec(t->right, fp, depth + 1);
    for (int i = 0; i < depth; ++i)
        fprintf(fp, "    ");
    fprintf(fp, "%d\n", t->elem);
    print_indented_rec(t->left, fp, depth + 1);
}

void tree_print_indented(const TreeNode *t, FILE *fp)
{
    print_indented_rec(t, fp, 0);
}

/* ---------------- 4.39 / 4.40 儿子-兄弟表示 ---------------- */
CSNode *cs_from_parents(int n, const int parent[])
{
    if (n <= 0 || parent == NULL)
        return NULL;
    CSNode **nodes = calloc((size_t)n, sizeof *nodes);
    if (nodes == NULL)
        return NULL;
    for (int i = 0; i < n; ++i) {
        nodes[i] = malloc(sizeof *nodes[i]);
        if (nodes[i] == NULL) {
            for (int k = 0; k < i; ++k) free(nodes[k]);
            free(nodes);
            return NULL;
        }
        nodes[i]->elem = i;
        nodes[i]->first_child = nodes[i]->next_sibling = NULL;
    }
    CSNode *root = NULL;
    for (int i = 0; i < n; ++i) {
        if (parent[i] < 0) {
            if (root == NULL)
                root = nodes[i];
            continue;
        }
        CSNode *p = nodes[parent[i]];
        if (p->first_child == NULL) {
            p->first_child = nodes[i];
        } else {
            CSNode *s = p->first_child;
            while (s->next_sibling != NULL)      /* 追加到兄弟链尾部：保持顺序 */
                s = s->next_sibling;
            s->next_sibling = nodes[i];
        }
    }
    free(nodes);
    return root;
}

void cs_preorder(CSNode *t, FILE *fp)
{
    if (t == NULL)
        return;
    fprintf(fp, "%d ", t->elem);
    for (CSNode *c = t->first_child; c != NULL; c = c->next_sibling)
        cs_preorder(c, fp);
}

void cs_postorder(CSNode *t, FILE *fp)
{
    if (t == NULL)
        return;
    for (CSNode *c = t->first_child; c != NULL; c = c->next_sibling)
        cs_postorder(c, fp);
    fprintf(fp, "%d ", t->elem);
}

void cs_dispose(CSNode *t)
{
    if (t == NULL)
        return;
    CSNode *c = t->first_child;
    while (c != NULL) {
        CSNode *next = c->next_sibling;
        cs_dispose(c);
        c = next;
    }
    free(t);
}

int cs_depth(CSNode *t)
{
    if (t == NULL)
        return -1;
    int best = -1;
    for (CSNode *c = t->first_child; c != NULL; c = c->next_sibling) {
        int d = cs_depth(c);
        if (d > best)
            best = d;
    }
    return best + 1;
}

/* 表达式树的遍历：运算符用字符打印（内部用负的 ASCII 存运算符） */
static void expr_print_rec(const TreeNode *t, FILE *fp, int order)
{
    if (t == NULL)
        return;
    if (order == 0) {                            /* 前序 */
        if (t->elem < 0) fprintf(fp, "%c ", (char)-t->elem);
        else             fprintf(fp, "%d ", t->elem);
    }
    expr_print_rec(t->left, fp, order);
    if (order == 1) {                            /* 中序 */
        if (t->elem < 0) fprintf(fp, "%c ", (char)-t->elem);
        else             fprintf(fp, "%d ", t->elem);
    }
    expr_print_rec(t->right, fp, order);
    if (order == 2) {                            /* 后序 */
        if (t->elem < 0) fprintf(fp, "%c ", (char)-t->elem);
        else             fprintf(fp, "%d ", t->elem);
    }
}

void expr_tree_print(const TreeNode *t, FILE *fp, int order)
{
    expr_print_rec(t, fp, order);
}
