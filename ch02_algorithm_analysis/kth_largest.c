/* Chapter 1/2/7: the selection problem -- find the k-th largest element.
 * Three answers with different cost profiles, mirroring Weiss's discussion:
 *   sort      O(N log N) time, destroys the array
 *   heap      O(N log k) time, O(k) extra space, leaves the input alone
 *   quickselect O(N) average time, destroys the array
 */
#include <limits.h>
#include <stdlib.h>
#include "ch02.h"

int kth_largest_sort(int a[], int n, int k)
{
    if (a == NULL || k < 1 || k > n)
        return INT_MIN;
    for (int i = 1; i < n; ++i) {          /* insertion sort, descending */
        int key = a[i], j = i - 1;
        while (j >= 0 && a[j] < key) {
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = key;
    }
    return a[k - 1];
}

/* Min-heap of the k largest values seen so far: the root is the answer. */
static void heap_sift_down(int heap[], int size, int i)
{
    for (;;) {
        int l = 2 * i + 1, r = 2 * i + 2, m = i;
        if (l < size && heap[l] < heap[m]) m = l;
        if (r < size && heap[r] < heap[m]) m = r;
        if (m == i) return;
        int t = heap[i]; heap[i] = heap[m]; heap[m] = t;
        i = m;
    }
}

int kth_largest_heap(const int a[], int n, int k)
{
    if (a == NULL || k < 1 || k > n)
        return INT_MIN;

    int *heap = malloc((size_t)k * sizeof *heap);
    if (heap == NULL)
        return INT_MIN;

    for (int i = 0; i < k; ++i)
        heap[i] = a[i];
    for (int i = k / 2 - 1; i >= 0; --i)      /* build min-heap */
        heap_sift_down(heap, k, i);

    for (int i = k; i < n; ++i) {
        if (a[i] > heap[0]) {
            heap[0] = a[i];
            heap_sift_down(heap, k, 0);
        }
    }
    int answer = heap[0];
    free(heap);
    return answer;
}

static int partition_desc(int a[], int lo, int hi)
{
    int pivot = a[hi], i = lo;
    for (int j = lo; j < hi; ++j) {
        if (a[j] > pivot) {
            int t = a[i]; a[i] = a[j]; a[j] = t;
            ++i;
        }
    }
    int t = a[i]; a[i] = a[hi]; a[hi] = t;
    return i;
}

int kth_largest_quickselect(int a[], int n, int k)
{
    if (a == NULL || k < 1 || k > n)
        return INT_MIN;

    int lo = 0, hi = n - 1, target = k - 1;
    while (lo <= hi) {
        int p = partition_desc(a, lo, hi);
        if (p == target)   return a[p];
        if (p < target)    lo = p + 1;    /* answer is to the right */
        else               hi = p - 1;
    }
    return INT_MIN;
}
