/* Chapter 2: binary search, including the overflow-safe midpoint and the
 * lower/upper bound variants used by the standard library.
 * Precondition for every routine: a[0..n-1] is sorted in non-decreasing order.
 */
#include "ch02.h"

int bisect(const int a[], int x, int n)
{
    int low = 0, high = n - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;   /* avoids low + high overflow */
        if (a[mid] < x)
            low = mid + 1;
        else if (a[mid] > x)
            high = mid - 1;
        else
            return mid;
    }
    return BISECT_NOT_FOUND;
}

int bisect_recursive(const int a[], int x, int lo, int hi)
{
    if (lo > hi)
        return BISECT_NOT_FOUND;
    int mid = lo + (hi - lo) / 2;
    if (a[mid] < x)
        return bisect_recursive(a, x, mid + 1, hi);
    if (a[mid] > x)
        return bisect_recursive(a, x, lo, mid - 1);
    return mid;
}

int lower_bound_idx(const int a[], int x, int n)
{
    int lo = 0, hi = n;                 /* answer in [0, n] */
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] < x)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}

int upper_bound_idx(const int a[], int x, int n)
{
    int lo = 0, hi = n;
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] <= x)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}
