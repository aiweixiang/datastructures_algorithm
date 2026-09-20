/* 习题 3.17：懒惰删除（lazy deletion）。
 *
 * 思想：删除时只**打标记**，不做数据搬移；当"已删除元素个数 > 存活元素个数"时
 * 才真正做一次整理（compact）。插入时优先复用已删除的槽位。
 *
 * 复杂度分析（这是本题的考点）：
 *   一次 compact 的代价是 O(size)，但它之后"已删除"数量至少减半；
 *   而两次 compact 之间至少发生了 (存活数) 次删除 ⇒ 摊还到每次删除是 O(1)。
 *   缺点：每个元素多一个标记位/字节；find 需要跳过已删除项，最坏仍是 O(N)；
 *        且元素个数在 compact 前会虚高（size 不等于存活数）。
 *   （与"删除时立即搬移"的 O(N) 相比，懒惰删除在"删多查少"的场景优势明显。）
 */
#include <stdlib.h>
#include "ch03.h"

struct LazyList {
    element_type  *data;
    unsigned char *deleted;
    int            capacity;
    int            size;        /* 含已删除的槽位总数 */
    int            live;        /* 存活元素个数 */
    int            compactions; /* 统计整理次数（便于测试摊还性质） */
};

LazyList *lz_create(int cap)
{
    if (cap < 1)
        cap = 1;
    LazyList *l = malloc(sizeof *l);
    if (l == NULL)
        return NULL;
    l->data = malloc((size_t)cap * sizeof *l->data);
    l->deleted = calloc((size_t)cap, 1);
    if (l->data == NULL || l->deleted == NULL) {
        free(l->data);
        free(l->deleted);
        free(l);
        return NULL;
    }
    l->capacity = cap;
    l->size = 0;
    l->live = 0;
    l->compactions = 0;
    return l;
}

void lz_dispose(LazyList *l)
{
    if (l == NULL)
        return;
    free(l->data);
    free(l->deleted);
    free(l);
}

int lz_compact(LazyList *l)
{
    if (l == NULL)
        return -1;
    if (l->live != l->size) {
        int m = 0;
        for (int i = 0; i < l->size; ++i)
            if (!l->deleted[i]) {
                l->data[m] = l->data[i];
                l->deleted[m] = 0;
                ++m;
            }
        for (int i = m; i < l->size; ++i)
            l->deleted[i] = 0;
        l->size = m;
        ++l->compactions;
    }
    return l->size;
}

static int lz_grow(LazyList *l)
{
    int nc = l->capacity * 2;
    element_type *d = realloc(l->data, (size_t)nc * sizeof *d);
    if (d == NULL)
        return -1;
    l->data = d;
    unsigned char *del = realloc(l->deleted, (size_t)nc);
    if (del == NULL)
        return -1;
    l->deleted = del;
    for (int i = l->capacity; i < nc; ++i)
        l->deleted[i] = 0;
    l->capacity = nc;
    return 0;
}

int lz_insert(element_type x, LazyList *l)
{
    if (l == NULL)
        return -1;
    for (int i = 0; i < l->size; ++i)          /* 优先复用已删除槽位 */
        if (l->deleted[i]) {
            l->data[i] = x;
            l->deleted[i] = 0;
            ++l->live;
            return 0;
        }
    if (l->size == l->capacity && lz_grow(l) != 0)
        return -1;
    l->data[l->size] = x;
    l->deleted[l->size] = 0;
    ++l->size;
    ++l->live;
    return 0;
}

int lz_find(element_type x, const LazyList *l)
{
    if (l == NULL)
        return ELEMENT_NOT_FOUND;
    for (int i = 0; i < l->size; ++i)
        if (!l->deleted[i] && l->data[i] == x)
            return i;
    return ELEMENT_NOT_FOUND;
}

int lz_delete(element_type x, LazyList *l)
{
    if (l == NULL)
        return -1;
    for (int i = 0; i < l->size; ++i) {
        if (!l->deleted[i] && l->data[i] == x) {
            l->deleted[i] = 1;
            --l->live;
            /* 已是垃圾多于有效数据：立刻整理，保证摊还 O(1) */
            if (l->live * 2 < l->size)
                lz_compact(l);
            return 0;
        }
    }
    return -1;
}

int lz_live(const LazyList *l) { return l ? l->live : 0; }
int lz_deleted(const LazyList *l) { return l ? l->size - l->live : 0; }
int lz_slots(const LazyList *l) { return l ? l->size : 0; }
int lz_compactions(const LazyList *l) { return l ? l->compactions : 0; }
