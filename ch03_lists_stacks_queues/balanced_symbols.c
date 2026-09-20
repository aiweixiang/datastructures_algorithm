/* Chapter 3: balanced-symbol checker (( ), [ ], { }), a classic stack use.
 * Every closing symbol must match the most recent unmatched opening symbol.
 */
#include <stdlib.h>
#include <string.h>
#include "ch03.h"

int balanced_symbols(const char *s)
{
    if (s == NULL)
        return 1;
    size_t n = strlen(s);
    char *stack = malloc(n + 1);
    if (stack == NULL)
        return 0;
    size_t top = 0;

    for (size_t i = 0; i < n; ++i) {
        char c = s[i];
        if (c == '(' || c == '[' || c == '{') {
            stack[top++] = c;
        } else if (c == ')' || c == ']' || c == '}') {
            char want = (c == ')') ? '(' : (c == ']' ? '[' : '{');
            if (top == 0 || stack[top - 1] != want) {
                free(stack);
                return 0;
            }
            --top;
        }
        /* any other character is ignored */
    }
    int ok = (top == 0);
    free(stack);
    return ok;
}
