#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

using namespace std;

class Date{
private:
    int day;
    int month;
    int year;

public:
    Date();
    Date(int y, int m, int d);

    static bool IsValid(int y, int m, int d);
    bool IsValid() const;

    int DaysBetween(const Date& other) const;

    bool operator<(const Date& other) const;
    bool operator<=(const Date& other) const;
    bool operator==(const Date& other) const;

    string ToString() const;
    static Date FromString(const string& s);

    int GetDay() const;
    int GetMonth() const;
    int GetYear() const;
};

class Book{
private:
    string bookId;
    string title;
    string author;
    string category;
    int availableCount;
    int totalCount;
    int borrowTimes;

public:
    Book(string id, string t, string a, string c, int total,int avail);
    
    bool CanBorrow() const;
    void BorrowOne();
    void ReturnOne();

    void InputBook();
    void PrintBook() const;

    string GetBookId() const;
    string GetTitle() const;
    string GetAuthor() const;
    int GetAvailableCount() const;
    int GetTotalCount() const;
    int GetBorrowTimes() const;

    string ToCSV() const;
    static Book FromCSV(const string& line);

};

class Reader{
private:
    string readerId;
    string name;
    string phone;

public:
    Reader(string id, string n, string tel);
    virtual ~Reader() = default;

    virtual int GetMaxBorrowCount() const = 0;
    virtual int GetMaxBorrowDays() const = 0;
    virtual string GetTypeName() const = 0;

    void InputReader();
    void PrintReader() const;
    string ToCSV() const;

    string GetReaderId() const;
    string GetName() const;
    string GetPhone() const;
};

class StudentReader: public Reader{
public:
    StudentReader(string id, string n, string tel);

    int GetMaxBorrowCount() const override { return 5; }
    int GetMaxBorrowDays() const override { return 30; }
    string GetTypeName() const override { return "学生"; }

};

class TeacherReader: public Reader{
public:
    TeacherReader(string id, string n, string tel);

    int GetMaxBorrowCount() const override { return 10; }
    int GetMaxBorrowDays() const override { return 60; }
    string GetTypeName() const override { return "教师"; }

};

class BorrowRecord{
private:
    string recordId;
    string bookId;
    string readerId;
    Date borrowDate;
    Date returnDate;
    string status;
    bool hasReturnDate;

public:
    BorrowRecord(string rid, string bid, string readerId, const Date& bDate);

    bool SetReturnDate(const Date& d);

    int GetBorrowDays() const;
    bool IsOverdue(int maxDays) const;

    string GetRecordId() const;
    string GetBookId() const;
    string GetReaderId() const;
    string GetStatus() const;
    string GetBorrowDate() const;
    string GetReturnDate() const;
    bool HasReturnDate() const;
    bool IsReturned() const;

    string ToCSV() const;
    void Print() const;

};

class LibrarySystem{
private:
    vector<Book> books;
    vector<Reader*> readers;
    vector<BorrowRecord> records;
    int nextRecordId;

public:
    LibrarySystem();
    ~LibrarySystem();

    LibrarySystem(const LibrarySystem&) = delete;
    LibrarySystem& operator=(const LibrarySystem&) = delete;

    void ShowMenu();
    void Run();

    void AddBook();
    void SearchBook();
    void DisplayAllBooks();

    void AddReader();
    void ShowReaderRecords();

    void BorrowBook();
    void ReturnBook();

    void SortBooksByPopularity();

    void SaveToFile();
    void LoadFromFile();

    Book* FindBook(const string& bookId);
    Reader* FindReader(const string& readerId);
    BorrowRecord* FindRecord(const string& recordId);
    string GenerateRecordId();
};

//-------------------------------------------------
// ------------Date成员函数-------------------------
Date::Date():year(2000),month(01),day(01){}
Date::Date(int y, int m, int d):year(y),month(m),day(d){}

bool Date::IsValid (int year, int month, int day){
    int MonthDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0)){
        MonthDays[1] = 29;
    }

    if ( month > 0 && month <= 12 && day > 0 && day <= MonthDays[month-1] ){
        return true;
    }else{
        return false;
    }
}

bool Date::IsValid () const{
    return IsValid(year, month, day);
}

