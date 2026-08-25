# LMS 零基础上手指南 (ONBOARDING.md)

> 写给图书馆管理系统的四位组员。假设你**完全没写过 C++**也能跟着读懂。
> 目标不只是"能跑"，而是**答辩 (VIVA) 时你能讲清楚自己模块的每一行**——
> 评分表里 Individual Ownership (4分) 和 Your Module / Logic / Coding / Troubleshooting (20分) 全靠这个。

---

## 0. 怎么用这份文档

| 你是谁 | 必读 | 选读 |
|---|---|---|
| Clement (Module 1 会员) | §1 全景 → §3 跑起来 → §4 地基 → §5 概念 → §6 M1 详解 | §10 演示脚本、§11 VIVA 题库 |
| Wen Kai (Module 2 图书) | §1 → §3 → §4 → §5 → §7 M2 详解 | 同上 |
| Leong (Module 3 借阅) | §1 → §3 → §4 → §5 → §8 M3 详解（最长最重要） | 同上 |
| Zi Chen (Module 4 罚款) | §1 → §3 → §4 → §9 M4 速览 | §10、§11 |
| 所有人 | §12 Git 生存指南、§13 排错手册 | |

**答辩前一周**：只看你模块那一节的「⭐ 三句话」和 §11 对应题组，反复自问自答。

---

## 1. 项目全景

### 1.1 这是什么

一个**菜单驱动的 C++ 控台程序**，模拟图书馆日常业务：
管理会员 → 管理藏书 → 借书还书预约 → 收罚款出报表。
四个人各写一个模块，最后**拼成一个程序**，靠 `common.h/common.cpp` 里的共享数据互通。

### 1.2 分工

| 模块 | 负责人 | 业务范围 |
|---|---|---|
| M1 会员管理 | Clement Ho Jun Hao | 注册、搜索（ID/姓名/电话）、更新、停权/恢复、删除、列表、记录卡 |
| M2 图书目录 | Chong Wen Kai | 增删改查图书、搜索（ID/书名/作者/ISBN）、副本增减、书架定位、目录与在架清单 |
| M3 借阅归还预约 | Leong Zheng Yang | 借书、续借、还书（自动开罚单）、预约排队、取消预约、在借清单、队列显示 |
| M4 罚款与报表 | Ang Zi Chen | 罚金计算、收款找零、部分还款、收据、四张报表 |
| 公共层 + 主菜单 | 全队共建 | 数据结构、日期运算、通用校验输入、种子数据 |

### 1.3 文件地图

```
library-management/
├── main.cpp              主菜单、开场/结束画面、系统日期工具（选项 5）
├── common.h              共享层的"说明书"：结构体、常量、所有跨模块函数的声明
├── common.cpp            共享层实现：日期运算、查找函数、canBorrow()、种子数据
├── module1_member.cpp    M1 全部代码（Clement 维护）
├── module2_book.cpp      M2 全部代码（Wen Kai 维护）
├── module3_loan.cpp      M3 全部代码（Leong 维护）
├── module4_fine.cpp      M4 全部代码（Zi Chen 维护）
├── run.bat               双击一键编译+运行（需要装好 g++）
├── lms.exe               编译好的成品（以最新源码为准）
└── ONBOARDING.md         本文档
```

### 1.4 程序骨架（先有个整体印象）

```
main.cpp 的 main()
   │  seedData() 先塞入演示数据
   │  do { 显示主菜单 → 读选择 } while (没选 0)
   │       ├─ 选 1 → memberManagementMenu(...)   ← M1 的入口
   │       ├─ 选 2 → bookCatalogueMenu(books)    ← M2 的入口
   │       ├─ 选 3 → loanMenu(loans, ...)        ← M3 的入口
   │       ├─ 选 4 → fineMenu(fines, ...)        ← M4 的入口
   │       └─ 选 5 → systemDateMenu()            ← 调整"今天"的假时钟
   ▼
每个 xxxMenu() 内部又是同样的套路：
   do { 显示子菜单 → readInt() 读选择 → switch 分发到具体功能函数 } while (没选 0)
```

> **记住这个模式**：`do-while 菜单循环 + switch 分发`。四个模块长得一模一样，
> 你只需要往这个骨架里填自己模块的功能函数。

---

## 2. 环境准备（一次性，15 分钟）

### 2.1 装 C++ 编译器（Windows）

编译器负责把你写的 `.cpp` 文本翻译成机器能跑的 `.exe`。

**方式 A：学校机房的 Code::Blocks**
自带 MinGW，设置里把编译器路径指过去就能用，无需安装。

**方式 B：自己装 MSYS2（推荐，和仓库测试环境一致）**
1. 去 https://www.msys2.org 下载安装；
2. 打开 UCRT64 终端，执行：`pacman -S mingw-w64-ucrt-x86_64-gcc`
3. 把 `C:\msys64\ucrt64\bin` 加入系统 PATH 环境变量；
4. 新开一个终端验证：`g++ --version` 能打印版本号即成功。

### 2.2 装 VS Code（可选但强烈建议）

装好后加两个扩展：**C/C++**（微软官方，代码高亮跳转）和 **GitLens**（看谁改过哪行）。

