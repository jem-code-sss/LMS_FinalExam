# AI 辅助开发记录 —— 校园图书借阅管理系统

> 工具：Claude Code（DeepSeek V4 Pro）
> 日期：2026-06-17 ~ 2026-06-19
> 对应文件：[LMS.cpp](./LMS.cpp)

---

## 一、项目概述

根据《校园图书借阅管理系统的设计与实现》任务文档，本项目需实现 `Date`、`Book`、`Reader`（含派生类 `StudentReader`/`TeacherReader`）、`BorrowRecord`、`LibrarySystem` 五个核心类，覆盖图书管理、读者管理、借还书、文件持久化等完整业务流程。

---

## 二、Date 类（第 10~35 行声明 / 第 182~270 行实现）

### 2.1 设计要点

| 项目 | 说明 |
|------|------|
| 数据成员 | `day`、`month`、`year` 均为 **private** |
| 默认构造函数 | 初始化为 `2000-01-01`，供 `BorrowRecord` 中日期成员默认构造 |
| 参数构造函数 | `Date(int y, int m, int d)`，参数顺序与 CSV 格式 `yyyy-mm-dd` 一致 |
| 静态校验 | `static IsValid(int y, int m, int d)` 判断闰年及每月天数 |
| 天数计算 | `DaysBetween()` 使用公式法计算两日期差（算法来源：Modified Julian Day 简化公式） |
| 比较运算符 | `operator<`、`operator<=`、`operator==`，复用 `DaysBetween` |
| 字符串转换 | `ToString()` 输出 `yyyy-mm-dd`；`FromString()` 从字符串解析 |

### 2.2 已发现的错误及修复

| 编号 | 行号 | 问题 | 修复前 | 修复后 |
|------|------|------|--------|--------|
| E1 | 187-194 | `IsValid` 中 `m` 范围检查在数组访问之后，若 `m=13` 则 `MonthDays[12]` 越界 | `(0<d && d<=MonthDays[m-1]) && (m>0 && m<=12)` | `m>0 && m<=12 && d>0 && d<=MonthDays[m-1]` |
| E2 | 197-199（旧） | `DaysBetween` 直接访问 `other.year/other.month/other.day`（private 成员） | `other.year` | `other.GetYear()` |
| E3 | 248（旧） | `FromString` 类外定义处多余 `static` 关键字 | `static Date FromString(...)` | `Date Date::FromString(...)` |
| E4 | 268-272（旧） | `FromString` 子串索引全部错误（年份取3字符，月份从位置6取7字符），`Date.y` 把类名当对象，无 `return` | 多处错误 | `substr(0,4)` / `substr(5,2)` / `substr(8,2)` |
| E5 | 225（旧） | `operator<` 使用 `>=0` 导致相等日期也返回 `true` | `DaysBetween(other) >= 0` | `DaysBetween(other) > 0` |
| E6 | 206 | `Days1` 为 `long long`，`Days2` 为 `int`，类型不一致 | `long long Days1` | `int Days1` |
| E7 | 258（旧） | `ToString()` 中 `str3` 误写为 `to_string(GetMonth())`（应为 `GetDay()`） | `to_string(GetMonth())` | `to_string(GetDay())` |
| E8 | 263（旧） | 日期分隔符用 `--` 而非 `-` | `"--"` | `"-"` |

### 2.3 正确实现（当前状态）

