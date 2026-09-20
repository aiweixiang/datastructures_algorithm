/* 第 1 章测试：1.3（只用 PrintDigit 输出实数/任意进制）、1.4（嵌套 #include 展开）、
 * 1.2（字谜游戏），以及 1.8/1.9/1.10 中等式与级数的数值验证。 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ch01_introduction/ch01.h"
#include "test_util.h"

#ifndef FIXTURE_DIR
#define FIXTURE_DIR "tests/fixtures"
#endif

/* ============ 1.3 ============ */
static char captured[512];

static const char *capture_real(double x, int decimals)
{
    FILE *f = tmpfile();
    if (f == NULL)
        return NULL;
    print_real_to(f, x, decimals);
    fflush(f);
    rewind(f);
    size_t n = fread(captured, 1, sizeof captured - 1, f);
    captured[n] = '\0';
    fclose(f);
    return captured;
}

static const char *capture_base(unsigned long long v, int base)
{
    FILE *f = tmpfile();
    if (f == NULL)
        return NULL;
    print_base_to(f, v, base);
    fflush(f);
    rewind(f);
    size_t n = fread(captured, 1, sizeof captured - 1, f);
    captured[n] = '\0';
    fclose(f);
    return captured;
}

static void test_print_real(void)
{
    CHECK_EQ(strcmp(capture_real(123.125, 3), "123.125"), 0);
    CHECK_EQ(strcmp(capture_real(-7.25, 2), "-7.25"), 0);
    CHECK_EQ(strcmp(capture_real(0.0, 2), "0.00"), 0);
    CHECK_EQ(strcmp(capture_real(0.5, 1), "0.5"), 0);
    CHECK_EQ(strcmp(capture_real(42.0, 0), "42."), 0);   /* 0 位小数仍打印 '.' */
    CHECK_EQ(strcmp(capture_real(1.0, 4), "1.0000"), 0);

    CHECK_EQ(strcmp(capture_base(0ULL, 10), "0"), 0);
    CHECK_EQ(strcmp(capture_base(1234ULL, 10), "1234"), 0);
    CHECK_EQ(strcmp(capture_base(255ULL, 2), "11111111"), 0);
    CHECK_EQ(strcmp(capture_base(255ULL, 8), "377"), 0);
    CHECK_EQ(strcmp(capture_base(100ULL, 1), ""), 0);    /* 非法进制：不输出 */
}

/* ============ 1.4 ============ */
static void test_include_expand(void)
{
    char err[256];
    FILE *f;
    long  n;
    char  out[1024];
    size_t got;

    f = tmpfile();
    CHECK(f != NULL);
    n = include_expand(FIXTURE_DIR "/inc_a.h", f, 16, err, sizeof err);
    fflush(f);
    rewind(f);
    got = fread(out, 1, sizeof out - 1, f);
    out[got] = '\0';
    fclose(f);
    CHECK_EQ(n, 6);                                      /* a + 2 + b + c + b + a */
    CHECK_EQ(strcmp(out,
                    "/* inc_a.h */\nline_a1\nline_b1\nline_c1\nline_b2\nline_a2\n"),
             0);

    f = tmpfile();                                       /* 循环包含 */
    n = include_expand(FIXTURE_DIR "/cycle_x.h", f, 16, err, sizeof err);
    fclose(f);
    CHECK_EQ(n, -1);
    CHECK(strstr(err, "circular") != NULL);

    f = tmpfile();                                       /* 深度限制 */
    n = include_expand(FIXTURE_DIR "/inc_d1.h", f, 2, err, sizeof err);
    fclose(f);
    CHECK_EQ(n, -1);
    CHECK(strstr(err, "depth") != NULL);

    f = tmpfile();                                       /* 文件不存在 */
    n = include_expand(FIXTURE_DIR "/no_such_file.h", f, 8, err, sizeof err);
    fclose(f);
    CHECK_EQ(n, -1);
    CHECK(strstr(err, "cannot open") != NULL);
}

