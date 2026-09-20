/* 可复现的线性同余伪随机数生成器（LCG）。
 * 教材用 rand()/random()，这里自带一个是为了让测试可复现：
 * 同一个种子永远给出同一串数，失败可以精确重放。
 * 注意取高位（>> 16）而不是低位：LCG 的低位周期极短，
 * 直接用低位取模会暴露明显规律（这也是习题中"随机数质量"话题的引子）。
 */
#include "ch02.h"

void rng_seed(unsigned long *state, unsigned long seed)
{
    *state = (seed != 0UL) ? seed : 1UL;
}

unsigned long rng_next(unsigned long *state)
{
    *state = (*state * 1103515245UL + 12345UL) & 0x7fffffffUL;
    return *state;
}

int rng_uniform(unsigned long *state, int n)
{
    if (n <= 1)
        return 0;
    return (int)((rng_next(state) >> 16) % (unsigned long)n);
}