### 2.3 拿到代码

```bash
git clone https://github.com/marcoangzc/library-management.git
cd library-management
```

### 2.4 编译并运行（三种方法任选）

```bash
# 方法一：双击 run.bat（本质就是下面这条命令）

# 方法二：手动编译（六个文件必须一起编，缺一不可）
g++ -std=c++17 -Wall -Wextra main.cpp common.cpp module1_member.cpp \
    module2_book.cpp module3_loan.cpp module4_fine.cpp -o lms.exe
./lms.exe

# 方法三：直接跑现成的（注意可能是旧版本，演示前务必重新编译）
./lms.exe
```

参数含义：`-std=c++17` 用 2017 版 C++ 标准；`-Wall -Wextra` 打开所有警告
（警告 = 编译器提醒"这里可能有问题"，**我们的目标是零警告**）；`-o lms.exe` 指定输出的可执行文件名。

---

## 3. 第一次跑起来

### 3.1 主菜单

```
========== LIBRARY MANAGEMENT SYSTEM - MAIN MENU ==========
  1. Member Management          ← M1
  2. Book Catalogue & Inventory ← M2
  3. Loan, Return & Reservation ← M3
  4. Fine Payment & Reporting   ← M4
  5. System Date (testing utility)
  0. Exit
  Members: 6   Titles: 8   Loans: 6
```

底部那行实时统计来自 `countActiveMembers()` / `countActiveBooks()`。

### 3.2 种子数据（演示和测试全靠它）

`seedData()` 在程序启动时塞入一批假数据，**每个数字都是精心设计的**：

**会员（6 人）**

| ID | 姓名 | 级别 tier | 特点（演示用途） |
|---|---|---|---|
| 1001 | Tan Wei Ming | S 学生 | 正常，已借 2 本（上限 3），再借 1 本就到顶 |
| 1002 | Nurul Aisyah | F 职员 | 正常，罚款半价 (×0.50)，有一笔逾期 |
| 1003 | Ravi Kumar | P 高级 | 借书上不封顶感（上限 6），**罚款全免 (×0)**，排在 2003 队列第 1 位 |
| 1004 | Lim Siew Ling | S 学生 | **欠 RM 4.50 罚款** → 拒绝借书演示 |
| 1005 | Chong Kai Xin | S 学生 | **被停权** → 拒绝借书演示 |
| 1006 | Ahmad Faizal | F 职员 | 排在 2003 队列第 2 位 |

**图书（8 种）**

| ID | 书名 | 类别 | 总数/在架 |
|---|---|---|---|
| 2001 | The Silent Patient | Fiction | 4/3 |
| 2002 | Sapiens | Non-Fiction | 3/2 |
| 2003 | A Brief History of Time | Science | **2/0 全部借出** ← 预约演示主角 |
| 2004 | Clean Code | Technology | 5/5 |
| 2005 | Guns, Germs and Steel | History | 2/2 |
| 2006 | Matilda | Children | 6/6 |
| 2007 | The Pragmatic Programmer | Technology | 3/3 |
| 2008 | Educated | Non-Fiction | 2/1 |

**借阅（6 笔）+ 罚款（1 笔）**（默认系统日期 2026-08-06）

| LoanID | 会员 | 书 | 到期日 | 状态 |
|---|---|---|---|---|
| 5000 | 1004 | 2006 | 06-15 | 已还（迟还 15 天 → 罚单 9001，RM 4.50 未付） |
| 5001 | 1001 | 2001 | 08-08 | 在借，未逾期 |
| 5002 | 1002 | 2003 | 07-24 | **逾期 13 天** |
| 5003 | 1004 | 2002 | 07-29 | **逾期 8 天** |
| 5004 | 1002 | 2008 | 08-13 | 在借 |
| 5005 | 1001 | 2003 | 08-03 | **逾期 3 天** |

> 演示前先把这张表存手机里，评委随便点名一个 ID 你都能接得住。

### 3.3 系统日期工具（主菜单选项 5）

程序里的"今天"不是真实时钟，是一个叫 `systemDate` 的全局变量。
选项 5 可以让它前进 N 天或直接设成某天——**一秒造出逾期场景**，这是给演示准备的开关。
所有逾期判断都读它：改完日期，回到 M3 的在借清单，逾期天数立刻变化。

### 3.4 业务规则速记卡

| 规则 | 数值 | 定义位置 |
|---|---|---|
| 借期 | 14 天 | `LOAN_DAYS` (common.h) |
| 最多续借 | 2 次 | `MAX_RENEW` |
| 罚金 | 每天 RM 0.30 | `FINE_PER_DAY` |
| 罚金封顶 | RM 20.00 | `FINE_CAP` |
| 借书上限 S/F/P | 3 / 4 / 6 本 | `LIMIT_*` |
| 罚款倍率 S/F/P | ×1.00 / ×0.50 / ×0.00 | `RATE_*` |
| 队列长度 | 每本书最多 10 人 | `MAX_QUEUE` |

> **改规则只需动 common.h 一处**——这就是"常量集中管理"的价值，VIVA 加分句。

---

## 4. 地基篇：读懂 common.h（所有人必读）

