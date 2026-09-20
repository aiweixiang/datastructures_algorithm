/* 第 4 章：线索二叉树（4.45）。这里实现**右线索**（right thread 指向中序后继）。
 *
 * 插入：新结点一定是叶子，其右指针置为线索。
 *   - 作为左儿子插入：它的中序后继就是父结点；
 *   - 作为右儿子插入：它继承父结点的线索（父的后继就是它的后继），
 *     同时把父结点的右指针改成真孩子（right_thread = 0）。
 * 删除：本实现采用"标准 BST 删除 + 重新穿线"（O(N)）。
 *   局部改线可以做到 O(1)~O(log N)，但需要区分"被删结点是不是某个祖先的后继"
 *   等 4 种情形，代码量大且易错；两种做法的取舍写在 ANSWERS.md 里。
 * 中序遍历**不用栈也不用递归**：靠线索找到后继。
 */
#include <stdlib.h>
#include "ch04.h"

static ThreadNode *new_node(int x)
{
    ThreadNode *n = malloc(sizeof *n);
    if (n == NULL)
        return NULL;
    n->elem = x;
    n->left = NULL;
    n->right = NULL;
    n->right_thread = 1;         /* 新结点是叶子：右指针是线索 */
    return n;
}

ThreadTree *thread_create(void)
{
    ThreadTree *t = malloc(sizeof *t);
    if (t == NULL)
        return NULL;
    t->root = NULL;
    t->size = 0;
    return t;
}

static void dispose_rec(ThreadNode *n)
{
    if (n == NULL)
        return;
    if (n->left != NULL)
        dispose_rec(n->left);
    if (!n->right_thread && n->right != NULL)
        dispose_rec(n->right);
    free(n);
}

void thread_dispose(ThreadTree *t)
{
    if (t == NULL)
        return;
    dispose_rec(t->root);
    free(t);
}

int thread_insert(int x, ThreadTree *t)
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

    ThreadNode *p = t->root, *parent = NULL;
    while (p != NULL) {
        parent = p;
        if (x < p->elem) {
            if (p->left == NULL) {                       /* 作为左儿子插入 */
                ThreadNode *n = new_node(x);
                if (n == NULL) return -1;
                n->right = p;                            /* 后继 = 父结点 */
                n->right_thread = 1;
                p->left = n;
                ++t->size;
                return 0;
            }
            p = p->left;
        } else if (x > p->elem) {
            if (p->right_thread || p->right == NULL) {   /* 作为右儿子插入 */
                ThreadNode *n = new_node(x);
                if (n == NULL) return -1;
                n->right = p->right;                     /* 继承父的线索 */
                n->right_thread = 1;
                p->right = n;
                p->right_thread = 0;
                ++t->size;
                return 0;
            }
            p = p->right;
        } else {
            return -1;                                   /* 重复 */
        }
    }
    (void)parent;
    return -1;
}

ThreadNode *thread_find(int x, const ThreadTree *t)
{
    if (t == NULL)
        return NULL;
    ThreadNode *p = t->root;
    while (p != NULL) {
        if (x < p->elem) {
            if (p->left == NULL) return NULL;
            p = p->left;
        } else if (x > p->elem) {
            if (p->right_thread) return NULL;
            p = p->right;
        } else {
            return p;
        }
    }
    return NULL;
}

/* 按中序顺序重新穿线：把每个结点的右线索指向后继，并同步维护
 * right_thread 标志（1 = right 是指向后继的线索，0 = right 是真孩子）。
 *
 * 旧实现只改写线索指针，却从不重算 right_thread，删除后两者不一致：
 * 某个 right_thread=0 的结点，其 right 已变成指回祖先的线索，遍历时
 * 就会把祖先当成孩子反复下行，造成死循环。这里用真实结构
 * （right_thread 区分真孩子/线索）做一次完整重建。 */
