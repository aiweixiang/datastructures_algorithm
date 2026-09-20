/* Chapter 2: Horner's rule.  Evaluating
 *     a[degree]*x^degree + ... + a[1]*x + a[0]
 * takes O(N) multiplications instead of the O(N^2) of the naive loop, and
 * it also avoids a separate power computation for every term.
 */
#include "ch02.h"

double horner(const double coeff[], int degree, double x)
{
    double result = 0.0;
    for (int i = degree; i >= 0; --i)
        result = result * x + coeff[i];
    return result;
}

double poly_eval_naive(const double coeff[], int degree, double x)
{
    double result = 0.0;
    for (int i = 0; i <= degree; ++i) {
        double p = 1.0;
        for (int j = 0; j < i; ++j)
            p *= x;
        result += coeff[i] * p;
    }
    return result;
}