四个模块能互相配合，全靠这一层。搞懂本节，你的模块就懂了一半。

### 4.1 结构体 struct：一张有多格的卡片

```cpp
struct Member {
    int    memID;        // 会员号，如 1001
    string name;         // 姓名
    string phone;
    char   tier;         // 'S'学生 'F'职员 'P'高级
    int    booksOnLoan;  // 当前借了几本
    double outstandingFine;
    bool   suspended;    // 是否停权
    bool   active;       // ★ 软删除标志，见 4.4
};
```

类比：`struct` 是**卡片模板**，`Member m1;` 是照模板做了一张实体卡片，
`m1.name = "Ali"` 是往格子里写字。四个核心卡片模板：
`Member`（人）、`Book`（书）、`Loan`(借阅流水)、`Fine`（罚单）。

### 4.2 vector：会自动变长的名单

```cpp
vector<Member> members;      // 一叠 Member 卡片
members.push_back(m1);       // 往末尾加一张
members.size();              // 现在有几张
members[i];                  // 取第 i 张（下标从 0 开始！）
```

为什么不用普通数组？因为注册多少会员事先不知道，vector 自动扩容。
四个共享名单 `members / books / loans / fines` 都在 `main()` 里创建，
**以引用方式传进各模块**——大家操作的是同一份数据。

### 4.3 全局共享数组：五张固定大小的表格

| 数组 | 形状 | 含义 | 谁写 |
|---|---|---|---|
| `reserveQueue[MAX_BOOKS][MAX_QUEUE]` | 200×10 | 每本书的预约排队表，格子存会员号，0=空 | M3 |
| `memberLoanHistory[MAX_MEMBERS][MAX_CATEGORIES]` | 100×6 | 每位会员各类书借过几本（推荐功能的原料） | M3 |
| `shelfLayout[MAX_SHELVES][SLOTS_PER_SHELF]` | 10×20 | 书架网格，格子存书号，0=空位 | M2 |
| `monthlyStats[MONTHS][MAX_CATEGORIES]` | 12×6 | 每月各类别罚款收入 | M4 |
| `borrowCount[MAX_BOOKS]` | 200 | 每本书累计被借次数（人气榜原料） | M3 |

> ⚠ **全项目最容易讲错的点**：`reserveQueue` 的**行下标是书在 vector 里的位置（0,1,2...），
> 不是书号（2001,2002...）**。因为书号从 2001 起跳，直接当行下标既越界又浪费 2000 行。
> 种子数据里 2003 是第三本书 → 下标 2 → `reserveQueue[2]` 才是它的队列。

### 4.4 软删除：只标记，不抹掉

删除会员/图书 = 把 `active` 设为 `false`，**记录永远留在 vector 里**。
为什么？

1. 历史借阅和罚款记录存的是 `memID/bookID`，人删了记录就成悬空引用；
2. 更隐蔽的坑：`reserveQueue` 的行号 = vector 下标，若 erase 掉中间一本书，
   后面所有书的队列**整体错一位**，等于全馆排队表作废。

所以查找函数有两套：`findMemberIndex()` 只找活的，`findMemberIndexAny()` 连死的也找
（打印历史记录时用后者）。列表和搜索则一律跳过 `active == false` 的记录。

### 4.5 日期运算：日期是数字，不是文字

```cpp
struct Date { int day; int month; int year; };

int    toDayNumber(Date d);      // 日期 → 从公元1年1月1日起的总天数（序号）
Date   addDays(Date d, int n);   // 序号加减后再转回来 = 加减天数
int    daysBetween(Date a, Date b); // 两个序号相减 = 相差几天
bool   isOverdue(const Loan& l, Date today); // status是在借 且 today晚于dueDate
int    daysOverdue(const Loan& l, Date today); // 逾期天数（已还的书按还书日算）
```

闰年、大小月全在 `daysInMonth()` 里处理。**为什么不用字符串存日期？**
字符串没法直接相减算天数，也没法可靠比较先后——转成序号后全是整数运算。

### 4.6 canBorrow()：资格检查只此一家

```cpp
// 返回码：0可借 1记录失效 2停权 3有欠款 4达到上限
int canBorrow(const Member& m);
string borrowStatusMessage(int code);  // 把返回码翻译成人话
```

M3 借书前必调它。好处：规则集中在公共层，M1 改规则 M3 不用动；
返回码而非 bool，用户能看到**具体被拒原因**。

### 4.7 通用输入函数：所有输入走同一扇门

| 函数 | 作用 |
|---|---|
| `readInt("提示", 最小, 最大)` | 读整数并强制范围，错了自动重新提示 |
| `readDouble(...)` | 读金额，同理 |
| `readLine / readNonEmptyLine` | 读一行文字（自动去首尾空格）/ 非空版 |
| `readChar("...", "YN")` | 读单个字符且必须在合法集内 |
| `readDate(...)` | 读日期，强制 `YYYY-MM-DD` 格式 |
| `confirmYesNo("...")` | 问 Y/N，返回 bool |

它们内部都有 `while(true)` 死循环 + 合法才 `return`，非法输入**永远逃不过去**。
你的模块里不要自己写 `cin >>`，统一用这些——输入风格全队一致，也是 UI 分数的一部分。

