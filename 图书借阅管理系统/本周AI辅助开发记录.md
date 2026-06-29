# 本周 AI 辅助开发记录（2026.06.22–2026.06.26）

---

## 第1天（06.22）— 核心业务流程实现

### 1.1 BorrowBook() — 借书函数

**问题**：函数只完成了图书查找和读者号输入，后续逻辑全部缺失（`found2 = true` 写死）。

**修正前（L678-701）**：

```cpp
void LibrarySystem::BorrowBook(){
    cout<<"请输入要借阅的图书书号："<<endl;
    string bookId;
    cin>>bookId;
    bool found1 = false;

    Book* book = FindBook(bookId);
    if (book != nullptr){
        cout<<"请确认图书信息："<<endl;
        book->PrintBook();
        found1 = true;
    }

    if (!found1){
        cout<<"未找到匹配的图书。"<<endl;   return;
    }
    
    cout<<"请输入读者号"<<endl;
    string readerId;
    cin>>readerId;
    bool found2 =true;    // BUG：永远是true
}
```

**修正后**：

```cpp
void LibrarySystem::BorrowBook(){
    string bookId,readerId,dateStr;

    // 1. 查找图书
    cout<<"请输入要借阅的图书书号："<<endl;
    cin>>bookId;
    Book* book = FindBook(bookId);
    if (book == nullptr){
        cout<<"错误：书号 "<<bookId<<" 不存在！"<<endl;
        return;
    }
    book->PrintBook();
    
    // 2. 查找读者
    cout<<"请输入读者号"<<endl;
    cin>>readerId;
    Reader* reader = FindReader(readerId);
    if (reader == nullptr){
        cout<<"错误：读者号 "<<readerId<<" 不存在！"<<endl;
        return;
    }

    // 3. 库存检查
    if (!book->CanBorrow()){
        cout<<"错误：该书已全部借出！"<<endl;   return;
    }

    // 4. 多态检查借阅上限（A4）
    int currentBorrowed = 0;
    for (auto& rec: records){
        if (rec.GetReaderId() == readerId && !rec.IsReturned()){
            currentBorrowed++;
        }
    }
    int maxCount = reader->GetMaxBorrowCount();
    if (currentBorrowed >= maxCount){
        cout<<"错误："<<reader->GetTypeName()
            <<"最多可借"<<maxCount<<"本，当前已借"
            <<currentBorrowed<<"本！"<<endl;
        return;
    }

    // 5. 日期校验
    cout<<"请输入借阅日期（格式：yyyy-mm-dd）：" ;
    cin>>dateStr;
    Date borrowDate = Date::FromString(dateStr);
    if (!borrowDate.IsValid()){
        cout<<"错误：日期格式不正确或不存在！"<<endl;
        return;
    }

    // 6. 创建借阅记录
    string recordId = GenerateRecordId();
    records.push_back(BorrowRecord(recordId, bookId, readerId, borrowDate));
    book->BorrowOne();

    cout<<"借书成功！"<<endl;
    cout<<"记录号："<<recordId<<"  应还期限："<<reader->GetMaxBorrowDays()<<"天"<<endl;
}
```

**设计要点**：用 guard clause（`if + return`）替代 `found1/found2` 标记；`reader->GetMaxBorrowCount()` 多态分派学生=5/教师=10；所有校验通过后才 `BorrowOne()`。

---

### 1.2 ReturnBook() — 还书函数

**问题**：只查到图书就停了，缺少读者查找、借阅记录匹配、日期校验、库存恢复。

**修正前（L703-717）**：

```cpp
void LibrarySystem::ReturnBook(){
    cout<<"请输入要归还的图书书号："<<endl;
    string id;
    cin>>id;
    bool found = false;

    Book* book = FindBook(id);
    if (book != nullptr){
        book->PrintBook();
        found = true;
    }
    if (!found){
        cout<<"未找到匹配的图书。"<<endl;   return;
    }
    // 后面全空！
}
```

**修正后**：