int Date:: DaysBetween(const Date& other) const{
    int y1 = GetYear();
    int m1 = GetMonth();
    int d1 = GetDay();
    if (m1 < 3)	y1--, m1 += 12;
	int Days1 = 365 * y1 + (y1 / 4) - y1 / 100 + y1 / 400 + (153 * m1 - 457) / 5 + d1 - 306;

    int y2 = other.GetYear();
    int m2 = other.GetMonth();
    int d2 = other.GetDay();
    if (m2 < 3) y2--, m2 += 12;
    int Days2 = 365 * y2 + (y2 >> 2) - y2 / 100 + y2 / 400 + (153 * m2 - 457) / 5 + d2 - 306;

    return Days2 - Days1;
}

bool Date:: operator<(const Date& other) const{
    if( DaysBetween(other) > 0 ){
        return true;
    }else{
        return false;
    }
}

bool Date:: operator<=(const Date& other) const{
    if( DaysBetween(other) >= 0 ){
        return true;
    } else {
        return false;
    }
}

bool Date:: operator==(const Date& other) const{
    if( DaysBetween(other) == 0 ){
        return true;
    } else {
        return false;
    }
}

string Date::ToString() const{
    string y = to_string(year);
    string m = (month >= 10 ? "" : "0") + to_string(month);
    string d = (day >= 10 ? "" : "0") + to_string(day);
    return y + "-" + m + "-" + d;
}

Date Date::FromString(const string& s){
    if (s.length() < 10) return Date();
    int y = stoi(s.substr(0,4));
    int m = stoi(s.substr(5,2));
    int d = stoi(s.substr(8,2));
    return Date(y,m,d);
}

int Date:: GetYear() const{
    return year;
}

int Date:: GetMonth() const{
    return month;
}

int Date:: GetDay() const{
    return day;
}

//-----------------------------------------
//---------------Book成员函数---------------

Book::Book(string id, string t, string a, string c, int total,int avail)
: bookId(id), title(t), author(a), category(c), availableCount(avail), totalCount(total), borrowTimes(0) {}

bool Book:: CanBorrow() const{
    return availableCount > 0;
}

void Book:: BorrowOne(){
    if ( !CanBorrow() ){
        cout<<"已全部借出！"<<endl;
        return;
    }
    availableCount--;
    borrowTimes++;
}

void Book::ReturnOne() {
    availableCount++;
}

void Book::InputBook() {
    cout<<"请输入书号：";       cin>>bookId;
    cout<<"请输入书名：";       cin>>title;
    cout<<"请输入作者：";       cin>>author;
    cout<<"请输入图书类别：";   cin>>category;
    cout<<"请输入总册数：";     cin>>totalCount;
    availableCount = totalCount;
    borrowTimes = 0;
}

void Book::PrintBook() const{
    cout<<"书号："<<bookId<<endl;
    cout<<"书名："<<title<<endl;
    cout<<"作者："<<author<<endl;
    cout<<"类别:"<<category<<endl;
    cout<<"总册数："<<totalCount<<endl;
    cout<<"可借册数："<<availableCount<<endl;
    cout<<"借阅次数："<<borrowTimes<<endl;
}

string Book:: GetBookId() const{
    return bookId;
}

string Book:: GetTitle() const{
    return title;
}

string Book:: GetAuthor() const{
    return author;
}

int Book:: GetAvailableCount() const{
    return availableCount;
}

int Book:: GetTotalCount() const{
    return totalCount;
}

int Book:: GetBorrowTimes() const{
    return borrowTimes;
}

string Book:: ToCSV() const{
    return bookId + "," + title + "," + author + "," + category + ","
            + to_string(totalCount) + "," + to_string(availableCount) + ","
            + to_string(borrowTimes);
}

Book Book::FromCSV(const string& line) {
    stringstream ss(line);
    string id, t, a, c;
    int total, avail, times;
    getline(ss,field,',');
    return Book(id, t, a, c, total, avail);
}

//-------------------------------------------------
//-------------------Reader成员函数-----------------
Reader::Reader(string id,string n, string tel):readerId(id), name(n), phone(tel) {}
void Reader::InputReader(){
    cout<<"请输入读者号：";     cin>>readerId;
    cout<<"请输入姓名：";       cin>>name;
    cout<<"请输入电话：";      cin>>phone;
}

void Reader::PrintReader() const{
    cout<<"读者类型："<<GetTypeName()<<endl;
    cout<<"读者号："<<GetReaderId()<<endl;
    cout<<"姓名:"<<GetName()<<endl;
    cout<<"电话："<<GetPhone()<<endl;
}