static void rethread_rec(ThreadNode *n, ThreadNode **prev)
{
    if (n == NULL)
        return;
    rethread_rec(n->left, prev);
    if (*prev != NULL && (*prev)->right_thread)
        (*prev)->right = n;              /* 前驱的线索指向当前后继 */
    *prev = n;
    if (!n->right_thread && n->right != NULL) {
        rethread_rec(n->right, prev);    /* 真孩子：继续，保留 right */
    } else {
        n->right = NULL;                 /* 线索暂置空，由后继回填 */
        n->right_thread = 1;
    }
}

static void rethread(ThreadTree *t)
{
    ThreadNode *prev = NULL;
    rethread_rec(t->root, &prev);
    if (prev != NULL) {                  /* 中序最大元：无线索指向 NULL */
        prev->right = NULL;
        prev->right_thread = 1;
    }
}

int thread_delete(int x, ThreadTree *t)
{
    if (t == NULL || t->root == NULL)
        return -1;

    /* 1) 标准 BST 删除（只把 right_thread = 1 当作"无右孩子"） */
    ThreadNode *parent = NULL, *cur = t->root;
    while (cur != NULL && cur->elem != x) {
        parent = cur;
        if (x < cur->elem)
            cur = cur->left;
        else if (cur->right_thread)
            return -1;                        /* 无右孩子 → 找不到 */
        else
            cur = cur->right;
    }
    if (cur == NULL)
        return -1;

    if (cur->left != NULL && !cur->right_thread && cur->right != NULL) {
        /* 两个孩子：用中序后继的值覆盖，然后删除后继（后继必无左孩子） */
        ThreadNode *sp = cur, *s = cur->right;
        while (s->left != NULL) {
            sp = s;
            s = s->left;
        }
        cur->elem = s->elem;
        /* 删除 s：s 无左孩子 */
        if (sp == cur) {
            sp->right = s->right;
            sp->right_thread = s->right_thread;
        } else {
            /* s 是左子树上最左结点，本无左孩子；它的位置由它的右孩子顶上。
             * 若 s->right 是线索而不是真孩子，必须置 NULL，否则会把
             * 一个指向后继（甚至是祖先）的线索当成真实左孩子，
             * 建线/遍历时就会形成环。 */
            sp->left = s->right_thread ? NULL : s->right;
        }
        free(s);
    } else {
        /* 至多一个孩子 */
        ThreadNode *child = (cur->left != NULL) ? cur->left : NULL;
        if (cur->left != NULL) {
            child = cur->left;
        } else if (!cur->right_thread && cur->right != NULL) {
            child = cur->right;
        } else {
            child = NULL;                     /* 叶子（右指针是线索） */
        }
        if (parent == NULL)
            t->root = child;
        else if (parent->left == cur)
            parent->left = child;
        else {
            parent->right = child;
            parent->right_thread = (child == NULL) ? 1 : 0;
        }
        free(cur);
    }
    --t->size;
    rethread(t);
    return 0;
}

int thread_size(const ThreadTree *t) { return t ? t->size : 0; }

/* 中序遍历：不用栈、不用递归 */
void thread_inorder(const ThreadTree *t, FILE *fp)
{
    if (t == NULL || t->root == NULL)
        return;
    ThreadNode *cur = t->root;
    while (cur->left != NULL)
        cur = cur->left;
    while (cur != NULL) {
        fprintf(fp, "%d ", cur->elem);
        if (cur->right_thread || cur->right == NULL)
            cur = cur->right;
        else {
            cur = cur->right;
            while (cur->left != NULL)
                cur = cur->left;
        }
    }
}

static int th_height_rec(const ThreadNode *n)
{
    if (n == NULL)
        return -1;
    int l = th_height_rec(n->left);
    int r = (n->right_thread || n->right == NULL) ? -1 : th_height_rec(n->right);
    return (l > r ? l : r) + 1;
}

int thread_height(const ThreadTree *t)
{
    return t ? th_height_rec(t->root) : -1;
}
