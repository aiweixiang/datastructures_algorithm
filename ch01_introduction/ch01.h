/* 第 1 章 引论：本章习题的公共接口。 */
#ifndef CH01_H
#define CH01_H

#include <stdio.h>
#include <stddef.h>

/* ================= 1.3 只用 PrintDigit 输出任意实数 ================= */
/* print_digit 是唯一的"输出原语"，只能输出一个 0..9 的数字字符。 */
void print_digit(FILE *fp, int d);
/* 用递归输出 n 在给定进制下的表示（先高位后低位），进制 2..10 */
void print_base_to(FILE *fp, unsigned long long n, int base);
/* 只用上面两个原语输出实数 x 的 decimals 位小数 */
void print_real_to(FILE *fp, double x, int decimals);
void print_real(double x, int decimals);            /* 写到 stdout */

/* ================= 1.4 嵌套 #include 展开 ================= */
/* 把 root 文件里的 #include "..." 递归展开后写到 out。
 * 返回值：展开出的行数；失败返回 -1，原因写入 err（可能为空串）。
 * max_depth <= 0 时按 1 处理。会检测循环包含。 */
long include_expand(const char *root, FILE *out, int max_depth,
                    char *err, size_t errsize);

/* ================= 1.2 字谜游戏（网格找词） ================= */
#define WP_DIRS 8              /* 8 个方向：横、竖、两条对角线，各含正反 */

typedef struct {
    int rows, cols;
    const char *const *grid;      /* grid[r][c] 为字符；每行长度 >= cols */
} WpGrid;

typedef struct {
    int row, col;                 /* 单词首字母的位置 */
    int dr, dc;                   /* 方向向量，取值 -1/0/1 */
    int word_index;               /* 对应 words[] 的下标 */
} WpMatch;

/* 在网格中找出 words 的所有出现，8 个方向（任意方向都算命中）。
 * 返回命中个数；matches 容量不足返回 -1。
 * 长度为 1 的单词只按一个方向计一次（否则同一格会被记 8 次）。 */
int word_puzzle(const WpGrid *g, const char *const words[], int nwords,
                WpMatch matches[], int max_matches);

#endif /* CH01_H */