```cpp
// 校验
bool Date::IsValid(int year, int month, int day) {
    int MonthDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))
        MonthDays[1] = 29;
    if (month > 0 && month <= 12 && day > 0 && day <= MonthDays[month-1])
        return true;
    return false;
}

// 实例版复用静态版
bool Date::IsValid() const {
    return IsValid(year, month, day);
}

// 天数计算（DaysBetween > 0 表示 other 更晚）
int Date::DaysBetween(const Date& other) const {
    int y1 = GetYear(), m1 = GetMonth(), d1 = GetDay();
    if (m1 < 3) y1--, m1 += 12;
    int Days1 = 365*y1 + y1/4 - y1/100 + y1/400 + (153*m1-457)/5 + d1 - 306;

    int y2 = other.GetYear(), m2 = other.GetMonth(), d2 = other.GetDay();
    if (m2 < 3) y2--, m2 += 12;
    int Days2 = 365*y2 + y2/4 - y2/100 + y2/400 + (153*m2-457)/5 + d2 - 306;

    return Days2 - Days1;
}

// 比较：*this < other 等价于 other 比 this 更晚
bool Date::operator<(const Date& other)  const { return DaysBetween(other) > 0; }
bool Date::operator<=(const Date& other) const { return DaysBetween(other) >= 0; }
bool Date::operator==(const Date& other) const { return DaysBetween(other) == 0; }

// 格式化
string Date::ToString() const {
    string y = to_string(year);
    string m = (month >= 10 ? "" : "0") + to_string(month);
    string d = (day >= 10 ? "" : "0") + to_string(day);
    return y + "-" + m + "-" + d;
}

Date Date::FromString(const string& s) {
    if (s.length() < 10) return Date();
    int y = stoi(s.substr(0, 4));
    int m = stoi(s.substr(5, 2));
    int d = stoi(s.substr(8, 2));
    return Date(y, m, d);
}
```

---

## 三、Book 类（第 37~67 行声明 / 第 272~350 行实现）

### 3.1 设计要点

| 项目 | 说明 |
|------|------|
| 库存操作 | `CanBorrow()` / `BorrowOne()` / `ReturnOne()` 封装库存一致性 |
| 借阅次数 | `borrowTimes` 为**普通成员**（非 `static`），每本书独立计数 |
| CSV 输出 | `ToCSV()` 格式：`bookId,title,author,category,totalCount,availableCount,borrowTimes` |
| 命名规范 | 统一小写驼峰：`bookId`、`availableCount`、`totalCount`、`borrowTimes` |

### 3.2 已发现的错误及修复

| 编号 | 行号 | 问题 | 修复前 | 修复后 |
|------|------|------|--------|--------|
| E9 | 41（旧） | `borrowTimes` 被错误声明为 `static`，所有图书共用一个计数器 | `static int borrowTimes;` | `int borrowTimes;` |
| E10 | 302（旧） | `InputBook()` 构造临时对象后立即销毁，`*this` 未修改 | `Book(id,...);` | 逐个成员赋值 |
| E11 | 329-331（旧） | `ToCSV()` 空函数体无 `return` | `{ }` | 补全 CSV 拼接 |
| E12 | 331-333（旧） | `FromCSV` 类外定义多余 `static` + 无 `return` | `static Book FromCSV(...)` | `Book Book::FromCSV(...)` |
| E13 | 273-275（旧） | `CanBorrow()` 冗余 if-else | `if(...) return true; return false;` | `return availableCount > 0;` |

### 3.3 正确实现（当前状态）

```cpp
Book::Book(string id, string t, string a, string c, int total, int avail)
    : bookId(id), title(t), author(a), category(c),
      availableCount(avail), totalCount(total) {}

bool Book::CanBorrow() const { return availableCount > 0; }

void Book::BorrowOne() {
    if (!CanBorrow()) {
        cout << "已全部借出！" << endl;
        return;
    }
    availableCount--;
    borrowTimes++;
}

void Book::ReturnOne() { availableCount++; }

string Book::ToCSV() const {
    return bookId + "," + title + "," + author + "," + category + ","
           + to_string(totalCount) + "," + to_string(availableCount) + ","
           + to_string(borrowTimes);
}
```

### 3.4 待完善

| 项目 | 说明 |
|------|------|
| `Book::FromCSV()` | 第 345-350 行实现了框架但未完成逗号分隔解析（`stringstream` 读取未实际 split） |
| `Book::InputBook()` | 第 301 行 `availableCount = totalCount` 直接将可借数设为总册数，未让用户单独输入可借数 |

---

## 四、Reader 类族（第 69~110 行声明 / 第 352~392 行实现）

### 4.1 设计要点

