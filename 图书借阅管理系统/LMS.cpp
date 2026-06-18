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

    void ShowMenu();
    void Run();

    void AddBook();
    void SearchBook();
    void DisplayAllBooks();

    void AddReader();

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
	int Days1 = 365 * y1 + (y1 >> 2) - y1 / 100 + y1 / 400 + (153 * m1 - 457) / 5 + d1 - 306;

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
: bookId(id), title(t), author(a), category(c), availableCount(avail), totalCount(total) {}

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
    return Book(id, t, a, c, total, avail);
}

//-------------------------------------------------
//-------------------Reader成员函数-----------------
Reader::Reader(string id,string n, string tel):readerId(id), name(n), phone(tel) {}
void Reader::InputReader(){
    cout<<"请输入读者号：";     cin>>readerId;
    cout<<"请输入姓名：";       cin>>name;
    cout<<" 请输入电话：";      cin>>phone;
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
    if (status == "RETURNED") return borrowDate.DaysBetween(returnDate);
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
    if (status == "RETURNED"){
        return true;
    }else{
        return false;
    }
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