---

## 5. C++ 关键概念速成（VIVA 第 11 条考这个）

### 5.1 引用传递 `&`：给地址，还是给复印件？

```cpp
void addMember(vector<Member>& members);          // 引用：函数改的就是原件
void displayAllMembers(const vector<Member>& m);  // const 引用：只许看，不许改
void displayBookRow(Book b);                      // 值传递：拿到的是复印件
```

- **不带 &**：把整张卡片复印一份给函数，函数撕了复印件原件无损；
- **带 &**：把卡片本身递过去（想象给对方你家地址），函数改的就是原物；
- **const &**：递原物但附一张"只许看"的字条——既省去复印大对象的开销，又防止误改。

**本项目惯例（背下来）**：要在函数里修改的容器 → `vector<T>&`；
只读的大容器 → `const vector<T>&`；只读的小记录 → 直接值传递（如 `displayBookRow(Book b)`）。

### 5.2 一个函数怎么"返回"两个值？——引用参数

```cpp
// M4 的经典示范：既要带回找零又要带回余额，但 return 只能带一个值
bool processPayment(double amountDue, double amountPaid,
                    double& change, double& balance);
```

`change` 和 `balance` 前面的 `&` 让函数能把结果**写回调用者的变量**。
这是评分表明示的考点（pass-by-reference for multiple returned values）。

### 5.3 static：本文件私用，谢绝外借

两个模块都需要"转小写"辅助函数，都写了 `toLowerCopy()`——
链接器一看：两个同名全局函数，冲突报错！（我们真实踩过这个坑。）
在前面加 `static`，表示**只在本 .cpp 文件内可见**，问题消失。

### 5.4 while(true) 校验循环：非法输入的旋转门

```cpp
while (true) {
    m.phone = readNonEmptyLine("  Phone (10-11 digits): ", PHONE_MAX_LEN);
    if (validatePhone(m.phone)) break;          // 合法 → break 逃出循环
    cout << "  >> Invalid phone number...\n";   // 非法 → 回到开头重来
}
```

配合 4.7 的输入函数，形成双层防线：格式层（readXxx）挡住"不是数字"，
业务层（validateXxx）挡住"位数不对/重复/超限"。

### 5.5 switch：菜单的大脑

```cpp
switch (choice) {
    case 1: addMember(members);  break;   // break 不能省，否则"贯穿"到下一句
    case 2: searchMember(members); break;
    case 0: break;                        // 什么都不做，由 do-while 判断退出
}
```

---

## 6. Module 1 详解 —— 会员管理（Clement）

> ⭐ **三句话介绍我的模块**（VIVA 开场直接用）：
> ① 我负责会员从入库到离场的完整生命周期：注册、查询、更新、停权、注销；
> ② 所有输入经过双层校验（格式层 readXxx + 业务层 validateXxx）；
> ③ 删除是软删除，历史记录永远可追溯，这是全系统数据一致性的前提。

### 6.1 子菜单与函数对照

| 菜单 | 功能 | 函数 |
|---|---|---|
| 1 | 注册新会员 | `addMember()` |
| 2 | 搜索（ID/姓名/电话） | `searchMember()` → 三个子函数 |
| 3 | 更新资料 | `updateMember()` |
| 4 | 停权 / 恢复 | `suspendMember()` |
| 5 | 删除会员 | `deleteMember()` |
| 6 | 会员总表 | `displayAllMembers()` |
| 7 | 会员记录卡 | `displayMemberCard()` |

### 6.2 逐函数拆解

**addMember()** — 注册的完整防线：

```cpp
if ((int)members.size() >= MAX_MEMBERS) { ... }   // 第0层：容量。memberLoanHistory 只有100行
m.memID = nextMemberID(members);                  // 自动编号：扫出最大号+1（种子最大1006→新人1007）
while (true) { m.name = readNonEmptyLine(...);    // 第1层：非空、≤50字符
    if (validateName(m.name)) break; ... }        // 第2层：不含数字
while (true) { m.phone = ...;                     // 10~11 位纯数字才放行
    if (validatePhone(m.phone)) break; ... }
m.tier = readChar("...", "SFP");                  // 第3层：只许 S/F/P 三选一
```

新会员四项默认值：`booksOnLoan=0, outstandingFine=0, suspended=false, active=true`。
**为什么 ID 不让用户自己输？** 手输必重复、必有跳号，程序分配又快又唯一。

**searchMemberByID/ByName/ByPhone()** — 一个精确匹配 + 两个模糊匹配：
按 ID 直接 `findMemberIndex()`；按姓名/电话是**子串匹配**且不分大小写：

```cpp
if (toLowerCopy(members[i].name).find(target) != string::npos)  // 输"tan"能找到"Tan Wei Ming"
```

`toLowerCopy()` 把两边都转小写再比，所以 `TAN`、`tan`、`Tan` 一视同仁。

**updateMember()** — 空输入 = 保留原值（不强迫用户重填所有字段）；改动前先校验，
不合格打印 `Kept unchanged`，绝不把脏数据写进记录。