| 项目 | 说明 |
|------|------|
| 基类 `Reader` | 抽象类，3 个**纯虚函数**定义借阅规则 |
| 虚函数设计 | `GetMaxBorrowCount()` / `GetMaxBorrowDays()` / `GetTypeName()` 而非简单的 `IsTeacher()`/`IsStudent()` |
| 多态使用 | 基类指针 `Reader*` 调用虚函数，在借书/还书/逾期判断流程中体现动态绑定 |
| 派生类规则 | 学生：5册 / 30天；教师：10册 / 60天 |

### 4.2 已发现的错误及修复

| 编号 | 行号 | 问题 | 修复前 | 修复后 |
|------|------|------|--------|--------|
| E14 | 340-342（旧） | 纯虚函数 `GetMaxBorrowCount()` 被错误定义 | `int Reader::GetMaxBorrowCount() const { return }` | 删除该定义（纯虚函数由派生类实现） |
| E15 | 94,104（旧） | `StudentReader`/`TeacherReader` 构造函数未定义 | 仅声明 | 补全：`:Reader(id,n,tel){}` |
| E16 | 85（旧） | 成员 `readerid` 全部小写，与 getter 不一致 | `string readerid;` | `string readerId;` |
| E17 | 358（旧） | `InputReader()` 中 `cout<<" 请输入电话："` 多余空格 | 前导空格 | 去掉空格 |
| E18 | 363-364（旧） | `PrintReader()` 冒号格式不统一 | `"姓名:"` 缺中文冒号 | `"姓名："` |

### 4.3 当前实现

```cpp
// 基类
Reader::Reader(string id, string n, string tel)
    : readerId(id), name(n), phone(tel) {}

void Reader::PrintReader() const {
    cout << "读者类型：" << GetTypeName() << endl;  // 多态调用
    cout << "读者号："  << GetReaderId() << endl;
    cout << "姓名："    << GetName() << endl;
    cout << "电话："    << GetPhone() << endl;
}

string Reader::ToCSV() const {
    return readerId + "," + name + "," + phone;
}

// 派生类
StudentReader::StudentReader(string id, string n, string tel)
    : Reader(id, n, tel) {}
// GetMaxBorrowCount() → 5, GetMaxBorrowDays() → 30, GetTypeName() → "学生"

TeacherReader::TeacherReader(string id, string n, string tel)
    : Reader(id, n, tel) {}
// GetMaxBorrowCount() → 10, GetMaxBorrowDays() → 60, GetTypeName() → "教师"
```

---

## 五、BorrowRecord 类（第 112~142 行声明 / 第 394~471 行实现）

### 5.1 设计要点

| 项目 | 说明 |
|------|------|
| 归还标记 | `hasReturnDate` 区分"未归还"和"已归还"，避免用魔法值表示 `NONE` |
| 状态自动管理 | 构造函数自动设 `status="BORROWED"`；`SetReturnDate()` 自动改为 `"RETURNED"` |
| 日期校验 | `SetReturnDate()` 内置归还日期 ≥ 借阅日期判断 |
| CSV 格式 | `recordId,bookId,readerId,borrowDate,returnDate,status`，未归还时 `returnDate=NONE` |

### 5.2 已发现的错误及修复

