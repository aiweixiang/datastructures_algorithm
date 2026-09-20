/* 习题 1.4：实现支持嵌套 #include 的文件展开程序（玩具预处理器）。
 *
 * 规则：
 *   1. 识别形如   #include "path"   的整行（允许前后有空白）；
 *   2. 相对路径相对于"包含它的那个文件"所在目录解析；
 *   3. 用"包含栈"检测循环包含（A 包含 B、B 又包含 A）；
 *   4. 用 max_depth 限制嵌套深度，防止恶意/意外的深递归。
 * 与真正的 C 预处理器不同：不处理宏、条件编译、#include <...>、
 * 头文件保护等（那些属于课外的扩展）。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ch01.h"

#define IE_PATH_MAX 512
#define IE_LINE_MAX 1024
#define IE_MAX_DEPTH 64

typedef struct {
    char paths[IE_MAX_DEPTH][IE_PATH_MAX];
    int  n;                 /* 当前栈深 */
    int  max_depth;
} IeStack;

static void ie_dirname(const char *path, char *out, size_t outsize)
{
    const char *slash = strrchr(path, '/');
    if (slash == NULL) {
        out[0] = '\0';
        return;
    }
    size_t len = (size_t)(slash - path) + 1;      /* 保留结尾的 '/' */
    if (len >= outsize)
        len = outsize - 1;
    memcpy(out, path, len);
    out[len] = '\0';
}

/* 若 line 是 #include "..." 则把路径写入 out 并返回 1 */
static int ie_parse_include(const char *line, char *out, size_t outsize)
{
    static const char kw[] = "include";
    const char *p = line;

    while (*p == ' ' || *p == '\t')
        ++p;
    if (*p != '#')
        return 0;
    ++p;
    while (*p == ' ' || *p == '\t')
        ++p;
    for (size_t k = 0; kw[k] != '\0'; ++k) {
        if (p[k] != kw[k])
            return 0;
    }
    p += sizeof kw - 1;
    while (*p == ' ' || *p == '\t')
        ++p;
    if (*p != '"')
        return 0;
    ++p;
    const char *q = strchr(p, '"');
    if (q == NULL || q == p)
        return 0;
    size_t len = (size_t)(q - p);
    if (len >= outsize)
        return 0;
    memcpy(out, p, len);
    out[len] = '\0';
    return 1;
}

static long ie_expand(const char *path, FILE *out, IeStack *st,
                      char *err, size_t errsize)
{
    if (st->n >= st->max_depth) {
        snprintf(err, errsize, "include depth limit (%d) exceeded at %s",
                 st->max_depth, path);
        return -1;
    }
    for (int i = 0; i < st->n; ++i) {
        if (strcmp(st->paths[i], path) == 0) {
            snprintf(err, errsize, "circular include detected: %s", path);
            return -1;
        }
    }

    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        snprintf(err, errsize, "cannot open include file: %s", path);
        return -1;
    }

    snprintf(st->paths[st->n], IE_PATH_MAX, "%s", path);
    ++st->n;

    char dir[IE_PATH_MAX];
    ie_dirname(path, dir, sizeof dir);

    char  line[IE_LINE_MAX];
    char  inc[IE_PATH_MAX];
    long  lines = 0;
    int   failed = 0;

    while (fgets(line, sizeof line, fp) != NULL) {
        if (ie_parse_include(line, inc, sizeof inc)) {
            char sub[2 * IE_PATH_MAX];
            int need = snprintf(sub, sizeof sub, "%s%s", dir, inc);
            if (need < 0 || (size_t)need >= sizeof sub) {
                snprintf(err, errsize, "include path too long: %s%s", dir, inc);
                failed = 1;
                break;
            }
            long r = ie_expand(sub, out, st, err, errsize);
            if (r < 0) {
                failed = 1;
                break;
            }
            lines += r;                    /* 被包含文件的行数计入总数 */
        } else {
            fputs(line, out);
            ++lines;
        }
    }

    fclose(fp);
    --st->n;
    return failed ? -1 : lines;
}

long include_expand(const char *root, FILE *out, int max_depth,
                    char *err, size_t errsize)
{
    IeStack st;
    st.n = 0;
    st.max_depth = (max_depth <= 0) ? 1 : max_depth;
    if (st.max_depth > IE_MAX_DEPTH)
        st.max_depth = IE_MAX_DEPTH;
    if (err != NULL && errsize > 0)
        err[0] = '\0';
    return ie_expand(root, out, &st, err, errsize);
}