**suspendMember()** — 读当前状态 → 打印 → `confirmYesNo` 二次确认 → 取反：
`m.suspended = !m.suspended;` 一行完成切换（toggle 模式）。

**deleteMember()** — 三道闸门，顺序有讲究：

```cpp
int index = findMemberIndex(members, id);          // 闸1：人必须存在
if (m.booksOnLoan > 0 || m.outstandingFine > 0.0)  // 闸2：有借书或欠款不许走
    return;
if (!confirmYesNo(...)) return;                    // 闸3：本人确认
m.active = false;                                  // 软删除：只翻标记
```

**displayAllMembers()** — `setw(8)` 设定列宽让表格对齐；跳过 `!active`；
末尾输出 `countActiveMembers()` 总计。

**displayMemberCard()** — 注意签名多了个参数 `const vector<Loan>& loans`：
M3 的数据以**只读引用**传进来，卡片下半部分列出该会员的在借清单并用
`isOverdue()` 标红逾期。这是模块间协作的直接展示（评分表第 6 条 Module Integration）。

### 6.3 与其他模块的交接

| 数据 | 方向 | 场景 |
|---|---|---|
| `members[i].booksOnLoan` | M3 写 / 我显示 | 借还书时 ±1 |
| `members[i].outstandingFine` | M4 写 / 我显示 | 罚款开单与结清时刷新 |
| `members[i].suspended` | 我写 / M3 读 | canBorrow() 的第 2 道检查 |
| `loans` | M3 拥有 / 我只读 | 记录卡显示在借清单 |

---

## 7. Module 2 详解 —— 图书目录（Wen Kai）

> ⭐ **三句话**：① 我管图书目录的增删改查和物理存放位置（书架网格）；
> ② 核心恒等式：**在借本数 = totalCopies − availableCopies**，一切副本操作都围绕它校验；
> ③ 删除同样是软删除，还要顺手清掉书架格子和排队表。

### 7.1 核心恒等式（本模块的灵魂）

```
totalCopies（总副本）  =  availableCopies（在架可借） + 在借本数
```

所以"能不能删 n 本副本"只需判断 `n > availableCopies`——
想拿走在借的书是不可能的，程序会告诉你架上有几本。

### 7.2 逐函数拆解

**addBook()** — 校验最密集的函数：

```cpp
b.bookID = nextBookID(books);            // 种子最大 2008 → 新书 2009
// ISBN 三连击：恰好13位 → 全是数字 → 不与其他活跃书目重复
if (!isAllDigits(b.isbn) || (int)b.isbn.length() != ISBN_LEN) ...
if (findDuplicateISBN(books, b.isbn, -1) != -1) ...
b.category = category - 1;               // 用户输 1~6，存储 0~5（数组下标习惯）
b.shelf = 0; b.position = 0;             // 0 = 尚未安排书架位置
```

**findDuplicateISBN(books, isbn, skipIndex)** — `skipIndex` 是个小巧思：
新增时传 -1（谁都比）；修改时传自己的下标（别把自己当成"重复的别人"）。

**manageCopies → addCopies/removeCopies()** — 加副本两边同加；
减副本先拦截再执行，保证恒等式永不被破坏。

**assignShelfLocation()** — 书架是一张 10×20 的格子纸 `shelfLayout[shelf][position]`，
格子里写着占用的书号（0 = 空）。换位置的三步舞：

```cpp
if (b.shelf >= 1 && ...) shelfLayout[旧] = EMPTY;   // 1. 先腾旧位（支持给同一本书换位）
if (shelfLayout[新] != EMPTY) { 报错; 回滚旧位; }    // 2. 新位被占 → 原地不动
shelfLayout[新] = b.bookID;                          // 3. 入驻
```

**removeBook()** — 软删除 + 三件清理工作：
拦截在借 (`onLoan>0`) 和被预约持有 (`reservedCopies>0`) → 清空书架格子 →
清空该书的排队行 `reserveQueue[index][q] = EMPTY`（排队的人随书一起散）。

**displayAvailability()** — 状态判定优先级：

```cpp
if (books[i].reservedCopies > 0)      status = "RESERVED - copy held"; // 还回来的正被人排队等
else if (books[i].availableCopies==0) status = "ALL COPIES ON LOAN";
else                                  status = "AVAILABLE";
```

底部附带 `displayShelfMap()`：**嵌套 for 循环**（外层书架、内层格子）把整馆布局画出来——
评分表要求 nested loops，这就是标准答案现场。

**countByCategory(const vector<Book>& books, int category)** — 返回值传递示范：
统计结果是一个 int，直接 `return` 出去，不需要引用。

---

## 8. Module 3 详解 —— 借阅·归还·预约（Leong，全系统的心脏）

> ⭐ **三句话**：① 我管理每一本书的旅程：借出 → 续借 → 归还，以及无书可借时的排队等候；
> ② 还书是全系统的枢纽——自动算罚金开罚单（喂给 M4），并把归还的副本递给队列下一位；
> ③ 排队表 `reserveQueue` 行是图书 vector 下标、列是排队次序、格子里存会员号。

### 8.1 先吃透预约队列（用种子数据推演一遍）

初始状态：2003 号书两本全在外，排队表第 2 行（下标 2）：`[1003, 1006]`。