/* ============ 1.2 ============ */
static void test_word_puzzle(void)
{
    static const char *grid[] = {
        "abcd",
        "efgh",
        "ijkl",
        "mnop",
    };
    static const char *words[] = {"abcd", "aeim", "afkp", "dgjm", "plhd", "nope", "a"};
    WpGrid g = {4, 4, grid};
    WpMatch m[64];

    int n = word_puzzle(&g, words, 7, m, 64);
    CHECK_EQ(n, 6);      /* abcd, aeim, afkp, dgjm, plhd, a —— nope 不存在 */

    struct { int wi, r, c, dr, dc; } want[] = {
        {0, 0, 0, 0,  1},   /* abcd：横向 */
        {1, 0, 0, 1,  0},   /* aeim：纵向 */
        {2, 0, 0, 1,  1},   /* afkp：主对角线 */
        {3, 0, 3, 1, -1},   /* dgjm：反对角线 */
        {4, 3, 3, -1, 0},   /* plhd：必须反向（原串 dhlp）才命中 */
        {6, 0, 0, 0,  1},   /* 单字符 a：只记一次 */
    };
    for (size_t k = 0; k < sizeof want / sizeof want[0]; ++k) {
        int found = 0;
        for (int i = 0; i < n; ++i)
            if (m[i].word_index == want[k].wi && m[i].row == want[k].r &&
                m[i].col == want[k].c && m[i].dr == want[k].dr &&
                m[i].dc == want[k].dc)
                found = 1;
        CHECK(found);
    }
    for (int i = 0; i < n; ++i)          /* 不存在的单词不得产生命中 */
        CHECK(m[i].word_index != 5);

    WpMatch small[2];                    /* 容量不足 */
    CHECK_EQ(word_puzzle(&g, words, 7, small, 2), -1);
}

/* ============ 1.8 / 1.9 / 1.10 的数值验证 ============ */
static unsigned long long fib(int n)
{
    unsigned long long a = 0, b = 1;
    for (int i = 0; i < n; ++i) {
        unsigned long long t = a + b;
        a = b;
        b = t;
    }
    return a;
}

static void test_identities(void)
{
    /* 1.8：2^100 mod 5 = 1 */
    long long r = 1;
    for (int i = 0; i < 100; ++i)
        r = (r * 2) % 5;
    CHECK_EQ(r, 1);

    /* 1.9 (a)：Σ_{i=1..N} F_i = F_{N+2} − 1 */
    unsigned long long sum = 0;
    for (int N = 1; N <= 60; ++N) {
        sum += fib(N);
        CHECK_EQ(sum, fib(N + 2) - 1);
    }
    /* 1.9 (b)：Σ F_i² = F_N · F_{N+1}   （N ≤ 40 以免溢出） */
    unsigned long long sumsq = 0;
    for (int N = 1; N <= 40; ++N) {
        sumsq += fib(N) * fib(N);
        CHECK_EQ(sumsq, fib(N) * fib(N + 1));
    }
    /* 1.9 (e)：相邻两项互素 —— 用 gcd 验证 */
    for (int N = 1; N <= 60; ++N) {
        unsigned long long a = fib(N), b = fib(N + 1);
        while (b != 0) { unsigned long long t = a % b; a = b; b = t; }
        CHECK_EQ(a, 1);
    }

    /* 1.10：1+3+…+(2N−1) = N²，Σ i³ = (N(N+1)/2)² */
    long long odd = 0, cubes = 0;
    for (long long N = 1; N <= 200; ++N) {
        odd += 2 * N - 1;
        cubes += N * N * N;
        long long tri = N * (N + 1) / 2;
        CHECK_EQ(odd, N * N);
        CHECK_EQ(cubes, tri * tri);
    }
}

int main(void)
{
    test_print_real();
    test_include_expand();
    test_word_puzzle();
    test_identities();
    return TEST_REPORT();
}
