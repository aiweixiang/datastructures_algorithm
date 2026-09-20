# 数据结构与算法分析（C 语言描述 · Weiss）课后习题解答

《Data Structures and Algorithm Analysis in C》(Mark Allen Weiss) 课后习题的**原创解答**：
能写成程序的题目全部给出可编译、可测试的 C 代码，理论题给出完整推导。

> 仓库内**不转载教材原文**，习题一律用「编号 + 主题」引用，解答为作者独立完成。
> 官方 Instructor's Solutions Manual 未被参考或复制。习题编号映射见 `NUMBERING.md`。

## 目录结构

```
.
├── NUMBERING.md                        习题编号对照表（你的版本 ↔ 英文 2e）
├── NOTES.md                            范围与版权说明
├── include/test_util.h                 零依赖断言测试框架
├── ch01_introduction/                  第 1 章 引论（1.1 ~ 1.10 全部完成）
│   ├── ANSWERS.md
│   ├── print_real.c                    1.3  只用 PrintDigit 输出实数/任意进制
│   ├── include_expand.c                1.4  嵌套 #include 展开（含循环检测）
│   └── word_puzzle.c                   1.2  字谜游戏（8 方向找词）
├── ch02_algorithm_analysis/            第 2 章 算法分析（2.1 ~ 2.25 全部完成）
│   ├── ANSWERS.md                      含 2.1~2.6、2.18、2.20~2.22、2.24 的书面推导
│   ├── max_subsequence_sum.c           最大子序列和 4 种算法（含区间）
│   ├── subsequence_variants.c          2.12 最小/最小正/最大乘积 + 2.25 迭代次数
│   ├── binary_search.c                 二分查找、lower/upper bound、2.23 单比较版
│   ├── fixed_point.c                   2.11 A[i] == i
│   ├── primes.c                        2.13 素数判定 / 2.14 筛法
│   ├── majority.c                      2.19 Boyer–Moore 投票
│   ├── pow_chain.c                     2.15~2.17 快速幂乘次数、x^62 八次乘法
│   ├── power.c  log2_floor.c           幂运算 / 对数
│   ├── gcd.c    horner.c               欧几里得 / Horner 法则
│   ├── fibonacci.c  kth_largest.c      Fibonacci / 选择问题
│   ├── permutations.c  rng.c           2.7 随机置换（+ 可复现 LCG）
│   └── ch02.h
├── ch03_lists_stacks_queues/           第 3 章 表、栈和队列（26/26 完成）
│   ├── ANSWERS.md                      含复杂度对照表与各题思路
│   ├── list_array.c  list_linked.c     顺序表 / 带头结点单链表（含判环、断环）
│   ├── list_cursor.c  list_doubly.c    游标表 / 带哨兵双链表
│   ├── list_ops.c  polynomial.c        有序表求交并 / 多项式 ADT
│   ├── stack_array.c  stack_linked.c   两种栈
│   ├── queue_circular.c                循环数组队列
│   ├── two_stacks.c  min_stack.c       一数组两栈 / 带 FindMin 的栈
│   ├── balanced_symbols.c              括号匹配
│   ├── infix_to_postfix.c              中缀→后缀、后缀→中缀
│   ├── postfix_eval.c                  后缀表达式求值
│   ├── print_lots.c                    3.2  按位置列表打印链表元素
│   ├── bignum.c                        3.9  任意精度整数（加/减/乘/比较）
│   ├── radix_records.c                 3.13 基数排序学生记录（LSD，稳定）
│   ├── adjacency.c                     3.14 图→邻接表（链表版 + 游标版）
│   ├── array_dedup.c                   3.16 数组去重（保序 O(N²) / 排序 O(N log N)）
│   ├── lazy_list.c                     3.17 懒惰删除（含摊还分析）
│   ├── three_stacks.c                  3.23 一个数组实现三个栈
│   ├── recursion_depth.c               3.24 斐波那契递归的时间/空间分离
│   ├── deque.c                         3.26 双端队列（两端 O(1)）
│   └── ch03.h
├── tests/                              每章一个测试 + fixtures/
├── demos/                              kth_Largest.c（手写原版）、selection_timing.c（1.1 计时）
└── Makefile
```

## 编译与测试

```bash
make test      # 编译并运行全部测试
make demos     # 编译 demos/（含 1.1 的计时表）
make clean
```

当前状态（`-std=c11 -Wall -Wextra`，**零警告**）：

| 测试 | 断言数 | 结果 |
|------|--------|------|
| `tests/test_ch01.c` | 595 | ✅ |
| `tests/test_ch02.c` | 63965 | ✅ |
| `tests/test_ch03.c` | 421092 | ✅ |
| **合计** | **485652** | ✅ 全部通过 |

另外三个测试都在 `-fsanitize=address,undefined` 下通过（无越界、无未定义行为）——
上面这个"游标邻接表 head 初值"和"懒惰删除模型数组越界"两个 bug 就是靠它抓出来的。

`make demos && ./build/selection_timing` 会打印 1.1 要的运行时间表（本机实测）：

```
        N     sort(ms)     heap(ms)   select(ms)
    10000        0.646        0.227        0.047   (三法结果一致: yes)
    40000        2.954        1.050        0.274   (三法结果一致: yes)
   160000       13.329        5.013        1.297   (三法结果一致: yes)
   640000       67.062       18.369        5.100   (三法结果一致: yes)
```

## 完成进度

| 章 | 主题 | 状态 |
|----|------|------|
| 1 | 引论（1.1 ~ 1.10） | ✅ 完成（4 个代码模块 + 6 题的证明/推导） |
| 2 | 算法分析（2.1 ~ 2.25） | ✅ 完成（12 个代码模块 + 10 题的书面推导） |
| 3 | 表、栈和队列（3.1 ~ 3.26） | ✅ 完成（26/26：21 个代码模块 + 复杂度分析题） |
| 4 | 树（4.1 ~ 4.46） | ⏳ 下一轮（BST/AVL/伸展树/B 树/证明题 46 项） |
| 5 | 散列 | ⏳ |
| 6 | 优先队列（堆） | ⏳ |
| 7 | 排序 | ⏳ |
| 8 | 不相交集 | ⏳ |
| 9 | 图论算法 | ⏳ |
| 10 | 算法设计技巧 | ⏳ |
| 11 | 摊还分析 | ⏳ |
| 12 | 高级数据结构 | ⏳ |

## 约定

- 链表/表的下标统一为 **0 起始**；教材中「第 k 个」的查询保持 **1 起始**（与题目措辞一致）。
  涉及 `A[i] == i`（2.11）这类题时两种约定都实现，避免下标歧义。
- 查找失败统一返回 `ELEMENT_NOT_FOUND`（`-1`）。
- 每个 `.c` 文件顶部注释写明对应习题、主题与复杂度。
- **习题编号以主题为准**：不同版本编号会整体移位（例如「带 FindMin 的栈」在英文 2e 是 3.25、
  在你的版本是 3.22），映射见 `NUMBERING.md`。
- 随机性测试使用自带的**确定性 LCG**，失败可精确复现；关键算法另与独立暴力参考实现对拍。
- 内存安全用 `-fsanitize=address,undefined` 复核：`build/test_ch0*.san`（见本文"编译与测试"）。
