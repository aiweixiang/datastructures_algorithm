/* Exercises 3.23 a/b/c: infix -> postfix, and postfix -> infix.
 * The conversion sweeps left to right: operands are emitted immediately,
 * operators wait on a stack until an operator of lower precedence (or a
 * ')') forces them out.  '^' is right-associative.
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ch03.h"

static int prec(char op)
{
    switch (op) {
    case '+': case '-': return 1;
    case '*': case '/': return 2;
    case '^': return 3;
    default:  return -1;
    }
}

char *infix_to_postfix(const char *expr, int allow_exp)
{
    if (expr == NULL)
        return NULL;
    size_t n = strlen(expr);

    char *out = malloc(3 * n + 8);
    char *stack = malloc(n + 1);
    if (out == NULL || stack == NULL) {
        free(out);
        free(stack);
        return NULL;
    }

    size_t o = 0, sp = 0;
    for (size_t i = 0; i < n; ) {
        char c = expr[i];

        if (isspace((unsigned char)c)) {
            ++i;
        } else if (isdigit((unsigned char)c) || c == '.') {
            while (i < n && (isdigit((unsigned char)expr[i]) || expr[i] == '.'))
                out[o++] = expr[i++];
            out[o++] = ' ';
        } else if (c == '(') {
            stack[sp++] = c;
            ++i;
        } else if (c == ')') {
            while (sp > 0 && stack[sp - 1] != '(') {
                out[o++] = stack[--sp];
                out[o++] = ' ';
            }
            if (sp == 0) { free(out); free(stack); return NULL; }   /* unbalanced */
            --sp;                                                   /* drop '(' */
            ++i;
        } else if (prec(c) > 0 && (c != '^' || allow_exp)) {
            while (sp > 0 && stack[sp - 1] != '(' &&
                   (prec(stack[sp - 1]) > prec(c) ||
                    (prec(stack[sp - 1]) == prec(c) && c != '^'))) {
                out[o++] = stack[--sp];
                out[o++] = ' ';
            }
            stack[sp++] = c;
            ++i;
        } else {
            free(out); free(stack); return NULL;                    /* bad char */
        }
    }

    while (sp > 0) {
        if (stack[sp - 1] == '(') { free(out); free(stack); return NULL; }
        out[o++] = stack[--sp];
        out[o++] = ' ';
    }
    if (o > 0 && out[o - 1] == ' ')
        --o;
    out[o] = '\0';
    free(stack);
    return out;
}

/* --- postfix -> infix: rebuild fully parenthesised infix text --- */
static char *str_wrap2(const char *a, char op, const char *b)
{
    size_t la = strlen(a), lb = strlen(b);
    char *r = malloc(la + lb + 6);          /* "(a op b)" + NUL */
    if (r == NULL)
        return NULL;
    sprintf(r, "(%s %c %s)", a, op, b);
    return r;
}

char *postfix_to_infix(const char *expr)
{
    if (expr == NULL)
        return NULL;
    size_t n = strlen(expr);
    char **stack = malloc((n + 1) * sizeof *stack);
    if (stack == NULL)
        return NULL;
    size_t sp = 0;

    for (size_t i = 0; i < n; ) {
        char c = expr[i];
        if (isspace((unsigned char)c)) {
            ++i;
        } else if (isdigit((unsigned char)c) || c == '.') {
            size_t start = i;
            while (i < n && (isdigit((unsigned char)expr[i]) || expr[i] == '.'))
                ++i;
            char *tok = malloc(i - start + 1);
            if (tok == NULL) goto fail;
            memcpy(tok, expr + start, i - start);
            tok[i - start] = '\0';
            stack[sp++] = tok;
        } else if (strchr("+-*/^", c) != NULL) {
            if (sp < 2) goto fail;
            char *b = stack[--sp];
            char *a = stack[--sp];
            char *merged = str_wrap2(a, c, b);
            free(a);
            free(b);
            if (merged == NULL) goto fail;
            stack[sp++] = merged;
            ++i;
        } else {
            goto fail;
        }
    }

    if (sp != 1) goto fail;
    {
        char *result = stack[0];
        free(stack);
        return result;
    }

fail:
    while (sp > 0)
        free(stack[--sp]);
    free(stack);
    return NULL;
}
