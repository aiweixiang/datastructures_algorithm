/* 习题 3.14：把图读入邻接表 —— 分别用**链表**和**游标（数组）**实现。
 *
 * 顶点编号 1..N（教材风格）。输入格式：每行 "u v [w]"，w 省略时默认 1；
 * 以 '#' 开头的行和空行被忽略。默认按**无向图**处理（加两条有向边）。
 *
 * 两种实现的取舍与表 ADT 一样：
 *   链表版：结点按需 malloc，容量无上限，但每个结点一个指针；
 *   游标版：一次性分配边数组 + 表头数组，可整体持久化，但容量必须预先给定，
 *           且需要自己管理"空闲边"的复用。
 */
#include <stdio.h>
#include <stdlib.h>
#include "ch03.h"

/* ---------------- 链表版 ---------------- */
AdjGraph *adj_create(int n)
{
    if (n < 0)
        return NULL;
    AdjGraph *g = malloc(sizeof *g);
    if (g == NULL)
        return NULL;
    g->head = calloc((size_t)n + 1, sizeof *g->head);   /* 下标 1..n */
    if (g->head == NULL) {
        free(g);
        return NULL;
    }
    g->n = n;
    g->m = 0;
    return g;
}

void adj_dispose(AdjGraph *g)
{
    if (g == NULL)
        return;
    for (int v = 1; v <= g->n; ++v) {
        AdjNode *p = g->head[v];
        while (p != NULL) {
            AdjNode *nx = p->next;
            free(p);
            p = nx;
        }
    }
    free(g->head);
    free(g);
}

int adj_add_edge(AdjGraph *g, int u, int v, int w, int undirected)
{
    if (g == NULL || u < 1 || u > g->n || v < 1 || v > g->n)
        return -1;
    AdjNode *a = malloc(sizeof *a);
    if (a == NULL)
        return -1;
    a->vertex = v;
    a->weight = w;
    a->next = g->head[u];
    g->head[u] = a;                       /* 头插：O(1) */
    ++g->m;

    if (undirected) {
        AdjNode *b = malloc(sizeof *b);
        if (b == NULL)
            return -1;
        b->vertex = u;
        b->weight = w;
        b->next = g->head[v];
        g->head[v] = b;
        ++g->m;
    }
    return 0;
}

AdjGraph *adj_read(FILE *fp, int n)
{
    if (fp == NULL)
        return NULL;
    AdjGraph *g = adj_create(n);
    if (g == NULL)
        return NULL;

    char line[256];
    while (fgets(line, sizeof line, fp) != NULL) {
        if (line[0] == '#' || line[0] == '\n')
            continue;
        int u = 0, v = 0, w = 1;
        int k = sscanf(line, "%d %d %d", &u, &v, &w);
        if (k >= 2)
            adj_add_edge(g, u, v, w, 1);   /* k == 2 时 w 保持默认值 1 */
    }
    return g;
}

void adj_print(const AdjGraph *g, FILE *fp)
{
    for (int v = 1; g != NULL && v <= g->n; ++v) {
        fprintf(fp, "%d:", v);
        for (AdjNode *p = g->head[v]; p != NULL; p = p->next)
            fprintf(fp, " %d(%d)", p->vertex, p->weight);
        fprintf(fp, "\n");
    }
}

/* ---------------- 游标（数组）版 ---------------- */
CursorAdjGraph *cadg_create(int n, int max_edges)
{
    if (n < 0 || max_edges < 0)
        return NULL;
    CursorAdjGraph *g = malloc(sizeof *g);
    if (g == NULL)
        return NULL;
    g->edges = malloc(((size_t)max_edges + 1) * sizeof *g->edges);
    g->head = calloc((size_t)n + 1, sizeof *g->head);
    if (g->edges == NULL || g->head == NULL) {
        free(g->edges);
        free(g->head);
        free(g);
        return NULL;
    }
    for (int i = 1; i <= max_edges; ++i)
        g->edges[i].next = CURSOR_NULL;
    /* 关键：head 的初值必须是 CURSOR_NULL（-1）而不是 0——
     * 本实现里 0 是合法的边下标惯例（edges[1..capacity]），
     * 若用 0 当"空"会让遍历走到未初始化的 edges[0]。 */
    for (int v = 1; v <= n; ++v)
        g->head[v] = CURSOR_NULL;
    g->n = n;
    g->m = 0;
    g->capacity = max_edges;
    return g;
}

void cadg_dispose(CursorAdjGraph *g)
{
    if (g == NULL)
        return;
    free(g->edges);
    free(g->head);
    free(g);
}

int cadg_add_edge(CursorAdjGraph *g, int u, int v, int w, int undirected)
{
    if (g == NULL || u < 1 || u > g->n || v < 1 || v > g->n)
        return -1;
    if (g->m + (undirected ? 2 : 1) > g->capacity)
        return -1;                          /* 真正的溢出：空间池用尽 */

    ++g->m;
    g->edges[g->m].vertex = v;
    g->edges[g->m].weight = w;
    g->edges[g->m].next = g->head[u];
    g->head[u] = g->m;

    if (undirected) {
        ++g->m;
        g->edges[g->m].vertex = u;
        g->edges[g->m].weight = w;
        g->edges[g->m].next = g->head[v];
        g->head[v] = g->m;
    }
    return 0;
}

/* 把链表版整体转成游标版，便于两种实现互相对拍。
 * 注意：游标版用头插法，若按链表顺序依次插入会得到**逆序**的邻接表，
 * 所以这里先收集再逆序插入，保证两种实现的邻接表顺序完全一致。 */
CursorAdjGraph *cadg_from_adj(const AdjGraph *g)
{
    if (g == NULL)
        return NULL;
    CursorAdjGraph *c = cadg_create(g->n, g->m > 0 ? g->m : 1);
    if (c == NULL)
        return NULL;

    for (int v = 1; v <= g->n; ++v) {
        int cnt = 0;
        for (AdjNode *p = g->head[v]; p != NULL; p = p->next)
            ++cnt;
        if (cnt == 0)
            continue;

        int *vs = malloc((size_t)cnt * sizeof *vs);
        int *ws = malloc((size_t)cnt * sizeof *ws);
        if (vs == NULL || ws == NULL) {
            free(vs);
            free(ws);
            cadg_dispose(c);
            return NULL;
        }
        int i = 0;
        for (AdjNode *p = g->head[v]; p != NULL; p = p->next) {
            vs[i] = p->vertex;
            ws[i] = p->weight;
            ++i;
        }
        for (i = cnt - 1; i >= 0; --i) {
            if (cadg_add_edge(c, v, vs[i], ws[i], 0) != 0) {
                free(vs);
                free(ws);
                cadg_dispose(c);
                return NULL;
            }
        }
        free(vs);
        free(ws);
    }
    return c;
}
