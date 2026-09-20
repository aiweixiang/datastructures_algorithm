/* 习题 3.26：双端队列（deque），两端插入/删除全部 O(1)（摊还）。
 *
 * 实现：循环数组 + 倍增扩容。用 front 与 size 描述队列（而不是 front/rear），
 * 这样"空"和"满"不再歧义，也不需要浪费一个槽位。
 *   front 指向队首，队尾下标 = (front + size - 1) % capacity
 *   push_front: front = (front - 1 + capacity) % capacity
 *   push_back : (front + size) % capacity
 */
#include <stdlib.h>
#include "ch03.h"

struct Deque {
    element_type *data;
    int           capacity;
    int           front;
    int           size;
};

Deque *deq_create(int init_capacity)
{
    if (init_capacity < 1)
        init_capacity = 1;
    Deque *d = malloc(sizeof *d);
    if (d == NULL)
        return NULL;
    d->data = malloc((size_t)init_capacity * sizeof *d->data);
    if (d->data == NULL) {
        free(d);
        return NULL;
    }
    d->capacity = init_capacity;
    d->front = 0;
    d->size = 0;
    return d;
}

void deq_dispose(Deque *d)
{
    if (d == NULL)
        return;
    free(d->data);
    free(d);
}

void deq_make_empty(Deque *d)
{
    if (d != NULL) {
        d->size = 0;
        d->front = 0;
    }
}

int deq_is_empty(const Deque *d) { return d == NULL || d->size == 0; }
int deq_size(const Deque *d) { return d ? d->size : 0; }
int deq_capacity(const Deque *d) { return d ? d->capacity : 0; }

static int deq_grow(Deque *d)
{
    int nc = d->capacity * 2;
    element_type *p = malloc((size_t)nc * sizeof *p);
    if (p == NULL)
        return -1;
    for (int i = 0; i < d->size; ++i)
        p[i] = d->data[(d->front + i) % d->capacity];
    free(d->data);
    d->data = p;
    d->capacity = nc;
    d->front = 0;
    return 0;
}

int deq_push_back(element_type x, Deque *d)
{
    if (d == NULL)
        return -1;
    if (d->size == d->capacity && deq_grow(d) != 0)
        return -1;
    d->data[(d->front + d->size) % d->capacity] = x;
    ++d->size;
    return 0;
}

int deq_push_front(element_type x, Deque *d)
{
    if (d == NULL)
        return -1;
    if (d->size == d->capacity && deq_grow(d) != 0)
        return -1;
    d->front = (d->front - 1 + d->capacity) % d->capacity;
    d->data[d->front] = x;
    ++d->size;
    return 0;
}

element_type deq_front(const Deque *d)
{
    if (d == NULL || d->size == 0)
        return ELEMENT_NOT_FOUND;
    return d->data[d->front];
}

element_type deq_back(const Deque *d)
{
    if (d == NULL || d->size == 0)
        return ELEMENT_NOT_FOUND;
    return d->data[(d->front + d->size - 1) % d->capacity];
}

element_type deq_pop_front(Deque *d)
{
    if (d == NULL || d->size == 0)
        return ELEMENT_NOT_FOUND;
    element_type v = d->data[d->front];
    d->front = (d->front + 1) % d->capacity;
    --d->size;
    return v;
}

element_type deq_pop_back(Deque *d)
{
    if (d == NULL || d->size == 0)
        return ELEMENT_NOT_FOUND;
    element_type v = d->data[(d->front + d->size - 1) % d->capacity];
    --d->size;
    return v;
}

void deq_print(const Deque *d, FILE *fp)
{
    fprintf(fp, "[");
    for (int i = 0; d != NULL && i < d->size; ++i)
        fprintf(fp, "%d%s", d->data[(d->front + i) % d->capacity],
                i + 1 < d->size ? ", " : "");
    fprintf(fp, "]");
}