| 场景 | 发生了什么 |
|---|---|
| ① 会员 1001 想借 2003 | 队首 1003 ≠ 1001 → 拒绝："有人排你前面"，并询问要不要也排队 |
| ② 5002 归还（1002 还 2003） | 还书流程 → `promoteNextInQueue()` 弹出队首 1003 → 这本还回的书**不上架**，改为 `reservedCopies++` 扣给 1003 → 队列变 `[1006]` |
| ③ 1003 听闻来借 | 队首是自己 → 书架上没有 → 消耗预留本 `reservedCopies--` → 同时把自己移出队列 → 成功借到，提示 "This copy was held aside for you" |
| ④ 1006 不等了，取消预约 | 他已是队首且有预留本 → 预留本释放回架 `availableCopies++` → 队列清空 |
| ⑤ 没人排队时还书 | `incrementAvailable()` 正常上架 |

> **为什么借书不能无视队列？** 公平性。若放任任何人插队，排队者永远轮不到。
> 这是业务规则，也是你 VIVA 最能出彩的设计点。

### 8.2 issueLoan() 借书全流程（九步）

```
读会员ID ──► findMemberIndex 找不到？重新提示
   │
canBorrow()? ──≠0──► borrowStatusMessage() 说清拒绝原因，结束
   │=0
读图书ID ──► findBookIndex 同上
   │
reserveQueue[bi][0] 队首是谁？
   ├─ 别人      ──► 拒绝 + joinQueueOffer() 问要不要排队
   ├─ 自己      ──► 书架有就拿书架的，没有就消耗 reservedCopies
   │                 （都没有 → 提示库里没副本，留在队列里不动）
   └─ 没人排队  ──► decrementAvailable() 失败 = All Copies On Loan → 提议排队
   │
可选自定义出借日（必须 ≥ systemDate，不许倒签）
   │
建 Loan 记录：loanID=nextLoanID()，dueDate=calcDueDate(issueDate)=issueDate+14
   │
同步四个共享变量：
   m.booksOnLoan++                    （M1 的额度）
   borrowCount[bi]++                  （M4 人气榜）
   memberLoanHistory[mi][category]++  （推荐功能）
   books[bi].availableCopies-- 已由 decrementAvailable 完成
   │
打印 LOAN SLIP 借书单
```

### 8.3 renewLoan() 续借的三重拦截

```cpp
if (l.renewCount >= MAX_RENEW)   → "最多续借 2 次"           // 防无限续期
if (isOverdue(l, systemDate))    → "已逾期 N 天，先还款"     // 逾期不给续
if (reserveQueue[bi][0]!=EMPTY)  → "有人在等这本书"          // 有队不续，公平
l.renewCount++;
l.dueDate = calcDueDate(l.dueDate);   // ★ 从【当前到期日】顺延 14 天，不是从今天
```

第三条注意细节：只要有人排队就拒，哪怕还没轮到他。

### 8.4 returnBook() 还书全流程

```
找 Loan → 必须是在借状态（防重复还书）
   │
status = RETURNED; returnDate = systemDate
m.booksOnLoan--
   │
daysOverdue(l, systemDate) > 0？
   ├─ 是 → base = 天数 × 0.30 → capped = min(base, 20) → × getFineRate(tier)
   │        → round 到 2 位小数 → 查重（一笔贷款只有一张罚单）→ fines.push_back()
   │        → m.outstandingFine += amount   （M1 立刻能看到欠款）
   │        → Premium 免罚则只提示不开单（amount ≤ 0）
   └─ 否 → "Returned on time"
   │
promoteNextInQueue(reserveQueue, bi) 弹出队首
   ├─ 有人 → reservedCopies++（副本扣给他）
   └─ 没人 → incrementAvailable() 上架
```

**罚单公式背熟**（评委必问）：
`罚金 = min(逾期天数 × RM0.30, RM20) × 级别倍率(S×1.00 / F×0.50 / P×0)`
例：5002 逾期 13 天，职员 Nurul → 13 × 0.30 = 3.90 < 20 封顶不触发 → × 0.50 = **RM 1.95**。

### 8.5 预约与取消

**placeReservation()** 四道检查：在架必须为 0（有书借什么约？）→ 查重（同一人不许重复排）→
队满 10 拒绝 → confirmYesNo 确认入队。
**cancelReservation()** 特殊处理：若取消的是队首且已有预留本，先把书放回架子再移除排队记录
（不然这本"专属书"会永远悬着没人领）。

### 8.6 私有工具函数（都加了 static）

| 函数 | 一句话 |
|---|---|
| `findQueuePosition()` | 此人在此书的队列里排第几？没有返回 -1 |
| `firstFreeSlot()` | 队尾空位在哪？满了返回 -1 |
| `removeFromQueueAt()` | 删中间一人后整体前移补位，最后一位置 EMPTY |
| `promoteNextInQueue()` | 弹出队首并返回其会员号（EMPTY = 没人排） |
| `calcDueDate()` | addDays(issueDate, LOAN_DAYS)，语义化包装 |

---

## 9. Module 4 速览 —— 罚款与报表（Zi Chen）

