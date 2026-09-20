/* 第 4 章：2-d 树（4.46 多关键字查找的一维简化）。 */
#include <stdlib.h>
#include "ch04.h"

KDTree *kd_create(void)
{
    KDTree *t = malloc(sizeof *t);
    if (t == NULL)
        return NULL;
    t->root = NULL;
    t->size = 0;
    return t;
}

static void dispose_rec(KDNode *n)
{
    if (n == NULL)
        return;
    dispose_rec(n->left);
    dispose_rec(n->right);
    free(n);
}

void kd_dispose(KDTree *t)
{
    if (t == NULL)
        return;
    dispose_rec(t->root);
    free(t);
}

static KDNode *new_node(int x, int y)
{
    KDNode *n = malloc(sizeof *n);
    if (n == NULL)
        return NULL;
    n->pt[0] = x;
    n->pt[1] = y;
    n->left = n->right = NULL;
    return n;
}

/* 交替用 x（偶数层）与 y（奇数层）作比较关键字 */
static KDNode *insert_rec(KDNode *t, int x, int y, int level, int *inserted)
{
    if (t == NULL) {
        *inserted = 1;
        return new_node(x, y);
    }
    int dim = level % 2;
    int v = (dim == 0) ? x : y;
    int tv = t->pt[dim];
    if (v < tv)
        t->left = insert_rec(t->left, x, y, level + 1, inserted);
    else if (v > tv)
        t->right = insert_rec(t->right, x, y, level + 1, inserted);
    else {
        if (t->pt[0] == x && t->pt[1] == y)
            return t;                                  /* 完全重复 */
        /* 该维度相等但另一个维度不同：固定往右放，保证搜索时两棵子树都查 */
        t->right = insert_rec(t->right, x, y, level + 1, inserted);
    }
    return t;
}

int kd_insert(int x, int y, KDTree *t)
{
    if (t == NULL)
        return -1;
    int inserted = 0;
    t->root = insert_rec(t->root, x, y, 0, &inserted);
    if (inserted)
        ++t->size;
    return inserted ? 0 : -1;
}

static const KDNode *contains_rec(const KDNode *t, int x, int y, int level)
{
    if (t == NULL)
        return NULL;
    if (t->pt[0] == x && t->pt[1] == y)
        return t;
    int dim = level % 2;
    int v = (dim == 0) ? x : y;
    int tv = t->pt[dim];
    if (v < tv)
        return contains_rec(t->left, x, y, level + 1);
    if (v > tv)
        return contains_rec(t->right, x, y, level + 1);
    /* 该维相等：两侧都可能藏着目标 */
    const KDNode *l = contains_rec(t->left, x, y, level + 1);
    return l != NULL ? l : contains_rec(t->right, x, y, level + 1);
}

int kd_contains(int x, int y, const KDTree *t)
{
    return t != NULL && contains_rec(t->root, x, y, 0) != NULL;
}

static int range_rec(const KDNode *t, int level,
                     int x1, int x2, int y1, int y2,
                     int out[][2], int max, int *written)
{
    if (t == NULL)
        return 0;
    int count = 0;
    if (t->pt[0] >= x1 && t->pt[0] <= x2 && t->pt[1] >= y1 && t->pt[1] <= y2) {
        if (*written < max) {
            out[*written][0] = t->pt[0];
            out[*written][1] = t->pt[1];
            ++*written;
        }
        ++count;
    }
    int dim = level % 2;
    int v = t->pt[dim];
    int lo = (dim == 0) ? x1 : y1;
    int hi = (dim == 0) ? x2 : y2;
    /* 左子树的关键字严格 < v，右子树 >= v（该维相等但另维不同的点统一放右边）。
     * 因此右边要用 hi >= v：hi == v 时右边可能还藏着该维恰好等于 v
     * （即正好落在边界上）的点，用 hi > v 会把它们全部漏掉。 */
    if (lo < v)
        count += range_rec(t->left, level + 1, x1, x2, y1, y2, out, max, written);
    if (hi >= v)
        count += range_rec(t->right, level + 1, x1, x2, y1, y2, out, max, written);
    return count;
}

int kd_range(const KDTree *t, int x1, int x2, int y1, int y2,
             int out[][2], int max)
{
    if (t == NULL || out == NULL || max <= 0 || x1 > x2 || y1 > y2)
        return 0;
    int written = 0;
    return range_rec(t->root, 0, x1, x2, y1, y2, out, max, &written);
}

int kd_size(const KDTree *t) { return t ? t->size : 0; }

static void nearest_rec(const KDNode *t, int level, int qx, int qy,
                        long long *best, int bestpt[2])
{
    if (t == NULL)
        return;
    long long dx = t->pt[0] - qx, dy = t->pt[1] - qy;
    long long d = dx * dx + dy * dy;
    if (d < *best) {
        *best = d;
        bestpt[0] = t->pt[0];
        bestpt[1] = t->pt[1];
    }
    int dim = level % 2;
    int qv = (dim == 0) ? qx : qy;
    long long diff = qv - t->pt[dim];
    const KDNode *near = (diff < 0) ? t->left : t->right;
    const KDNode *far  = (diff < 0) ? t->right : t->left;
    nearest_rec(near, level + 1, qx, qy, best, bestpt);
    /* 只有"到分割平面的距离"小于当前最优时，另一侧才可能更近 */
    if (diff * diff < *best)
        nearest_rec(far, level + 1, qx, qy, best, bestpt);
}

int kd_nearest(const KDTree *t, int x, int y, int *best)
{
    if (t == NULL || t->root == NULL) {
        if (best != NULL) { best[0] = 0; best[1] = 0; }
        return -1;
    }
    long long b = 0x7fffffffffffffffLL;
    int pt[2] = {0, 0};
    nearest_rec(t->root, 0, x, y, &b, pt);
    if (best != NULL) {
        best[0] = pt[0];
        best[1] = pt[1];
    }
    return (int)b;
}
