/* 习题 3.9：任意精度整数运算包。
 *
 * 表示：十进制位数组（小端，dig[0] 是个位）+ 符号（-1/0/+1）。
 * 算法：加法/减法 O(N)，乘法用教科书竖式 O(N·M)。
 * 用十进制位而不是 base 2^32 是为了让"打印/解析/进位"与人类直觉一致，
 * 常数差一点但代码可读性好得多；工程实现常用 base 10^9（每个 limb 存 9 位）。
 *
 * 测试：2^100、20! 与 64 位算术对拍、100! 的尾零个数（应为 24 个）。
 */
#include <stdlib.h>
#include <string.h>
#include "ch03.h"

struct BigInt {
    int            sign;   /* -1 / 0 / +1 */
    int            len;    /* 有效位数，至少 1 */
    int            cap;
    unsigned char *dig;    /* 小端：dig[0] 是个位 */
};

static BigInt *bi_alloc(int cap)
{
    if (cap < 1)
        cap = 1;
    BigInt *b = malloc(sizeof *b);
    if (b == NULL)
        return NULL;
    b->dig = calloc((size_t)cap, 1);
    if (b->dig == NULL) {
        free(b);
        return NULL;
    }
    b->cap = cap;
    b->len = 1;
    b->sign = 0;                 /* 初始为 0 */
    return b;
}

/* 去掉高位零，并把全零规范成 sign = 0 */
static void bi_norm(BigInt *b)
{
    while (b->len > 1 && b->dig[b->len - 1] == 0)
        --b->len;
    if (b->len == 1 && b->dig[0] == 0)
        b->sign = 0;
    else if (b->sign == 0)
        b->sign = 1;
}

static BigInt *bi_copy(const BigInt *a)
{
    BigInt *r = bi_alloc(a->len);
    if (r == NULL)
        return NULL;
    memcpy(r->dig, a->dig, (size_t)a->len);
    r->len = a->len;
    r->sign = a->sign;
    return r;
}

static int bi_cmp_mag(const BigInt *a, const BigInt *b)
{
    if (a->len != b->len)
        return a->len < b->len ? -1 : 1;
    for (int i = a->len - 1; i >= 0; --i)
        if (a->dig[i] != b->dig[i])
            return a->dig[i] < b->dig[i] ? -1 : 1;
    return 0;
}

static BigInt *bi_add_mag(const BigInt *a, const BigInt *b)
{
    int n = (a->len > b->len ? a->len : b->len) + 1;
    BigInt *r = bi_alloc(n);
    if (r == NULL)
        return NULL;
    int carry = 0;
    for (int i = 0; i < n; ++i) {
        int s = carry;
        if (i < a->len) s += a->dig[i];
        if (i < b->len) s += b->dig[i];
        r->dig[i] = (unsigned char)(s % 10);
        carry = s / 10;
    }
    r->len = n;
    r->sign = 1;
    bi_norm(r);
    return r;
}

/* 前提：|a| >= |b| */
static BigInt *bi_sub_mag(const BigInt *a, const BigInt *b)
{
    BigInt *r = bi_alloc(a->len);
    if (r == NULL)
        return NULL;
    int borrow = 0;
    for (int i = 0; i < a->len; ++i) {
        int s = a->dig[i] - borrow - (i < b->len ? b->dig[i] : 0);
        if (s < 0) {
            s += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        r->dig[i] = (unsigned char)s;
    }
    r->len = a->len;
    r->sign = 1;
    bi_norm(r);
    return r;
}

BigInt *big_from_str(const char *s)
{
    if (s == NULL)
        return NULL;
    int sign = 1;
    if (*s == '-') { sign = -1; ++s; }
    else if (*s == '+') { ++s; }
    size_t n = strlen(s);
    if (n == 0)
        return NULL;
    for (size_t i = 0; i < n; ++i)
        if (s[i] < '0' || s[i] > '9')
            return NULL;

    BigInt *b = bi_alloc((int)n);
    if (b == NULL)
        return NULL;
    for (size_t i = 0; i < n; ++i)
        b->dig[i] = (unsigned char)(s[n - 1 - i] - '0');   /* 反序：小端 */
    b->len = (int)n;
    b->sign = (sign < 0) ? -1 : 1;
    bi_norm(b);
    return b;
}

BigInt *big_from_ll(long long v)
{
    char buf[32];
    snprintf(buf, sizeof buf, "%lld", v);
    return big_from_str(buf);
}

void big_dispose(BigInt *b)
{
    if (b == NULL)
        return;
    free(b->dig);
    free(b);
}

char *big_to_str(const BigInt *b)
{
    if (b == NULL)
        return NULL;
    int neg = (b->sign < 0);
    char *s = malloc((size_t)b->len + (size_t)neg + 1);
    if (s == NULL)
        return NULL;
    int o = 0;
    if (neg)
        s[o++] = '-';
    for (int i = b->len - 1; i >= 0; --i)
        s[o++] = (char)('0' + b->dig[i]);
    s[o] = '\0';
    return s;
}

int big_cmp(const BigInt *a, const BigInt *b)
{
    if (a->sign != b->sign)
        return a->sign < b->sign ? -1 : 1;
    if (a->sign == 0)
        return 0;
    int c = bi_cmp_mag(a, b);
    return (a->sign > 0) ? c : -c;
}

BigInt *big_neg(const BigInt *a)
{
    BigInt *r = bi_copy(a);
    if (r != NULL)
        r->sign = -r->sign;
    return r;
}

BigInt *big_add(const BigInt *a, const BigInt *b)
{
    if (a == NULL || b == NULL)
        return NULL;
    if (a->sign == 0) return bi_copy(b);
    if (b->sign == 0) return bi_copy(a);

    if (a->sign == b->sign) {
        BigInt *r = bi_add_mag(a, b);
        if (r != NULL)
            r->sign = a->sign;
        return r;
    }
    int c = bi_cmp_mag(a, b);
    if (c == 0)
        return bi_alloc(1);                     /* 互为相反数 → 0 */
    if (c > 0) {
        BigInt *r = bi_sub_mag(a, b);
        if (r != NULL) r->sign = a->sign;
        return r;
    }
    BigInt *r = bi_sub_mag(b, a);
    if (r != NULL) r->sign = b->sign;
    return r;
}

BigInt *big_sub(const BigInt *a, const BigInt *b)
{
    BigInt *nb = big_neg(b);
    if (nb == NULL)
        return NULL;
    BigInt *r = big_add(a, nb);
    big_dispose(nb);
    return r;
}

BigInt *big_mul(const BigInt *a, const BigInt *b)
{
    if (a == NULL || b == NULL)
        return NULL;
    if (a->sign == 0 || b->sign == 0)
        return bi_alloc(1);

    BigInt *r = bi_alloc(a->len + b->len + 1);
    if (r == NULL)
        return NULL;

    for (int i = 0; i < a->len; ++i) {
        if (a->dig[i] == 0)
            continue;
        int carry = 0;
        for (int j = 0; j < b->len; ++j) {
            int t = r->dig[i + j] + (int)a->dig[i] * (int)b->dig[j] + carry;
            r->dig[i + j] = (unsigned char)(t % 10);
            carry = t / 10;
        }
        int k = i + b->len;
        while (carry > 0) {
            int t = r->dig[k] + carry;
            r->dig[k] = (unsigned char)(t % 10);
            carry = t / 10;
            ++k;
        }
    }
    r->len = r->cap;
    r->sign = a->sign * b->sign;
    bi_norm(r);
    return r;
}