> 你的模块你自己最清楚，这里留一张**流水线全景图**方便向队友解释交接关系：

```
returnBook(M3) 自动开罚单 Fine{amount, paid=0, settled=false}
        │
calculateOverdueFine(菜单1)：逾期一览表 + 补建漏掉的罚单（已有则只显示不重复建）
        │
processPayment(amountDue, amountPaid, &change, &balance)(菜单2)
        │  纯计算函数：不做任何输入输出，可单独测试 ← 评分亮点
        │  paid≥due → 找零+结清；paid<due → 部分还款余额顺延
        ▼
fines[fi].paid += credited; settled = (balance==0)
recalcMemberOutstanding()  ← 从罚单反算会员欠款总额，保证 M1 显示永不漂移
monthlyStats[月份][类别] += credited  ← 收入按月按类入账
        │
printReceipt(菜单3，须已付款) / 四张报表(菜单4~7)
```

**money2dp() 存在的理由**（排错好素材）：二进制浮点存不下 0.30 这种小数，
多次部分还款后余额可能是 0.0000000001，永远差一点没法判 settled——
所有金额过一遍 `round(x*100)/100` 彻底杜绝。

---

## 10. 十二步完整演示脚本（汇报彩排照这个来）

前提：重新编译后运行，系统日期默认 2026-08-06。每步给出按键路径和预期看点。

| # | 按键 | 看点 |
|---|---|---|
| 1 | 启动画面 → Enter | 介绍系统范围与四人分工 |
| 2 | 主菜单 `3` → `6` | 在借清单里 5002/5003/5005 标 OVERDUE（13/8/3 天）——种子数据设计的威力 |
| 3 | `3` → `1`，会员 `1005` | 借书被拒："Membership is suspended."（资格检查①） |
| 4 | `3` → `1`，会员 `1004` | 被拒："Outstanding fine must be settled first."（检查②） |
| 5 | `3` → `1`，`1001` + `2004` | 成功！借书单 due = 2026-08-20；主菜单 Books/Loans 数字变化 |
| 6 | `3` → `2`，续借 `5001` | 成功，due 变 2026-08-22（renewCount 1/2） |
| 7 | `3` → `2`，续借 `5001` 两次 | 第二次成功 due 变 2026-09-05；紧接着的**第三次**被拒（MAX_RENEW=2 已用完）（边界测试） |
| 8 | `3` → `3`，归还 `5002` | 自动算罚 RM 1.95 开罚单 9002；"Copy held for member 1003"（队列顶位） |
| 9 | `3` → `1`，`1003` + `2003` | "This copy was held aside for you..."（预约闭环，全场高光） |
| 10 | `4` → `2`，罚单 `9001` 付 RM5.00 | 找零 0.50，SETTLED；会员 1004 欠款清零 |
| 11 | `4` → `3`，打印 `9001` 收据 | 完整收据含 x1.00 tier multiplier 字样 |
| 12 | `4` → `7`，选月份 `8` | August 报表 Children 列出现刚收的 4.50（收入实时入账） |

**备用杀手锏**：主菜单 `5` → `1` 前进 30 天 → 回 `3` → `6`，逾期天数全体上涨——
演示"模拟时钟"设计，顺便展示系统对时间流逝的反应。

---

## 11. VIVA 题库（按评分维度分组，先自问再翻答案要点）

### A. 你的模块（评分第 9 条：功能 + Input→Processing→Output）

1. **一分钟介绍你的模块？** → 背 §6/§7/§8 的⭐三句话，再加一个真实数据例子。
2. **你的模块和其他模块在哪交换数据？** → 照抄 §6.3 的交接表思路列自己的。
3. **一条数据在你模块里的完整旅程？** → 例：借书 = 输入两个 ID（输入）→
   资格检查+扣副本+建记录（处理）→ 借书单（输出）。

### B. 你的逻辑（评分第 10 条）

4. **借书前做了哪几层检查，为什么这个顺序？** → 资格(canBorrow) → 图书存在 →
   队列优先权 → 库存。先拦"人"的问题再问"书"，错误信息最具体。
5. **续借为什么设三重拦截？各防什么？** → 见 §8.3 注释。
6. **罚金怎么算？** → 公式 + 举 RM 1.95 这个例子现场心算。
7. **逾期状态为什么不存储？** → 它是派生值：dueDate 与 systemDate 比一下就有，
   存了反而要在每次日期变化时刷新全部记录，容易不一致。

### C. 你的编码（评分第 11 条）

8. **指一处 pass-by-reference 和一处 pass-by-value，为什么？** →
   `vector<T>&` 要改原件 / `const&` 只读大对象省拷贝 / `displayBookRow(Book b)` 小记录复印件即可。
9. **const 引用的两个好处？** → 不复制整个 vector（快）+ 编译器禁止误改（安全）。
10. **reserveQueue 的行列是什么？为什么行不用 bookID？** → §4.3 警告框，一字不差。
11. **哪里用了 switch / while 校验 / 嵌套 for？** → switch=菜单分发；while=readXxx+validate 循环；
    嵌套 for=shelfMap / reserveQueue 遍历。
