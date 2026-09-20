# 习题编号对照表（按你的版本核对）

教材不同印次/译本的编号会整体移位。本仓库的规则是：

> **正文标题以「主题」为主，编号只作注记；代码文件名一律用主题名。**
> 下表以**你的版本编号**为准（已由你提供列表核对）。

## 第 1 章（1.1 ~ 1.10，与英文第 2 版编号一致）

| 你的编号 | 主题 | 实现 / 解答 |
|----------|------|-------------|
| 1.1 | 选择问题程序 + 运行时间表 | `ch02_algorithm_analysis/kth_largest.c`、`demos/selection_timing.c` |
| 1.2 | 字谜游戏求解 | `ch01_introduction/word_puzzle.c` |
| 1.3 | 只用 PrintDigit 输出任意实数 | `ch01_introduction/print_real.c` |
| 1.4 | 嵌套 #include 展开 | `ch01_introduction/include_expand.c` + `tests/fixtures/` |
| 1.5 | 对数不等式与恒等式 | `ch01_introduction/ANSWERS.md` |
| 1.6 | 无穷级数求和 | 同上 |
| 1.7 | 调和级数部分和估计 | 同上 |
| 1.8 | 2^100 mod 5 | `ch02_algorithm_analysis/power.c`（`pow_mod`） |
| 1.9 | 斐波那契性质证明 | `ch01_introduction/ANSWERS.md` + 数值验证 |
| 1.10 | 奇数和与立方和公式 | 同上 |

## 第 2 章（2.1 ~ 2.25）

| 你的编号 | 主题 | 实现 / 解答 |
|----------|------|-------------|
| 2.1 | 函数增长率排序 | `ch02_algorithm_analysis/ANSWERS.md` |
| 2.2 / 2.3 | 大 O 记号判定、增长速度比较 | 同上 |
| 2.4 | 证明 log^k N = o(N) | 同上 |
| 2.5 | 构造互不为大 O 的函数对 | 同上 |
| 2.6 | 分析 6 段代码 | 同上（模式表） |
| 2.7 | 三种随机置换算法 | `permutations.c`、`rng.c`（**B 不均匀**，已精确枚举证明） |
| 2.8 | 估算最大子序列和运行时间 | `max_subsequence_sum.c` + 对照表 |
| 2.9 / 2.10 | 多项式求值、Horner 法则 | `horner.c` |
| 2.11 | 判断 A[i] == i | `fixed_point.c` |
| 2.12 | 最小子序列和 / 最小正和 / 最大乘积 | `subsequence_variants.c` |
| 2.13 / 2.14 | 素数判定、Eratosthenes 筛法 | `primes.c` |
| 2.15 ~ 2.17 | X^62 八次乘法、非递归快速幂、乘法次数 | `pow_chain.c`、`power.c` |
| 2.18 | 最坏/实际运行时间比较 | ANSWERS.md |
| 2.19 | 主要元素 | `majority.c` |
| 2.20 / 2.21 | 固定字长假设、字谜 + 二分优化 | ANSWERS.md |
| 2.22 / 2.23 | 二分查找边界、单比较二分 | `binary_search.c` |
| 2.24 | 分治最大子序列和的修改 | `max_subsequence_sum.c` + ANSWERS.md |
| 2.25 | 立方 vs 平方迭代次数 | `subsequence_variants.c` |

## 第 3 章（3.1 ~ 3.26）—— 已确认的编号偏移

**已知规律：英文第 2 版 3.22 ~ 3.25 在你这一版是 3.19 ~ 3.22（整体前移 3 号）。**

| 你的编号 | 主题 | 英文 2e | 实现 / 状态 |
|----------|------|---------|-------------|
| 3.1 | 打印单链表所有元素 | 3.1 | `list_linked.c`（`lil_print`）✅ |
| 3.2 | `PrintLots` 按位置打印链表元素 | 3.2 | `print_lots.c` ✅ |
| 3.3 | 只调指针交换相邻节点 | 3.3 | `list_linked.c`（`lil_swap_adjacent`）✅ |
| 3.4 / 3.5 | 有序表交集 / 并集 | 3.4 / 3.5 | `list_ops.c` ✅ |
| 3.6 / 3.7 / 3.8 | 多项式加、乘、幂 | 3.6 / 3.7 / 3.8 | `polynomial.c` ✅ |
| 3.9 | 任意精度整数包 | 3.9 | `bignum.c` ✅ |
| 3.10 | Josephus 问题 | 3.10 | `josephus.c` ✅ |
| 3.11 | 递归/非递归查找链表元素 | 3.11 | `list_linked.c` ✅ |
| 3.12 | 反转单链表（常数空间） | 3.12 | `list_linked.c`（`lil_reverse`）✅ |
| 3.13 | 基数排序学生记录 | 3.13 | `radix_records.c` ✅ |
| 3.14 | 图读入邻接表（链表/游标） | 3.14 | `adjacency.c` ✅ |
| 3.15 | 自调整表 + 性质证明 | 3.15 | `list_array.c`（move-to-front / transpose）✅ |
| 3.16 | 删除数组中的重复元素 | 3.16 | `array_dedup.c` ✅ |
| 3.17 | 懒惰删除及优缺点 | 3.17 | `lazy_list.c` ✅ |
| 3.18 | 检测平衡符号 | 3.21 | `balanced_symbols.c` ✅ |
| 3.19 | 计算后缀表达式的值 | 3.22 | `postfix_eval.c` ✅ |
| 3.20 | 中缀转后缀、后缀转中缀 | 3.23 | `infix_to_postfix.c` ✅ |
| 3.21 | 用一个数组实现两个栈 | 3.24 | `two_stacks.c` ✅ |
| 3.22 | 支持 FindMin 的栈 + 下界证明 | **3.25 a/b** | `min_stack.c` ✅ |
| 3.23 | 用一个数组实现三个栈 | 3.26 | `three_stacks.c` ✅ |
| 3.24 | 斐波那契递归的栈空间 | 3.27 | `recursion_depth.c` ✅ |
| 3.25 | 实现队列（链表/数组） | 3.28 | `queue_circular.c`、`stack_linked.c` ✅ |
| 3.26 | 双端队列（所有操作 O(1)） | 3.29 | `deque.c` ✅ |

