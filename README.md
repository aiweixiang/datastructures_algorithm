# 数据结构与算法分析（C 语言描述 · Weiss 第 2 版）课后习题解答

本仓库是《Data Structures and Algorithm Analysis in C》(Second Edition, Mark Allen Weiss)
课后习题的**个人原创解答**：能写成程序的题目全部给出可编译、可测试的 C 代码，理论题给出推导过程。

> 说明：仓库内**不转载**教材原文，习题一律用「编号 + 主题」引用，解答为本仓库作者独立完成。
> 正式的 Instructor's Solutions Manual 未被参考或复制。

## 目录结构

```
.
├── include/test_util.h                 零依赖断言测试框架
├── ch01_introduction/                  第 1 章 引论
│   └── ANSWERS.md
├── ch02_algorithm_analysis/            第 2 章 算法分析
│   ├── ch02.h                          本章全部接口
│   ├── ANSWERS.md                      书面题解答
│   ├── max_subsequence_sum.c           最大子序列和的 4 种算法
│   ├── binary_search.c                 二分查找 / lower_bound / upper_bound
│   ├── gcd.c                           Euclid、扩展 Euclid、lcm
│   ├── power.c                         幂运算（O(N) 与 O(log N)、模幂）
│   ├── log2_floor.c                    ⌊log2 N⌋、⌈log2 N⌉、2 的幂判定
│   ├── horner.c                        Horner 法则
│   ├── fibonacci.c                     递归 / 迭代 / 记忆化 / 矩阵快速幂
│   └── kth_largest.c                   选择问题（排序、堆、快速选择）
├── ch03_lists_stacks_queues/           第 3 章 表、栈和队列
│   ├── ch03.h                          本章全部接口与 ADT 定义
│   ├── ANSWERS.md                      书面题解答 + 复杂度对照表
│   ├── list_array.c                    顺序表
│   ├── list_linked.c                   带头结点的单链表（含 Floyd 判环/断环）
│   ├── list_cursor.c                   游标（数组模拟链表）实现
│   ├── list_doubly.c                   带哨兵的双链表
│   ├── list_ops.c                      有序表求交/求并
│   ├── stack_array.c                   动态数组栈（倍增扩容）
│   ├── stack_linked.c                  链式栈
│   ├── queue_circular.c                循环数组队列（倍增扩容）
│   ├── two_stacks.c                    习题 3.24：一个数组实现两个栈
│   ├── balanced_symbols.c              括号匹配
│   ├── infix_to_postfix.c              中缀→后缀、后缀→中缀（3.23）
│   ├── postfix_eval.c                  后缀表达式求值（3.22）
│   ├── josephus.c                      Josephus 问题（模拟 + 递推）
│   └── polynomial.c                    多项式 ADT
├── tests/                              每个章一个测试可执行文件
├── demos/                              最早手写的 demo（保留原始版本）
└── Makefile
```

## 编译与测试

```bash
make test          # 编译并运行全部测试
make demos         # 编译 demos/
make clean
```

环境：任意 C11 编译器（gcc/clang）+ make。当前测试状态：

| 测试 | 断言数 | 结果 |
|------|--------|------|
| `tests/test_ch02.c` | 52037 | ✅ 全部通过 |
| `tests/test_ch03.c` | 713 | ✅ 全部通过 |

编译参数为 `-std=c11 -Wall -Wextra`，**零警告**。

## 完成进度

| 章 | 主题 | 状态 |
|----|------|------|
| 1 | Introduction 引论 | ✅ 书面解答 |
| 2 | Algorithm Analysis 算法分析 | ✅ 书面解答 + 8 个代码模块 + 测试 |
| 3 | Lists, Stacks, and Queues | ✅ 书面解答 + 14 个代码模块 + 测试 |
| 4 | Trees 树 | ⏳ 待完成 |
| 5 | Hashing 散列 | ⏳ 待完成 |
| 6 | Priority Queues (Heaps) 优先队列 | ⏳ 待完成 |
| 7 | Sorting 排序 | ⏳ 待完成 |
| 8 | The Disjoint Set ADT 不相交集 | ⏳ 待完成 |
| 9 | Graph Algorithms 图论算法 | ⏳ 待完成 |
| 10 | Algorithm Design Techniques 算法设计技巧 | ⏳ 待完成 |
| 11 | Amortized Analysis 摊还分析 | ⏳ 待完成 |
| 12 | Advanced Data Structures 高级数据结构 | ⏳ 待完成 |

## 约定

- 链表/表的下标在本仓库中统一为 **0 起始**；教材中「第 k 个」的查询保持 **1 起始**（与题目措辞一致）。
- 查找失败统一返回 `ELEMENT_NOT_FOUND`（值为 `-1`）。
- 每个 `.c` 文件顶部注释写明该实现对应的习题编号与复杂度。
- 教材不同印次的习题编号可能有个别出入；`ANSWERS.md` 中按「编号 + 主题」双标注，编号不一致时以主题为准。