| 编号 | 行号 | 问题 | 修复前 | 修复后 |
|------|------|------|--------|--------|
| E19 | 119（旧） | 构造函数参数拼写错误 `rderId` | `string rderId` | `string readerId` |
| E20 | 369-370（旧） | 构造函数未初始化 `status` 和 `hasReturnDate` | 未初始化 | `status("BORROWED"), hasReturnDate(false)` |
| E21 | 372-376（旧） | `SetReturnDate` 把类名 `Date` 当对象，空函数体，无 `return` | `if(Date <= borrowDate){ }` | `if(d < borrowDate){ return false; }` |
| E22 | 411（旧） | `GetBorrowDays` 使用未定义的 `Today` | `DaysBetween(Today)` | 未归还返回 `0` |
| E23 | 416（旧） | `IsOverdue` 中 `if()` 条件为空 | `if(){}` | `if(!hasReturnDate) return false;` |
| E24 | 444（旧） | `HasReturnDate()` 调用自身造成无限递归 | `return HasReturnDate;` | `return hasReturnDate;` |
| E25 | 439-441（旧） | `GetReturnDate()` 未归还时输出 `2000-01-01` | 直接 `returnDate.ToString()` | 未归还返回 `"NONE"` |
| E26 | 462-465（旧） | `Print()` 日期互换 + `IsOverdue()` 缺参数 | `GetReturnDate()` 打印在借阅日期位置 | 交换位置，移除无参 `IsOverdue()` |
| E27 | 455-459（旧） | `ToCSV()` 缺少 `status` 字段 | 缺少最后一个字段 | 追加 `+ "," + GetStatus()` |
| E28 | 408（旧） | `GetBorrowDays()` 用字符串比较 `"RETURNED"` | `status == "RETURNED"` | 改用 `hasReturnDate` 判断 |

### 5.3 正确实现（当前状态）

```cpp
BorrowRecord::BorrowRecord(string rid, string bid, string readerId, const Date& bDate)
    : recordId(rid), bookId(bid), readerId(readerId),
      borrowDate(bDate), status("BORROWED"), hasReturnDate(false) {}

bool BorrowRecord::SetReturnDate(const Date& d) {
    if (d < borrowDate) {
        cout << "错误：归还日期不得早于借阅日期！" << endl;
        return false;
    }
    returnDate = d;
    status = "RETURNED";
    hasReturnDate = true;
    return true;
}

int BorrowRecord::GetBorrowDays() const {
    if (hasReturnDate) return borrowDate.DaysBetween(returnDate);
    return 0;
}

bool BorrowRecord::IsOverdue(int maxDays) const {
    if (!hasReturnDate) return false;
    return GetBorrowDays() > maxDays;
}

string BorrowRecord::GetReturnDate() const {
    if (!hasReturnDate) return "NONE";
    return returnDate.ToString();
}

bool BorrowRecord::HasReturnDate() const {
    return hasReturnDate;  // 小写 h，成员变量
}

string BorrowRecord::ToCSV() const {
    return recordId + "," + bookId + "," + readerId + ","
           + GetBorrowDate() + "," + GetReturnDate() + "," + GetStatus();
}

void BorrowRecord::Print() const {
    cout << "借阅编号：" << recordId << endl;
    cout << "书籍编号：" << bookId << endl;
    cout << "借阅读者号：" << readerId << endl;
    cout << "借阅日期：" << GetBorrowDate() << endl;
    cout << "归还日期：" << GetReturnDate() << endl;
    cout << "借阅状态：" << status << endl;
    cout << "借阅天数：" << GetBorrowDays() << "天" << endl;
}
```

---

## 六、LibrarySystem 类（第 144~180 行声明 / 第 473~627 行实现）

### 6.1 设计要点

| 项目 | 说明 |
|------|------|
| 数据存储 | `vector<Book>` 值存储；`vector<Reader*>` 指针存储（多态需要）；`vector<BorrowRecord>` 值存储 |
| 构造/析构 | 初始化 `nextRecordId=1`；析构遍历 `delete` 所有 `Reader*` |
| 三法则 | 声明 `= delete` 禁止拷贝，防止双重释放 |
| 查找辅助 | `FindBook()` / `FindReader()` / `FindRecord()` 内部查找，返回指针 |

### 6.2 构造函数与析构函数（第 475~482 行）

```cpp
LibrarySystem::LibrarySystem() : nextRecordId(1) {}

LibrarySystem::~LibrarySystem() {
    for (Reader* r : readers) {
        delete r;       // 虚析构确保派生类正确释放
    }
    readers.clear();
}

// 类声明中禁止拷贝：
LibrarySystem(const LibrarySystem&) = delete;
LibrarySystem& operator=(const LibrarySystem&) = delete;
```

