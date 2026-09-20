/* Chapter 3: a polynomial ADT with an array representation.
 * coeff[i] multiplies x^i, so Add is a coefficient-wise scan and Multiply
 * is a nested loop -- exactly the O(N*M) bound the exercise asks about.
 */
#include <stdlib.h>
#include "ch03.h"

static Polynomial *poly_alloc(int degree)
{
    Polynomial *p = malloc(sizeof *p);
    if (p == NULL)
        return NULL;
    p->coeff = calloc((size_t)degree + 1, sizeof *p->coeff);
    if (p->coeff == NULL) {
        free(p);
        return NULL;
    }
    p->degree = degree;
    return p;
}

/* trim leading zero coefficients so that degree is meaningful */
static Polynomial *poly_trim(Polynomial *p)
{
    while (p->degree > 0 && p->coeff[p->degree] == 0.0)
        --p->degree;
    return p;
}

Polynomial *poly_create(const double coeff[], int degree)
{
    if (coeff == NULL || degree < 0)
        return NULL;
    Polynomial *p = poly_alloc(degree);
    if (p == NULL)
        return NULL;
    for (int i = 0; i <= degree; ++i)
        p->coeff[i] = coeff[i];
    return poly_trim(p);
}

void poly_dispose(Polynomial *p)
{
    if (p == NULL) return;
    free(p->coeff);
    free(p);
}

Polynomial *poly_add(const Polynomial *a, const Polynomial *b)
{
    if (a == NULL || b == NULL)
        return NULL;
    int deg = a->degree > b->degree ? a->degree : b->degree;
    Polynomial *r = poly_alloc(deg);
    if (r == NULL)
        return NULL;
    for (int i = 0; i <= a->degree; ++i)
        r->coeff[i] += a->coeff[i];
    for (int i = 0; i <= b->degree; ++i)
        r->coeff[i] += b->coeff[i];
    return poly_trim(r);
}

Polynomial *poly_mul(const Polynomial *a, const Polynomial *b)
{
    if (a == NULL || b == NULL)
        return NULL;
    Polynomial *r = poly_alloc(a->degree + b->degree);
    if (r == NULL)
        return NULL;
    for (int i = 0; i <= a->degree; ++i)
        for (int j = 0; j <= b->degree; ++j)
            r->coeff[i + j] += a->coeff[i] * b->coeff[j];
    return poly_trim(r);
}

double poly_eval(const Polynomial *p, double x)
{
    if (p == NULL)
        return 0.0;
    double result = 0.0;
    for (int i = p->degree; i >= 0; --i)        /* Horner */
        result = result * x + p->coeff[i];
    return result;
}

void poly_print(const Polynomial *p, FILE *fp)
{
    if (p == NULL) {
        fprintf(fp, "0");
        return;
    }
    int printed = 0;
    for (int i = p->degree; i >= 0; --i) {
        double c = p->coeff[i];
        if (c == 0.0)
            continue;
        if (!printed)
            fprintf(fp, "%s", c < 0 ? "-" : "");
        else
            fprintf(fp, "%s", c < 0 ? " - " : " + ");
        double a = c < 0 ? -c : c;
        if (i == 0) {
            fprintf(fp, "%.6g", a);
        } else if (i == 1) {
            if (a == 1.0)
                fprintf(fp, "x");
            else
                fprintf(fp, "%.6gx", a);
        } else {
            if (a == 1.0)
                fprintf(fp, "x^%d", i);
            else
                fprintf(fp, "%.6gx^%d", a, i);
        }
        printed = 1;
    }
    if (!printed)
        fprintf(fp, "0");
}
