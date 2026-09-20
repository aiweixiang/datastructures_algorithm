/* Chapter 2: the four classic maximum-subsequence-sum algorithms.
 * The empty subsequence is allowed, so the answer is never negative.
 */
#include "ch02.h"

int mss_cubic(const int a[], int n)
{
    int max_sum = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = i; j < n; ++j) {
            int this_sum = 0;
            for (int k = i; k <= j; ++k)
                this_sum += a[k];
            if (this_sum > max_sum)
                max_sum = this_sum;
        }
    }
    return max_sum;
}

int mss_quadratic(const int a[], int n)
{
    int max_sum = 0;
    for (int i = 0; i < n; ++i) {
        int this_sum = 0;
        for (int j = i; j < n; ++j) {
            this_sum += a[j];       /* reuse the previous partial sum */
            if (this_sum > max_sum)
                max_sum = this_sum;
        }
    }
    return max_sum;
}

static int max3(int a, int b, int c)
{
    int m = a > b ? a : b;
    return m > c ? m : c;
}

/* Best sum that crosses the midpoint: best suffix ending at center
 * plus best prefix starting at center + 1. */
static int mss_dc(const int a[], int left, int right)
{
    if (left == right)
        return a[left] > 0 ? a[left] : 0;

    int center = left + (right - left) / 2;

    int max_left_sum = 0, left_sum = 0;
    for (int i = center; i >= left; --i) {
        left_sum += a[i];
        if (left_sum > max_left_sum)
            max_left_sum = left_sum;
    }
    int max_right_sum = 0, right_sum = 0;
    for (int i = center + 1; i <= right; ++i) {
        right_sum += a[i];
        if (right_sum > max_right_sum)
            max_right_sum = right_sum;
    }

    return max3(max_left_sum + max_right_sum,
                mss_dc(a, left, center),
                mss_dc(a, center + 1, right));
}

int mss_divide_conquer(const int a[], int n)
{
    return n > 0 ? mss_dc(a, 0, n - 1) : 0;
}

int mss_linear(const int a[], int n)
{
    int max_sum = 0, this_sum = 0;
    for (int i = 0; i < n; ++i) {
        this_sum += a[i];
        if (this_sum > max_sum)
            max_sum = this_sum;
        else if (this_sum < 0)
            this_sum = 0;       /* a negative prefix can only hurt */
    }
    return max_sum;
}

int mss_linear_range(const int a[], int n, int *start, int *end)
{
    int max_sum = 0, this_sum = 0;
    int best_start = -1, best_end = -1, cur_start = 0;

    for (int i = 0; i < n; ++i) {
        this_sum += a[i];
        if (this_sum > max_sum) {
            max_sum = this_sum;
            best_start = cur_start;
            best_end = i;
        } else if (this_sum < 0) {
            this_sum = 0;
            cur_start = i + 1;
        }
    }
    if (start)
        *start = best_start;
    if (end)
        *end = best_end;
    return max_sum;
}