string Reader::ToCSV() const{
    return readerId + "," + name + "," + phone;
}

string Reader::GetReaderId() const{
    return readerId;
}

string Reader::GetName() const{
    return name;
}

string Reader::GetPhone() const{
    return phone;
}

//-------------------------------------------
//------------StudentReader成员函数----------
StudentReader::StudentReader(string id, string n, string tel)
:Reader(id, n, tel) {}

//-------------------------------------------
//------------TeacherReader成员函数------------
TeacherReader::TeacherReader(string id, string n, string tel)
:Reader(id, n, tel) {}

//------------------------------------------
//----------BorrowRecord成员函数-------------
BorrowRecord::BorrowRecord (string rid, string bid, string readerId, const Date& bDate)
: recordId(rid), bookId(bid),readerId(readerId),
borrowDate(bDate), status("BORROWED"), hasReturnDate(false) {}

bool BorrowRecord::SetReturnDate(const Date& d){
    if (d < borrowDate){
        cout<<"错误归还日期不得早于借阅日期！"<<endl;
        return false;
    }
    returnDate = d;
    status = "RETURNED";
    hasReturnDate = true;
    return true;
}

int BorrowRecord::GetBorrowDays() const{
    if (HasReturnDate) return borrowDate.DaysBetween(returnDate);
    return 0;
}

bool BorrowRecord::IsOverdue(int maxDays) const{
    if (!hasReturnDate) return false;
    return GetBorrowDays() > maxDays;
}

string BorrowRecord::GetRecordId() const{
    return recordId;
}

string BorrowRecord::GetBookId() const{
    return bookId;
}

string BorrowRecord::GetReaderId() const{
    return readerId;
}

string BorrowRecord::GetStatus() const{
    return status;
}

string BorrowRecord::GetBorrowDate() const{
    return borrowDate.ToString();
}

string BorrowRecord::GetReturnDate() const{
    if (!hasReturnDate) return "NONE";
    return returnDate.ToString();
}

bool BorrowRecord::HasReturnDate() const{
    return hasReturnDate;
}   

bool BorrowRecord::IsReturned() const{
    return status == "RETURNED";
}

string BorrowRecord::ToCSV()const{
    return recordId + "," + bookId + ","+ readerId + "," + GetBorrowDate()
            + "," + GetReturnDate() + "," + GetStatus();
}

void BorrowRecord::Print() const{
    cout<<"借阅编号："<<recordId<<endl;
    cout<<"书籍编号："<<bookId<<endl;
    cout<<"借阅读者号："<<readerId<<endl;
    cout<<"借阅日期："<<GetBorrowDate()<<endl;
    cout<<"归还日期："<<GetReturnDate()<<endl;
    cout<<"借阅状态："<<status<<endl;
    cout<<"借阅天数："<<GetBorrowDays()<<"天"<<endl;
}

//---------------------------------------------------------------
//--------------------LibrarySystem成员函数-----------------------
LibrarySystem::LibrarySystem(): nextRecordId(1) {}

LibrarySystem::~LibrarySystem(){
    for (Reader* r : readers){
        delete r;
    }
    readers.clear();
}

void LibrarySystem::ShowMenu(){
    cout<<"\n==============================="<<endl;
    cout<<"     欢迎使用图书借阅管理系统"<<endl;
    cout<<"1. 添加图书"<<endl;
    cout<<"2. 显示所有图书"<<endl;
    cout<<"3. 查询图书"<<endl;
    cout<<"4. 添加读者"<<endl;
    cout<<"5. 借书"<<endl;
    cout<<"6. 还书"<<endl;
    cout<<"7. 热门图书排序"<<endl;
    cout<<"8. 读者借阅记录查询"<<endl;
    cout<<"9. 保存数据到文件"<<endl;
    cout<<"10.从文件中载入数据"<<endl;
    cout<<"0. 退出系统"<<endl;
    cout<<"\n==============================="<<endl;
    cout<<"请输入选项:(0-10):";
}