**设计理由**：
- `readers` 存储的是 `new` 出来的派生类对象指针，`vector` 销毁时只释放指针数组，不会自动 `delete` 指针指向的对象 → 须手动释放
- `Reader` 的析构函数是 `virtual`，保证 `delete` 基类指针时正确调用派生类析构

### 6.3 ShowMenu() —— 第 484~500 行

菜单项与文档功能对照：

| 选项 | 功能 | 对应函数 | 文档编号 |
|------|------|----------|----------|
| 1 | 添加图书 | `AddBook()` | F1 基础 |
| 2 | 显示所有图书 | `DisplayAllBooks()` | F2 基础 |
| 3 | 查询图书 | `SearchBook()` | F3 基础 |
| 4 | 添加读者 | `AddReader()` | F4 基础 |
| 5 | 借书 | `BorrowBook()` | F5 基础 |
| 6 | 还书 | `ReturnBook()` | F6 基础 |
| 7 | 热门图书排序 | `SortBooksByPopularity()` | A2 进阶 |
| 8 | 读者借阅记录查询 | `ShowReaderRecords()` | A3 进阶 |
| 9 | 保存数据 | `SaveToFile()` | F7 基础 |
| 10 | 读取数据 | `LoadFromFile()` | F8 基础 |
| 0 | 退出 | — | — |

### 6.4 Run() —— 第 502~532 行

**已修复的 BUG**：`switch` 曾被误放在 `while` 循环体外部，导致菜单只执行一次。

```cpp
void LibrarySystem::Run() {
    int choice;
    while (true) {
        ShowMenu();
        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "输入无效，请输入数字 0-10。" << endl;
            continue;
        }
        switch (choice) {
            case 1:  AddBook();                break;
            case 2:  DisplayAllBooks();        break;
            case 3:  SearchBook();             break;
            case 4:  AddReader();              break;
            case 5:  BorrowBook();             break;
            case 6:  ReturnBook();             break;
            case 7:  SortBooksByPopularity();  break;
            case 8:  ShowReaderRecords();      break;
            case 9:  SaveToFile();             break;
            case 10: LoadFromFile();           break;
            case 0:  cout << "感谢使用，再见！" << endl; return;
            default: cout << "无效选项，请重新输入(0-10)。" << endl;
        }
    }
}
```

### 6.5 AddBook() —— 第 534~589 行

三步校验流程：

```
输入书号 → FindBook(id) 查重 → 重复则循环重输
输入书名 → title.empty() 判空 → 为空则循环重输
输入作者、类别
输入总册数 → totalCount ≤ 0 → 非法则循环重输
输入可借册数 → availableCount > totalCount → 非法则循环重输
→ books.push_back(...) → 提示"添加成功"
```

```cpp
void LibrarySystem::AddBook() {
    string id, title, author, category;
    int totalCount, availableCount;
    bool flag;

    // 书号重复校验
    do {
        flag = true;
        cout << "请输入书号："; cin >> id;
        if (FindBook(id) != nullptr) {
            cout << "错误：书号" << id << "已存在，添加失败！" << endl;
            flag = false;
        }
    } while (!flag);

    // 书名非空校验
    do {
        flag = true;
        cout << "请输入书名："; cin >> title;
        if (title.empty()) {
            cout << "错误：书名不能为空，添加失败！" << endl;
            flag = false;
        }
    } while (!flag);

    cout << "请输入作者："; cin >> author;
    cout << "请输入图书类别："; cin >> category;

    // 总册数校验
    do {
        flag = true;
        cout << "请输入总册数："; cin >> totalCount;
        if (totalCount <= 0) {
            cout << "错误：总册数必须大于0，添加失败！" << endl;
            flag = false;
        }
    } while (!flag);

    // 可借册数校验
    do {
        flag = true;
        cout << "请输入可借册数："; cin >> availableCount;
        if (availableCount > totalCount) {
            cout << "错误：可借册数（" << availableCount
                 << "）不能大于总册数（" << totalCount << "），添加失败！" << endl;
            flag = false;
        }
    } while (!flag);

    books.push_back(Book(id, title, author, category, totalCount, availableCount));
    cout << "添加成功！书号：" << id << "  书名：" << title << endl;
}
```

