/* Chapter 3: the Josephus problem -- N people in a circle, every m-th
 * person is eliminated, who survives?  Two answers: a direct simulation
 * with a circular linked list, O(N*m), and the O(N) recurrence
 *     J(1) = 0,  J(n) = (J(n-1) + m) mod n     (0-based)
 */
#include <stdlib.h>
#include "ch03.h"

int josephus_linked(int n, int m)
{
    if (n <= 0 || m <= 0)
        return -1;
    if (n == 1)
        return 1;

    Node *nodes = malloc((size_t)n * sizeof *nodes);
    if (nodes == NULL)
        return -1;
    for (int i = 0; i < n; ++i) {
        nodes[i].elem = i + 1;
        nodes[i].next = &nodes[(i + 1) % n];    /* a circle */
    }

    Node *cur = &nodes[0];
    Node *prev = &nodes[n - 1];
    while (cur->next != cur) {
        for (int i = 1; i < m; ++i) {           /* count cur as 1 */
            prev = cur;
            cur = cur->next;
        }
        prev->next = cur->next;                 /* eliminate cur */
        cur = cur->next;                        /* next person counts 1 */
    }

    int survivor = cur->elem;
    free(nodes);
    return survivor;
}

int josephus_formula(int n, int m)
{
    if (n <= 0 || m <= 0)
        return -1;
    int j = 0;
    for (int i = 2; i <= n; ++i)
        j = (j + m) % i;
    return j + 1;                               /* convert to 1-based */
}
