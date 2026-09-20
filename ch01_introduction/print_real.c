/* 习题 1.3：只用 PrintDigit（每次输出一个数字字符）打印任意实数。
 *
 * 关键点：递归天然把"低位先算出来"的顺序反转成"高位先输出"。
 *   print(1234) → 先递归 print(123) → 再输出 4
 * 于是不需要任何数组或字符串缓冲。
 */
#include "ch01.h"

void print_digit(FILE *fp, int d)
{
    if (d < 0)
        d = 0;
    if (d > 9)
        d = 9;
    fputc('0' + d, fp);          /* 唯一的输出原语 */
}

void print_base_to(FILE *fp, unsigned long long n, int base)
{
    if (base < 2 || base > 10)
        return;
    if (n >= (unsigned long long)base)
        print_base_to(fp, n / (unsigned long long)base, base);
    print_digit(fp, (int)(n % (unsigned long long)base));
}

void print_real_to(FILE *fp, double x, int decimals)
{
    if (x < 0) {                 /* 负号不是数字，用 fputc 直接输出 */
        fputc('-', fp);
        x = -x;
    }
    unsigned long long ip = (unsigned long long)x;
    print_base_to(fp, ip, 10);
    fputc('.', fp);

    double frac = x - (double)ip;
    for (int i = 0; i < decimals; ++i) {
        frac *= 10.0;
        int d = (int)frac;
        if (d < 0)  d = 0;
        if (d > 9)  d = 9;
        print_digit(fp, d);
        frac -= d;
    }
    /* 说明：这里是截断而不是四舍五入。若要做四舍五入，
     * 先在 x 上加 5 * 10^-(decimals+1) 再走同样的流程即可。 */
}

void print_real(double x, int decimals)
{
    print_real_to(stdout, x, decimals);
}
