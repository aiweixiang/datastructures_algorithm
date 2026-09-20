/* 习题 3.24：斐波那契朴素递归的栈空间。
 *
 * 结论：**时间是指数级，但空间只有 O(N)**。
 *   递归树有约 2F(N+1)−1 个结点（指数级），可是它是按深度优先展开的，
 *   同一时刻栈上只有"当前的一条路径"，最深路径长度是 N，
 *   所以最大递归深度 = N，栈空间 Θ(N)。
 *   这提醒我们：递归的**时间**看递归树的结点数，**空间**看递归树的深度。
 */
#include "ch03.h"

static int fib_depth_rec(int n, int depth, int *max_depth)
{
    if (depth > *max_depth)
        *max_depth = depth;
    if (n <= 1)
        return n;
    return fib_depth_rec(n - 1, depth + 1, max_depth) +
           fib_depth_rec(n - 2, depth + 1, max_depth);
}

int fib_max_depth(int n)
{
    if (n < 0)
        return 0;
    int max_depth = 0;
    /* 从 1 开始计：fib(0) 自己也要占一帧，所以 n = 0 时深度是 1，
     * n ≥ 1 时最深路径为 fib(n) → fib(n−1) → … → fib(1)，恰好 n 层。 */
    (void)fib_depth_rec(n, 1, &max_depth);
    return max_depth;
}

/* 调用次数满足 C(0)=C(1)=1，C(n)=C(n-1)+C(n-2)+1；
 * 解得 C(n) = 2F(n+1) − 1。用递推算，不需要真的递归（否则指数时间）。 */
unsigned long long fib_call_count(int n)
{
    if (n < 0)
        return 0;
    if (n <= 1)
        return 1;
    unsigned long long c0 = 1, c1 = 1;      /* C(0), C(1) */
    for (int i = 2; i <= n; ++i) {
        unsigned long long c = c0 + c1 + 1;
        c0 = c1;
        c1 = c;
    }
    return c1;
}