```cpp
void LibrarySystem::ReturnBook(){
    string bookId,readerId,dateStr;
    // 1. 查书
    cout<<"请输入要归还的图书书号："<<endl;
    cin>>bookId;
    Book* book = FindBook(bookId);
    if (book == nullptr){ cout<<"错误：书号 "<<bookId<<" 不存在！"<<endl; return; }

    // 2. 查读者
    cout<<"请输入读者号"<<endl;
    cin>>readerId;
    Reader* reader = FindReader(readerId);
    if (reader == nullptr){ cout<<"错误：读者号 "<<readerId<<" 不存在！"<<endl; return; }

    // 3. 匹配未归还记录（bookId + readerId + !IsReturned）
    BorrowRecord* targetRecord = nullptr;
    for (auto& rec: records){
        if (rec.GetBookId() == bookId && rec.GetReaderId() == readerId && !rec.IsReturned()){
            targetRecord = &rec; break;
        }
    }
    if (targetRecord == nullptr){
        cout<<"错误：未找到未归还借阅记录！"<<endl; return;
    }

    // 4. 日期校验 + 归还
    cout<<"请输入归还日期（格式：yyyy-mm-dd）：" ;
    cin>>dateStr;
    Date returnDate = Date::FromString(dateStr);
    if (!returnDate.IsValid()){ cout<<"错误：日期非法！"<<endl; return; }

    if (!targetRecord->SetReturnDate(returnDate)) return;
    book->ReturnOne();

    // 5. 显示结果（含逾期判断 A1）
    int days = targetRecord->GetBorrowDays();
    int maxDays = reader->GetMaxBorrowDays();
    if (targetRecord->IsOverdue(maxDays, returnDate)){
        cout<<"已逾期！逾期"<<(days - maxDays)<<"天"<<endl;
    }
}
```

---

## 第2天（06.23）— 文件读写与剩余功能

### 2.1 SaveToFile() — 保存数据

**问题**：空函数体。

**修正后**：

```cpp
void LibrarySystem::SaveToFile(){
    // 1. books.csv
    ofstream fbooks("books.csv");
    if (!fbooks.is_open()){ cout<<"错误：无法创建 books.csv！"<<endl; return; }
    fbooks<<"bookId,title,author,category,totalCount,availableCount,borrowTimes"<<endl;
    for (auto& book:books) fbooks<<book.ToCSV()<<endl;
    fbooks.close();

    // 2. readers.csv
    ofstream freaders("readers.csv");
    if (!freaders.is_open()){ cout<<"错误：无法创建 readers.csv！"<<endl; return; }
    freaders<<"readerId,name,phone,type"<<endl;
    for (auto* r: readers) freaders<<r->ToCSV()<<endl;
    freaders.close();

    // 3. borrow_records.csv
    ofstream frecords("borrow_records.csv");
    if (!frecords.is_open()){ cout<<"错误：无法创建 borrow_records.csv！"<<endl; return; }
    frecords<<"recordId,bookId,readerId,borrowDate,returnDate,status"<<endl;
    for (auto& rec: records) frecords<<rec.ToCSV()<<endl;
    frecords.close();

    cout<<"数据全部保存完毕！"<<endl;
}
```

**早期版本 bug**（已修复）：
- 文件名 `"book.csv"` 少 s → `"books.csv"`
- 表头拼写 `boorowTimes` → `borrowTimes`
- 检查流时写错对象 `!fbooks.is_open()` 检查的是 `freaders`（复制粘贴错误）

---

### 2.2 ShowReaderRecords() — 读者借阅记录查询（A3）

**问题**：声明了但无函数体。

**修正后**：

```cpp
void LibrarySystem::ShowReaderRecords(){
    string readerId;
    cout<<"请输入读者号："; cin>>readerId;
    Reader* reader = FindReader(readerId);
    if (reader == nullptr){ cout<<"错误：读者号不存在！"<<endl; return; }

    reader->PrintReader();
    cout<<"最大可借："<<reader->GetMaxBorrowCount()<<"本"<<endl;

    bool found = false;
    for (auto& rec: records){
        if (rec.GetReaderId() == readerId){
            rec.Print();
            found = true;
        }
    }
    if (!found) cout<<"暂无借阅记录。"<<endl;
}
```

---

### 2.3 LoadFromFile() — 从 CSV 载入数据（F8）

**问题**：声明了但无函数体。

**修正后（初版）**：

```cpp
void LibrarySystem::LoadFromFile(){
    for (Reader* r: readers) delete r;
    readers.clear(); books.clear(); records.clear();

    // 1. 载入 books.csv — 用 Book::FromCSV 逐行解析
    // 2. 载入 readers.csv — 按 type 列分流 new TeacherReader / StudentReader
    // 3. 载入 borrow_records.csv — 解析日期，恢复 nextRecordId
}
```

**早期版本 bug**（已修复）：
- `loadReaders++` 写在 while 循环外面，计数永远为 1
- `freaders.close()` 写在 else 外面，文件不存在时也执行

---

### 2.4 代码检查 — 多个 Bug 修复