## 第 4 章（4.1 ~ 4.46）

实现与推导见 `ch04_trees/ANSWERS.md`，测试见 `tests/test_ch04.c`。

| 你的编号 | 主题 | 实现 / 状态 |
|----------|------|-------------|
| 4.1 ~ 4.3 | 二叉树性质、深度 | `tree_core.c`（`tree_depth`） + ANSWERS.md ✅ |
| 4.4 ~ 4.7 | 四种遍历、遍历序列判定 | `tree_preorder/inorder/postorder/level_order` ✅ |
| 4.8 | 表达式树（中缀↔后缀、求值） | `expr_tree_from_postfix/eval/print` ✅ |
| 4.9 | 由两种遍历重建二叉树 | ANSWERS.md ✅ |
| 4.10 / 4.11 | BST 的 Find / Insert / Delete | `bst.c` ✅ |
| 4.12 | 随机 BST 高度/IPL（随机实验） | `bst_random` + 测试 ✅ |
| 4.13 | 删除两子结点的三种做法 | `bst_delete_right_min/left_max/merge` ✅ |
| 4.14 | 内部路径长 IPL | `bst_internal_path_length` + 公式 ✅ |
| 4.15 | AVL 最少结点数 N(h) | `avl_min_nodes` ✅ |
| 4.16 | 单旋 / 双旋四种形态 | `avl.c`（`rotate_*` / `rebalance`） ✅ |
| 4.17 | 完全平衡 | `avl_is_perfect` ✅ |
| 4.18 / 4.19 | AVL 插入（递归 / 非递归） | `avl_insert` / `avl_insert_iter` ✅ |
| 4.20 | AVL 删除 | `avl_delete` ✅ |
| 4.21 | 高度域位宽（一字节足够） | `avl_height_bits` ✅ |
| 4.22 | AVL 高度上界 ≈ 1.44 log₂N | 由 4.15 反解 ✅ |
| 4.23 / 4.24 | Splay 访问伸展到根 / 删除 | `splay_access` / `splay_delete` ✅ |
| 4.25 ~ 4.27 | Splay 链式最坏 / 顺序访问 O(N) / 摊还 O(log N) | `splay_chain_path_length`、`splay_sequential_access_rotations`、旋转计数 ✅ |
| 4.28 | 一次遍历求结点/叶/满结点数 | `tree_stats` ✅ |
| 4.29 / 4.30 | 随机 BST / 最少结点 AVL 树 | `bst_random` / `avl_min_tree` ✅ |
| 4.31 | 有序数组 → 理想平衡 BST | `bst_ideal_from_sorted` ✅ |
| 4.32 | 打印 [k1,k2] 区间（剪枝） | `bst_print_range` ✅ |
| 4.33 / 4.34 | 分配坐标并画树 | `tree_assign_xy` / `tree_print_grid` / `tree_print_indented` ✅ |
| 4.35 | 层序遍历 | `tree_level_order` ✅ |
| 4.36 | 2-3 树（= M=3 的 B 树） | `tree23_create` ✅ |
| 4.37 | B 树插入/删除 | `btree_insert/delete` ✅ |
| 4.38 | B* 树 | ANSWERS.md（算法描述） ✅ |
| 4.39 / 4.40 | 儿子-兄弟表示、前/后序 | `cs_from_parents/cs_preorder/cs_postorder` ✅ |
| 4.41 / 4.42 | 相似 / 同构 | `tree_similar` / `tree_isomorphic` ✅ |
| 4.43 | 判断是否为 BST（开区间法） | `bst_is_bst` ✅ |
| 4.44 | FindKth（用子树大小） | `bst_find_kth` ✅ |
| 4.45 | 线索二叉树（中序无栈/无递归） | `threaded.c` ✅ |
| 4.46 | 2-d 树（插入/查找/范围/最近邻） | `kdtree.c` ✅ |

> 各章编号列表已全部核对完成（第 1~4 章）。后续章节可继续按「编号 + 一句话主题」发来。