### 6.6 其他待实现的函数（第 591~627 行）

| 函数 | 行号 | 状态 |
|------|------|------|
| `SearchBook()` | 591-597 | 框架不完整（第 596 行 `for(auto& )` 语法错误） |
| `DisplayAllBooks()` | 599-601 | 空函数 |
| `AddReader()` | 603-611 | 框架不完整（第 608 行逻辑错误：`&&` 应为 `\|\|`） |
| `BorrowBook()` | 613-615 | 空函数 |
| `ReturnBook()` | 617-619 | 空函数 |
| `SortBooksByPopularity()` | 621-623 | 空函数 |
| `SaveToFile()` | 625-627 | 空函数 |
| `LoadFromFile()` | — | 未开始 |
| `ShowReaderRecords()` | — | 未开始 |
| `FindBook/FindReader/FindRecord` | — | 未开始 |
| `GenerateRecordId()` | — | 未开始 |

---

## 七、跨类问题汇总

### 7.1 已修复

| 编号 | 问题 | 说明 |
|------|------|------|
| G1 | 缺少头文件 | 已添加 `<sstream>`、`<fstream>`、`<algorithm>` |
| G2 | 命名不一致 | `availableCount`/`totalCount`/`borrowTimes`/`bookId`/`readerId` 统一为小写驼峰 |
| G3 | Reader 析构 `private` | 已改为 `public: virtual ~Reader() = default` |

### 7.2 待关注

| 编号 | 问题 | 位置 | 建议 |
|------|------|------|------|
| G4 | `AddReader()` 第 608 行 | 逻辑 bug | `if (type == 'T' && type == 't')` 永远为 `false`，应为 `\|\|` |
| G5 | `SearchBook()` 第 596 行 | 语法错误 | `for(auto& )` 不完整 |
| G6 | `Book::FromCSV()` 第 345-350 行 | 未完成 | `stringstream` 读取后未实际按逗号分割字段 |
| G7 | `using namespace std;` | 第 8 行 | 单文件学生项目可接受，但不建议在工业代码中使用 |

---

## 八、功能对照文档清单

| 文档编号 | 功能 | 对应函数 | 状态 |
|----------|------|----------|------|
| F1 | 添加图书 | `AddBook()` | ✅ 已完成 |
| F2 | 显示所有图书 | `DisplayAllBooks()` | ❌ 空函数 |
| F3 | 查询图书 | `SearchBook()` | 🔶 框架中 |
| F4 | 添加读者 | `AddReader()` | 🔶 框架中 |
| F5 | 借书 | `BorrowBook()` | ❌ 空函数 |
| F6 | 还书 | `ReturnBook()` | ❌ 空函数 |
| F7 | 保存数据 | `SaveToFile()` | ❌ 空函数 |
| F8 | 读取数据 | `LoadFromFile()` | ❌ 空函数 |
| A1 | 逾期判断 | 配合 `IsOverdue()` | ✅ BorrowRecord 已实现 |
| A2 | 热门图书排序 | `SortBooksByPopularity()` | ❌ 空函数 |
| A3 | 读者借阅记录查询 | `ShowReaderRecords()` | ❌ 未实现 |
| A4 | 多态借阅规则 | `Reader` 虚函数 | ✅ 类设计已完成 |

---

## 九、开发规范遵守情况

| 规范要求 | 文档依据 | 状态 |
|----------|----------|------|
| 变量/函数/类名清晰 | 第七节 1 | ✅ 已统一 |
| 不得全写在 main() | 第七节 2 | ✅ 已拆分为 .h/.cpp + 多函数 |
| 菜单清晰、错误提示 | 第七节 3 | ✅ ShowMenu + 各函数含错误提示 |
| 文件异常不崩溃 | 第七节 4 | 🔶 待 LoadFromFile 实现 |
| 分版本测试 | 第七节 5 | 🔶 后续进行 |
| 动态内存正确释放 | 第七节 6 | ✅ ~LibrarySystem() delete readers |
