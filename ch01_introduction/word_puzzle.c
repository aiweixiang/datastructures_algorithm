/* 习题 1.2：字谜游戏求解程序。
 *
 * 对每个单词，枚举每个起点和 8 个方向，逐字符匹配。
 *   R×C 网格、W 个单词、单词最长 L：
 *   时间 O(W · R · C · L)，空间 O(1)（除输出）。
 * 优化思路见 ANSWERS.md（按首字母分桶、或按起点生成串再查哈希/字典树）。
 */
#include <string.h>
#include "ch01.h"

static const int WP_DIR[WP_DIRS][2] = {
    { 0,  1}, { 1,  0}, { 1,  1}, { 1, -1},
    { 0, -1}, {-1,  0}, {-1, -1}, {-1,  1}
};

static int wp_match_at(const WpGrid *g, const char *w, int r, int c,
                       int dr, int dc)
{
    size_t len = strlen(w);
    for (size_t k = 0; k < len; ++k) {
        int rr = r + dr * (int)k;
        int cc = c + dc * (int)k;
        if (rr < 0 || rr >= g->rows || cc < 0 || cc >= g->cols)
            return 0;
        if (g->grid[rr][cc] != w[k])
            return 0;
    }
    return 1;
}

int word_puzzle(const WpGrid *g, const char *const words[], int nwords,
                WpMatch matches[], int max_matches)
{
    if (g == NULL || g->grid == NULL || words == NULL || matches == NULL)
        return -1;

    int n = 0;
    for (int wi = 0; wi < nwords; ++wi) {
        const char *w = words[wi];
        if (w == NULL || w[0] == '\0')
            continue;
        int ndirs = (w[1] == '\0') ? 1 : WP_DIRS;   /* 单字符只记一次 */

        for (int r = 0; r < g->rows; ++r) {
            for (int c = 0; c < g->cols; ++c) {
                for (int d = 0; d < ndirs; ++d) {
                    if (!wp_match_at(g, w, r, c, WP_DIR[d][0], WP_DIR[d][1]))
                        continue;
                    if (n == max_matches)
                        return -1;                 /* 输出容量不足 */
                    matches[n].row = r;
                    matches[n].col = c;
                    matches[n].dr = WP_DIR[d][0];
                    matches[n].dc = WP_DIR[d][1];
                    matches[n].word_index = wi;
                    ++n;
                }
            }
        }
    }
    return n;
}