| 位置 | 问题 | 修正前 | 修正后 |
|------|------|--------|--------|
| L714 | 借阅上限检查用错方法 | `rec.GetRecordId() == readerId` | `rec.GetReaderId() == readerId` |
| L823-828 | SortBooksByPopularity lambda 类型错误 | `[](Book* a, Book* b)` | `[](const Book& a, const Book& b)` |
| L897-900 | GenerateRecordId `nextRecordId≥1000` 时溢出 | `string(3-len, '0')` | `int zeros = (3-len>0)?(3-len):0;` |
| L379-381 | Reader::ToCSV 不含类型 | `readerId+","+name+","+phone` | 加上 `+","+GetTypeName()` |
| L216 | 位移代替除法风格不一致 | `y2 >> 2` | `y2 / 4` |

---

## 第3天（06.24）— 工程化与编译配置

### 3.1 LMS.h 头文件拆分

**问题**：全部代码在单文件 LMS.cpp，不符合文档要求"鼓励拆分为 .h 和 .cpp 文件"。

**修正**：创建 `LMS.h`，将所有类声明移入，`StudentReader`/`TeacherReader` 的 `return 5;` 等单行函数作为内联保留在头文件。`LMS.cpp` 加 `#include "LMS.h"`，只保留函数实现。

```cpp
// LMS.h（核心结构）
#ifndef LMS_H
#define LMS_H
#include <string>
#include <vector>
using namespace std;

class Date { ... };          // 声明
class Book { ... };
class Reader { ... };        // 抽象基类
class StudentReader: public Reader { ... };  // 内联实现
class TeacherReader: public Reader { ... };
class BorrowRecord { ... };
class LibrarySystem { ... };
#endif
```

### 3.2 Date 成员顺序警告修复

**问题**：LMS.h 声明 `day → month → year`，初始化列表 `year → month → day`，`-Wreorder` 警告。

**修正**：LMS.h 改为 `year → month → day`，与初始化列表顺序一致。

### 3.3 VSCode 编译配置

**问题**：F5 运行只编译 `main.cpp`，报 `undefined reference to LibrarySystem::~LibrarySystem()`。

**修正**：创建 `.vscode/tasks.json` + `.vscode/launch.json`：

```json
// tasks.json — 显式列出两个 cpp 文件（Windows 下 *.cpp 通配符不展开）
"args": ["-std=c++17", "-g",
    "${workspaceFolder}\\main.cpp",
    "${workspaceFolder}\\LMS.cpp",
    "-o", "${workspaceFolder}\\lms.exe"]

// launch.json — 关键行
"preLaunchTask": "C++ 编译全部文件"
```

---

## 第4天（06.25）— 输入流修复与校验补全

### 4.1 AddBook() 输入流 Bug

**问题1**：`cin>>id` 后遗留 `\n`，书名循环的 `getline` 首次读到空行，误报"书名不能为空"。

```cpp
// 修正：书号循环结束后加 cin.ignore()
}while(!flag);
cin.ignore();   // ← 吃掉 cin>>id 遗留的 \n
```

**问题2**：`getline` 后面的 `cin.ignore()` 多余 — `getline` 已消费 `\n`，再加 `ignore()` 会吞掉下一条输入的第一个字符。

```cpp
// 修正前
getline(cin, title);
cin.ignore();   // ← 删掉！会吞作者名首字

// 修正后
getline(cin, title);
// 直接继续
```

**问题3**：数字输入无 `cin.fail()` 保护，输入字母进入死循环。

```cpp
// 修正后
cin>>totalCount;
if (cin.fail()){
    cin.clear();
    cin.ignore(10000, '\n');
    cout<<"错误：请输入数字！"<<endl;
    flag = false;
}else if (totalCount <= 0){
    ...
}
```

### 4.2 InputReader() 手机号验证（F4 补全）

**问题**：文档要求"检查手机号基本格式"，但 AddReader 没做。

**修正前**：

```cpp
void Reader::InputReader(){
    cout<<"请输入读者号："; cin>>readerId; cin.ignore();
    cout<<"请输入姓名：";   getline(cin,name);
    cout<<"请输入电话：";   cin>>phone;
}
```

**修正后**：

```cpp
void Reader::InputReader(){
    cout<<"请输入读者号："; cin>>readerId; cin.ignore();
    cout<<"请输入姓名：";   getline(cin,name);
    bool phoneValid;
    do{
        phoneValid = true;
        cout<<"请输入电话（11位数字，以1开头）：";
        cin>>phone;
        if (phone.length() != 11 || phone[0] != '1'){
            phoneValid = false;
        }else{
            for (char c : phone){
                if (c < '0' || c > '9'){ phoneValid = false; break; }
            }
        }
        if (!phoneValid) cout<<"错误：手机号格式不正确，请重新输入！"<<endl;
    }while(!phoneValid);
}
```

**早期版本 bug**（已修复）：`&&` 误写为 `||` 导致首位非1的11位号也能通过。

---

### 4.3 Run() 退出流程优化

**修正前**：选0后 `break` 只跳出 switch，还要走"请按回车继续"+清屏才退出。

