/* Minimal zero-dependency test helper for the tests/ folder. */
#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <stdio.h>

static int tests_run = 0;
static int tests_failed = 0;

#define CHECK(cond)                                                           \
    do {                                                                      \
        ++tests_run;                                                          \
        if (!(cond)) {                                                        \
            ++tests_failed;                                                   \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);   \
        }                                                                     \
    } while (0)

#define CHECK_EQ(got, want)                                                   \
    do {                                                                      \
        long long g_ = (long long)(got);                                      \
        long long w_ = (long long)(want);                                     \
        ++tests_run;                                                          \
        if (g_ != w_) {                                                       \
            ++tests_failed;                                                   \
            fprintf(stderr, "FAIL %s:%d: %s == %s (got %lld, want %lld)\n",   \
                    __FILE__, __LINE__, #got, #want, g_, w_);                 \
        }                                                                     \
    } while (0)

#define CHECK_NEAR(got, want, eps)                                            \
    do {                                                                      \
        double g_ = (double)(got);                                            \
        double w_ = (double)(want);                                           \
        double e_ = (double)(eps);                                            \
        ++tests_run;                                                          \
        if (!((g_ - w_) < e_ && (w_ - g_) < e_)) {                            \
            ++tests_failed;                                                   \
            fprintf(stderr, "FAIL %s:%d: %s ~= %s (got %f, want %f)\n",       \
                    __FILE__, __LINE__, #got, #want, g_, w_);                 \
        }                                                                     \
    } while (0)

#define TEST_REPORT()                                                         \
    (printf("%s: %d checks, %d failures\n", __FILE__, tests_run,              \
            tests_failed),                                                    \
     (tests_failed == 0) ? 0 : 1)

#endif /* TEST_UTIL_H */
