/* Chapter 3: queue as a circular array that grows by doubling.
 * Keeping an explicit `size` (instead of the front/rear/size-free trick)
 * makes empty and full distinguishable without wasting a slot, and the
 * circular buffer avoids the O(N) shift of a naive array queue.
 */
#include <stdlib.h>
#include "ch03.h"

struct QueueCircular {
    int           capacity;
    int           front;      /* index of the front element */
    int           size;
    element_type *data;
};

QueueCircular *que_create(int init_capacity)
{
    if (init_capacity < 1)
        init_capacity = 1;
    QueueCircular *q = malloc(sizeof *q);
    if (q == NULL)
        return NULL;
    q->data = malloc((size_t)init_capacity * sizeof *q->data);
    if (q->data == NULL) {
        free(q);
        return NULL;
    }
    q->capacity = init_capacity;
    q->front = 0;
    q->size = 0;
    return q;
}

void que_dispose(QueueCircular *q)
{
    if (q == NULL) return;
    free(q->data);
    free(q);
}

void que_make_empty(QueueCircular *q) { if (q) { q->size = 0; q->front = 0; } }
int  que_is_empty(const QueueCircular *q) { return q == NULL || q->size == 0; }
int  que_size(const QueueCircular *q) { return q ? q->size : 0; }

static int que_grow(QueueCircular *q)
{
    int new_cap = q->capacity * 2;
    element_type *p = malloc((size_t)new_cap * sizeof *p);
    if (p == NULL)
        return -1;
    for (int i = 0; i < q->size; ++i)      /* unroll the circle */
        p[i] = q->data[(q->front + i) % q->capacity];
    free(q->data);
    q->data = p;
    q->capacity = new_cap;
    q->front = 0;
    return 0;
}

int que_enqueue(element_type x, QueueCircular *q)
{
    if (q == NULL)
        return -1;
    if (q->size == q->capacity && que_grow(q) != 0)
        return -1;
    q->data[(q->front + q->size) % q->capacity] = x;
    ++q->size;
    return 0;
}

element_type que_front(const QueueCircular *q)
{
    if (q == NULL || q->size == 0)
        return ELEMENT_NOT_FOUND;
    return q->data[q->front];
}

element_type que_dequeue(QueueCircular *q)
{
    if (q == NULL || q->size == 0)
        return ELEMENT_NOT_FOUND;
    element_type v = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    --q->size;
    return v;
}

void que_print(const QueueCircular *q, FILE *fp)
{
    fprintf(fp, "<");
    for (int i = 0; q && i < q->size; ++i)
        fprintf(fp, "%d%s", q->data[(q->front + i) % q->capacity],
                i + 1 < q->size ? ", " : "");
    fprintf(fp, ">");
}