**修正后**：`case 0: cout<<"感谢使用，再见！"<<endl; return;` 直接退出 `Run()`。

---

## 第5天（06.26）— 文档对照与测试数据

### 5.1 SortBooksByPopularity() 改手写排序（A2 合规）

**问题**：用了 `std::sort`，文档要求"至少手写一种排序算法"。

**修正前**：

```cpp
sort(books.begin(), books.end(), [](const Book& a, const Book& b){
    return a.GetBorrowTimes() > b.GetBorrowTimes();
});
```

**修正后（选择排序）**：

```cpp
void LibrarySystem::SortBooksByPopularity(){
    for (size_t i = 0; i < books.size(); i++){
        size_t maxIdx = i;
        for (size_t j = i + 1; j < books.size(); j++){
            if (books[j].GetBorrowTimes() > books[maxIdx].GetBorrowTimes()){
                maxIdx = j;
            }
        }
        if (maxIdx != i) swap(books[i], books[maxIdx]);
    }
    cout<<"图书已按照热门程度排序："<<endl;
    DisplayAllBooks();
}
```

---

### 5.2 LoadFromFile() 异常处理完善（F8 补全）

**问题**：文档要求六种校验，初版只做了"文件不存在"和"空行"。

**修正后**：逐行 `try-catch` + 行号追踪，覆盖全部六种：

| 校验项 | 处理方式 |
|--------|----------|
| 文件不存在 | 警告，跳过载入 |
| 空行 | `continue` |
| 字段/格式错误 | `catch (...)` 打印行号和内容 |
| **主键重复** | `FindBook(id) != nullptr` → 跳过 |
| **关联错误** | `FindBook(bid)==nullptr` 或 `FindReader(readerId)==nullptr` → 跳过 |
| **库存/日期非法** | 校验后 `continue` |

```cpp
// 示例：borrow_records 校验片段
// 主键重复
if (FindRecord(rid) != nullptr){
    cout<<"警告：borrow_records.csv 第"<<lineNum<<"行 记录号重复，已跳过。"<<endl;
    continue;
}
// 关联错误
if (FindBook(bid) == nullptr){
    cout<<"警告：borrow_records.csv 第"<<lineNum<<"行 bookId("<<bid<<")不存在，已跳过。"<<endl;
    continue;
}
// 日期非法
if (!borrowDate.IsValid()){
    cout<<"警告：borrow_records.csv 第"<<lineNum<<"行 借阅日期非法，已跳过。"<<endl;
    continue;
}
```

---

### 5.3 测试数据生成

按照文档第六节格式，生成 3 个 CSV 文件（8 本图书、6 位读者、15+1 条借阅记录），覆盖：

- **正常**：借书/还书完整流程
- **边界**：库存为0（数据结构）、当天归还（BR007）、从未借阅（大学英语）
- **超期**：学生超期（BR010: 40>30天）、教师超期（BR012: 66>60天）
- **多态**：学生(上限5/期限30)、教师(上限10/期限60)

---

## 本周改动汇总

| 类别 | 数量 | 涉及函数/文件 |
|------|------|-------------|
| 新增实现 | 4 | BorrowBook, ReturnBook, SaveToFile, ShowReaderRecords, LoadFromFile |
| Bug 修复 | 11 | GetReaderId, GenerateRecordId, SaveToFile 文件名/表头, AddBook 输入流, 手机号验证 &&, 解析两次, 文件名拼写 borrowed→borrow |
| 文档合规 | 3 | SortBooksByPopularity 手写排序, LoadFromFile 六种校验, 手机号格式 |
| 工程化 | 3 | LMS.h 拆分, Date 声明顺序, VSCode tasks+launch |
| 测试数据 | 3 文件 | books.csv(8), readers.csv(6), borrow_records.csv(16) |

---

## 当前文档符合性状态

| 编号 | 功能 | 状态 |
|------|------|------|
| F1 | 添加图书（重复/空名/册数校验） | ✅ |
| F2 | 显示所有图书 | ✅ |
| F3 | 查询图书（按书号/书名关键字） | ✅ |
| F4 | 添加读者（编号重复+手机号格式） | ✅ |
| F5 | 借书（存在/库存/上限/日期/记录） | ✅ |
| F6 | 还书（库存/状态/天数/逾期） | ✅ |
| F7 | 保存数据（3个CSV） | ✅ |
| F8 | 读取数据（6种校验+行号+异常跳过） | ✅ |
| A1 | 逾期判断 | ✅ |
| A2 | 热门图书排序（手写选择排序） | ✅ |
| A3 | 读者借阅记录查询 | ✅ |
| A4 | 多态借阅规则（Student/Teacher） | ✅ |
