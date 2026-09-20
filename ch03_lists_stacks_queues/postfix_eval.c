/* Exercise 3.22: evaluate a postfix expression.
 * Operands are decimal numbers, operators are + - * / ^.
 * Returns 0 on success, -1 on any malformed input.
 */
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "ch03.h"

int postfix_eval(const char *expr, double *result)
{
    if (expr == NULL)
        return -1;
    size_t n = strlen(expr);

    double *stack = malloc((n + 1) * sizeof *stack);
    if (stack == NULL)
        return -1;
    size_t sp = 0;

    for (size_t i = 0; i < n; ) {
        char c = expr[i];
        if (isspace((unsigned char)c)) {
            ++i;
        } else if (isdigit((unsigned char)c) || c == '.') {
            char *end = NULL;
            double v = strtod(expr + i, &end);
            if (end == expr + i) { free(stack); return -1; }
            stack[sp++] = v;
            i = (size_t)(end - expr);
        } else if (strchr("+-*/^", c) != NULL) {
            if (sp < 2) { free(stack); return -1; }
            double b = stack[--sp];
            double a = stack[--sp];
            double r;
            switch (c) {
            case '+': r = a + b; break;
            case '-': r = a - b; break;
            case '*': r = a * b; break;
            case '/':
                if (b == 0.0) { free(stack); return -1; }
                r = a / b;
                break;
            default:  r = pow(a, b); break;
            }
            stack[sp++] = r;
            ++i;
        } else {
            free(stack);
            return -1;
        }
    }

    if (sp != 1) {
        free(stack);
        return -1;
    }
    if (result)
        *result = stack[0];
    free(stack);
    return 0;
}