12. **struct 对比多个平行数组的好处？** → 一个 Member 打包七项相关数据，
    push_back 整卡移动不会错位；平行数组加一个字段要改到处。
13. **nextMemberID 怎么保证唯一？** → 扫全表取最大值+1，删除过的号也不复用（历史可追溯）。

### D. 你的排错（评分第 12 条，讲真实案例最加分）

14. **讲一次真实 bug** → 三选一：① M1 曾少一个右大括号导致 7 个函数嵌套报错
    （编译器提示 "function-definition is not allowed here"，学会顺着第一个错往下找）；
    ② 两文件同名 toLowerCopy 链接冲突，加 static 解决；
    ③ 浮点余额 0.0000000001 导致罚单永远无法结清，money2dp 解决。
15. **readInt 输入字母会发生什么？** → cin 进入失败状态 → clear() 清状态 →
    ignore() 丢掉坏输入 → 循环重新提示，程序绝不崩溃也不死收脏数据。
16. **如果要把借期改成 21 天？** → 只改 common.h 的 `LOAN_DAYS = 14` 一处，
    全系统生效——常量集中管理的价值（加分句）。
17. **为什么系统日期做成假时钟？** → 演示逾期不用真等两周；也便于测试任意边界日期。

### E. 系统级问题（可能被追问）

18. **两人同时改 main 分支冲突了怎么办？** → 见 §12 事故处理；预防：各改各的模块文件。
19. **为什么删除都用软删除？** → §4.4 两大理由，尤其 reserveQueue 错位那个。
20. **系统怎么防止同一张罚单收两次钱？** → settled 标志 + 结清后再付款会被拒 +
    一笔 loan 只允许存在一张 Fine（双重查重）。

---

## 12. Git 生存指南（每天都要用）

### 12.1 每日五连（背下来）

```bash
git pull                        # ① 开工前拉最新代码
# ...写代码...
git status                      # ② 看自己动了哪些文件（红色=未暂存）
git add 文件名                   # ③ 装进"待提交篮子"（git add . = 全部）
git commit -m "清晰说明这次改了什么"  # ④ 本地存档
git push                        # ⑤ 上传 GitHub
```

### 12.2 约定

- **main 分支任何时刻都必须能编译通过**——演示前一晚别往 main 推半成品；
- 大改动开自己的分支：`git checkout -b yourname-feature`，
  写完推上去在 GitHub 点 **Pull Request**，队友看过再 Merge；
- 各人只改自己的 `moduleX_*.cpp`，公共文件（common/main）改动先在群里说一声。

### 12.3 事故处理

| 状况 | 处理 |
|---|---|
| `git push` 被拒 (non-fast-forward) | 队友先推了新代码 → `git pull` 解决（如有冲突见下）→ 再 push |
| pull 时提示 conflict | 文件里 `<<<<<<<` 到 `>>>>>>>` 之间二选一手工合并，删掉标记，重新 add+commit |
| 本地改砸了想放弃 | `git restore 文件名`（⚠ 改动永久消失，想清楚再敲） |
| 提交信息写错了 | 还没 push：`git commit --amend` 改最近一次 |
| ⚠ 永远不要 | `git push --force`（会抹掉队友的工作） |

---

## 13. 排错手册

### 13.1 编译错误对照表

| 报错关键句 | 病根 | 备注 |
|---|---|---|
| `a function-definition is not allowed here before '{'` | 上一个函数少了右大括号 `}`，后面的函数全被"吞"进去 | 我们 M1 的真实事故 |
| `multiple definition of 'xxx'` | 两个 .cpp 定义了同名全局函数 | 加 `static` 或改名（toLowerCopy 真实事故） |
| `'cout' was not declared in this scope` | 忘了 `#include "common.h"` | common.h 里带了 iostream 和 using namespace std |
| `undefined reference to 'main'` | 编译命令里没带 main.cpp 或漏了某个文件 | 六个文件必须一起编 |
| `cannot open output file lms.exe: Permission denied` | 旧的 lms.exe 还开着 | 关掉它的窗口再编译 |
| `expected ';'` | 上一行少分号 | 看报错行号的**上一行** |

### 13.2 运行怪象

| 怪象 | 解释 |
|---|---|
| 输错类型后疯狂刷提示 | readXxx 的 while 校验循环在工作；交互模式下正常重试，管道喂错输入才会刷屏 |
| getline 被"跳过" | readInt 内部已经吃掉了换行符，别再手动 `cin.ignore()`（M1 真实事故，曾要多按一次回车） |
| 表格歪了 | 内容超过 setw 给的宽度；长名字截断用 `.substr(0, n)` |

### 13.3 演示前 Checklist

- [ ] `g++ -Wall -Wextra ...` 重新编译，**零警告**
- [ ] 双击 run.bat 能正常进入主菜单
- [ ] 手机里备好 §3.2 种子数据表和 §10 演示脚本
- [ ] 试一遍自己模块的每条拒绝路径（输错 ID、非法输入、边界值）
- [ ] 全组过一遍 §11 题库，每人能脱稿答自己模块的题

---

*本文档由 Zi Chen 维护。发现讲得不清楚的地方，直接改这份文件然后 push——它和代码一样是需要持续打磨的产品。*

