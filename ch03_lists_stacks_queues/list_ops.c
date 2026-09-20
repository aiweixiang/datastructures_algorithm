/* Chapter 3: set operations on sorted lists (intersection and union in a
 * single linear pass each) -- the O(N+M) "merge" pattern.
 */
#include "ch03.h"

int sorted_intersect(const int a[], int na, const int b[], int nb, int out[])
{
    int i = 0, j = 0, k = 0;
    while (i < na && j < nb) {
        if (a[i] < b[j]) {
            ++i;
        } else if (a[i] > b[j]) {
            ++j;
        } else {
            out[k++] = a[i];
            while (i + 1 < na && a[i + 1] == a[i]) ++i;   /* skip dups in a */
            while (j + 1 < nb && b[j + 1] == b[j]) ++j;   /* skip dups in b */
            ++i; ++j;
        }
    }
    return k;
}

int sorted_union(const int a[], int na, const int b[], int nb, int out[])
{
    int i = 0, j = 0, k = 0;
    while (i < na || j < nb) {
        int v;
        if (i < na && (j >= nb || a[i] < b[j])) {
            v = a[i++];
        } else if (j < nb && (i >= na || b[j] < a[i])) {
            v = b[j++];
        } else {
            v = a[i];                            /* equal: emit once */
            ++i; ++j;
        }
        if (k == 0 || out[k - 1] != v)
            out[k++] = v;
    }
    return k;
}