void LibrarySystem::Run(){
    int choice;
    while (true){
        ShowMenu();
        cin>>choice;

        if (cin.fail()){
            cin.clear();
            cin.ignore(10000,'\n');
            cout<<"输入无效，请输入数字 0-10。"<<endl;
            continue;
        }
        switch (choice)
        {
            case 1:     AddBook();                  break;
            case 2:     DisplayAllBooks();          break;
            case 3:     SearchBook();               break;
            case 4:     AddReader();                break;
            case 5:     BorrowBook();               break;
            case 6:     ReturnBook();               break;
            case 7:     SortBooksByPopularity();    break;
            case 8:     ShowReaderRecords();        break;
            case 9:     SaveToFile();               break;
            case 10:    LoadFromFile();             break;
            case 0:     cout<<"感谢使用，再见！"<<endl; return;

            default:
            cout<<"无效选项，请重新输入(0-10)。"<<endl;
        }
    }
}

void LibrarySystem::AddBook(){
    string id, title, author, category;
    int totalCount, availableCount;
    bool flag;
    do{
        flag = true;
        cout<<"请输入书号：";
        cin>>id;

        if (FindBook(id) != nullptr){
        cout<<"错误：书号"<<id<<"已存在，添加失败！"<<endl;
        flag = false;
        }
    }while(!flag);

    do{
        flag = true;
        cout<<"请输入书名：";
        cin>>title;

        if (title.empty()){
        cout<<"错误：书名不能为空，添加失败！"<<endl;
        flag = false;
        }
    }while(!flag);

    
    cout<<"请输入作者:";
    cin>>author;

    cout<<"请输入图书类别：";
    cin>>category;

    do{
        flag = true;
        cout<<"请输入总册数：";
        cin>>totalCount;
        if (totalCount<=0){
        cout<<"错误：总册数必须大于0，添加失败！"<<endl;
        flag = false;
        }
    }while (!flag);
    
    do{
        flag = true;
        cout<<"请输入可借册数：";
        cin>>availableCount;
        if (availableCount > totalCount) {
        cout << "错误：可借册数（" << availableCount
             << "）不能大于总册数（" << totalCount << "），添加失败！" << endl;
        flag = false;
        }
    }while(!flag); 
    books.push_back(Book(id, title, author, category, totalCount, availableCount));
    cout<<"添加成功！书号："<<id<<"书名："<<title<<endl;
}

void LibrarySystem::SearchBook(){
    int choice;
    cout<<"请选择查询方式："<<endl;
    cout<<"1 - 按书号查询"<<endl;
    cout<<"2 - 按书名查询"<<endl;
    cout<<"请输入：";
    cin>>choice;

    if (cin.fail()|| (choice != 1 && choice != 2)){
        cin.clear();
        cin.ignore(10000,'\n');
        cout<<"输入无效，请选择 1 或 2 。"<<endl;
        return;
    }

    bool found = false;
    
    if (choice == 1){
        string id;
        cout<<"请输入书号：";
        cin>>id;

        Book* book = FindBook(id);
        if (book != nullptr){
            book->PrintBook();
            found = true;
        }
    }else{
        string keyword;
        cout<<"请输入书名关键字：";
        cin>>keyword;

        for(auto& book: books){
            if(book.GetTitle().find(keyword) != string::npos){
                book.PrintBook();
                cout<<"--------------------------"<<endl;
                found = true;
            }
        }
    }

    if (!found){
        cout<<"未找到匹配的图书。"<<endl;
    }
}

void LibrarySystem::DisplayAllBooks(){
    if(books.empty()){
        cout<<"暂无图书数据。"<<endl;
        return;
    }

    cout<<"\n============== 图书列表（共"<<books.size()<<"本） =============="<<endl;
    for(auto& book : books){
        book.PrintBook();
        cout<<"------------------------------------"<<endl;
    }
}

void LibrarySystem::AddReader(){
    char type;
    cout<<"请输入要输入的读者信息类型（T-教师，S-学生）：";
    cin>>type;
    cin.ignore();

    if (type == 'T' || type == 't'){
        Reader* r = new TeacherReader("","","");
        r->InputReader();
        
        readers.push_back(r);
    }else if(type == 'S' || type == 's'){
        Reader* r = new StudentReader("","","");
        r->InputReader();
        readers.push_back(r);
    }else{
        cout<<"类型无效";return;
    }

}

void LibrarySystem::BorrowBook(){

}

void LibrarySystem::ReturnBook(){

}

void LibrarySystem::SortBooksByPopularity(){

}

void LibrarySystem::SaveToFile(){
    
}

Book* LibrarySystem::FindBook(const string& bookId){

}

Reader* LibrarySystem:: FindReader(const string& readerId){

}

BorrowRecord* LibrarySystem::FindRecord(const string& recordId){

}

string LibrarySystem:: GenerateRecordId(){

}