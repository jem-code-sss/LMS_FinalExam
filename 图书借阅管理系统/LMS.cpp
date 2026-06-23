#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "LMS.h"

using namespace std;

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
    int Days2 = 365 * y2 + (y2 / 4) - y2 / 100 + y2 / 400 + (153 * m2 - 457) / 5 + d2 - 306;

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
    if (s.length() < 10) return Date(0,0,0);
    try{
        int y = stoi(s.substr(0,4));
        int m = stoi(s.substr(5,2));
        int d = stoi(s.substr(8,2));
        if (IsValid(y,m,d)) return Date(y,m,d);
    } catch (...){}
    return Date(0,0,0);
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

Book::Book(string id, string t, string a, string c, int total,int avail, int times)
: bookId(id), title(t), author(a), category(c), availableCount(avail), totalCount(total), borrowTimes(times) {}

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
    string totalStr, availStr, timesStr;
    getline(ss,id,',');
    getline(ss,t,',');
    getline(ss,a,',');
    getline(ss,c,',');
    getline(ss,totalStr,',');
    getline(ss,availStr,',');
    getline(ss,timesStr,',');

    try{
        int total = stoi(totalStr);
        int avail = stoi(availStr);
        int times = stoi(timesStr);
        return Book(id, t, a, c, total, avail, times);
    }catch(...){
        return Book("ERROR","","","",0,0,0);
    }
    
}

//-------------------------------------------------
//-------------------Reader成员函数-----------------
Reader::Reader(string id,string n, string tel):readerId(id), name(n), phone(tel) {}
void Reader::InputReader(){
    cout<<"请输入读者号：";     cin>>readerId;  cin.ignore();
    cout<<"请输入姓名：";       getline(cin,name);
    cout<<"请输入电话：";      cin>>phone;
}

void Reader::PrintReader() const{
    cout<<"读者类型："<<GetTypeName()<<endl;
    cout<<"读者号："<<GetReaderId()<<endl;
    cout<<"姓名:"<<GetName()<<endl;
    cout<<"电话："<<GetPhone()<<endl;
}

string Reader::ToCSV() const{
    return readerId + "," + name + "," + phone + "," + GetTypeName();
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
BorrowRecord::BorrowRecord (string rid, string bid, string rId, const Date& bDate)
: recordId(rid), bookId(bid),readerId(rId),
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
    if (hasReturnDate) return borrowDate.DaysBetween(returnDate);
    return 0;
}

bool BorrowRecord::IsOverdue(int maxDays, const Date& today) const{
    if (hasReturnDate){
        return GetBorrowDays() > maxDays;   
    }else{
        return borrowDate.DaysBetween(today) > maxDays;
    }

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
        cin.ignore();

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
            case 0:     cout<<"感谢使用，再见！"<<endl; SaveToFile(); return;

            default:
            cout<<"无效选项，请重新输入(0-10)。"<<endl;
        }
        cout<<endl;
        cout<<"\n请按回车继续......"<<endl;
        cin.ignore();
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif
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
        }else if(id.length()!=13){
            cout<<"错误：书号位数为13位，请重新输入！"<<endl;
            flag = false;
        }
    }while(!flag);

    do{
        flag = true;
        cout<<"请输入书名：";
        getline(cin, title);
        cin.ignore();

        if (title.empty()){
        cout<<"错误：书名不能为空，添加失败！"<<endl;
        flag = false;
        }
    }while(!flag);

    
    cout<<"请输入作者:";
    getline(cin,author);

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

    if (type != 'T' && type != 't' && type != 'S' && type != 's'){
        cout<<"类型无效！"<<endl;   return;
    }

    Reader* r = nullptr;
    if (type == 'T' || type == 't'){
        r = new TeacherReader("","","");
    }else{
        r = new StudentReader("","","");
    }
    
    r->InputReader();
    if (FindReader(r->GetReaderId()) != nullptr){
        cout<<"错误：读者号"<<r->GetReaderId()<<"已存在！"<<endl;
        delete r;
        return;
    }

    readers.push_back(r);
    cout<<"添加成功！读者号："<<r->GetReaderId()<<"姓名："<<r->GetName()<<endl;
}

void LibrarySystem:: ShowReaderRecords(){
    string readerId;
    cout<<"请输入读者号：";
    cin>>readerId;

    Reader* reader = FindReader(readerId);
    if (reader == nullptr){
        cout<<"错误：读者号"<<readerId<<"不存在！"<<endl;
        return;
    }

    cout<<"\n====== 读者信息 ======"<<endl;
    reader->PrintReader();
    cout<<"最大可借："<<reader->GetMaxBorrowCount()<<"本"<<endl;
    cout<<"借阅期限："<<reader->GetMaxBorrowDays()<<"天"<<endl;

    cout<<"\n====== 借阅记录 ======"<<endl;
    bool found = false;
    for (auto& rec: records){
        if (rec.GetReaderId() == readerId){
            rec.Print();
            cout<<"----------------------------"<<endl;
            found = true;
        }
    }
    if (!found){
        cout<<"暂无借阅记录。"<<endl;
    }
}

void LibrarySystem::BorrowBook(){
    string bookId,readerId,dateStr;

//----------- 1. 查找图书 --------------
    cout<<"请输入要借阅的图书书号："<<endl;
    cin>>bookId;

    Book* book = FindBook(bookId);
    if (book == nullptr){
        cout<<"错误：书号 "<<bookId<<" 不存在！"<<endl;
        return;
    }
    cout<<"请确认图书信息："<<endl;
    book->PrintBook();
    
//----------- 2. 查找读者 --------------

    cout<<"请输入读者号"<<endl;
    cin>>readerId;
    Reader* reader = FindReader(readerId);
    if (reader == nullptr){
        cout<<"错误：读者号 "<<readerId<<" 不存在！"<<endl;
        return;
    }
    cout<<"读者："<<reader->GetTypeName()<<" "<<reader->GetName()<<endl;

//----------- 3. 库存检查 --------------

    if (!book->CanBorrow()){
        cout<<"错误：该书已全部借出！"<<endl;   return;
    }

//----------- 4. 检查借阅上限 --------------

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

//----------- 5. 日期输入与校验 --------------

    cout<<"请输入借阅日期（格式：yyyy-mm-dd）：" ;
    cin>>dateStr;

    Date borrowDate = Date::FromString(dateStr);
    if (!borrowDate.IsValid()){
        cout<<"错误：日期格式不正确或不存在！"<<endl;
        return;
    }

//----------- 6. 创建借阅记录 --------------

    string recordId = GenerateRecordId();
    records.push_back(BorrowRecord(recordId, bookId, readerId, borrowDate));
    book->BorrowOne();

    cout<<"借书成功！"<<endl;
    cout<<"记录号："<<recordId<<endl;
    cout<<"书号："<<bookId<<" 书名："<< book->GetTitle()<<endl;
    cout<<"借阅日期："<<borrowDate.ToString()<<endl;
    cout<<"应还期限："<<reader->GetMaxBorrowDays()<<"天"<<endl;
}

void LibrarySystem::ReturnBook(){
    string bookId,readerId,dateStr;
    cout<<"请输入要归还的图书书号："<<endl;
    cin>>bookId;

    Book* book = FindBook(bookId);
    if (book == nullptr){
        cout<<"错误：书号 "<<bookId<<" 不存在！"<<endl;
        return;
    }
    book->PrintBook();
    
//----------- 2. 查找读者 --------------

    cout<<"请输入读者号"<<endl;
    cin>>readerId;
    Reader* reader = FindReader(readerId);
    if (reader == nullptr){
        cout<<"错误：读者号 "<<readerId<<" 不存在！"<<endl;
        return;
    }

//----------- 3. 查找借阅记录 --------------
    BorrowRecord* targetRecord = nullptr;
    for (auto& rec: records){
        if (rec.GetBookId() == bookId &&
            rec.GetReaderId() == readerId &&
            !rec.IsReturned()){
                targetRecord = &rec;
                break;
            }
    }

    if (targetRecord == nullptr){
        cout<<"错误：未找到读者"<<reader->GetName()
            <<"对书号"<<bookId<<"的未归还借阅记录！"<<endl;
        return;
    }
    cout<<"借阅记录如下："<<endl;
    targetRecord->Print();

//----------- 4. 输入并校验归还日期 --------------

    cout<<"请输入归还日期（格式：yyyy-mm-dd）：" ;
    cin>>dateStr;

    Date returnDate = Date::FromString(dateStr);
    if (!returnDate.IsValid()){
        cout<<"错误：日期格式不正确或不存在！"<<endl;
        return;
    }

//----------- 5. 归还 --------------

    if (!targetRecord->SetReturnDate(returnDate)) return;
    book->ReturnOne();

//----------- 6. 显示 --------------
    int days = targetRecord->GetBorrowDays();
    int maxDays = reader->GetMaxBorrowDays();

    cout<<"还书成功！"<<endl;
    cout<<"记录号："<<targetRecord->GetRecordId()<<endl;
    cout<<"借阅天数："<<days<<"天"<<endl;

    if (targetRecord->IsOverdue(maxDays, returnDate)){
        cout<<"已逾期！逾期"<<(days - maxDays)<<"天"<<endl;
    }else{
        cout<<"剩余"<<(maxDays - days)<<"天"<<endl;
    }

}

void LibrarySystem::SortBooksByPopularity(){
    sort(books.begin() ,books.end(), [](const Book& a,const Book& b){
        return a.GetBorrowTimes() > b.GetBorrowTimes();
    });
    cout<<"图书已按照热门程度排序："<<endl;
    DisplayAllBooks();
}

void LibrarySystem::SaveToFile(){
//--------------1. books.csv ---------------
    ofstream fbooks("books.csv");
    if (!fbooks.is_open()){
        cout<<"错误：无法创建 books.csv！"<<endl;
        return;
    }
    fbooks<<"bookId,title,author,category,totalCount,availableCount,borrowTimes"<<endl;
    for (auto& book:books){
        fbooks<<book.ToCSV()<<endl;
    }
    fbooks.close();
    cout<<"已保存"<<books.size()<<"本图书到books.csv"<<endl;

//--------------2. readers.csv ---------------
    ofstream freaders("readers.csv");
    if (!freaders.is_open()){
        cout<<"错误：无法创建 readers.csv！"<<endl;
        return;
    }
    freaders<<"readerId,name,phone,type"<<endl;
    for (auto* r: readers){
        freaders<<r->ToCSV()<<endl;
    }
    freaders.close();
    cout<<"已保存"<<readers.size()<<"位读者到readers.csv"<<endl;

//--------------3. borrow_records.csv ---------------
    ofstream frecords("borrow_records.csv");
    if (!frecords.is_open()){
        cout<<"错误：无法创建 borrow_records.csv！"<<endl;
        return;
    }
    frecords<<"recordId,bookId,readerId,borrowDate,returnDate,status"<<endl;
    for (auto& rec: records){
        frecords<<rec.ToCSV()<<endl;
    }
    frecords.close();
    cout<<"已保存"<<records.size()<<"条借阅记录到borrow_records.csv"<<endl;

    cout<<"数据全部保存完毕！"<<endl;
}

void LibrarySystem:: LoadFromFile(){
    for (Reader* r: readers) delete r;
    readers.clear();
    books.clear();
    records.clear();

    string line;
    int loadBooks = 0, loadReaders = 0, loadRecords = 0;

//----------- 1. 载入 books.csv -----------
    ifstream fbooks("books.csv");
    if (!fbooks.is_open()){
        cout<<"警告：未找到 books.csv，跳过图书载入。"<<endl;
    }else{
        getline(fbooks,line);
        while (getline(fbooks,line)){
            if (line.empty()) continue;
            books.push_back(Book::FromCSV(line));
            loadBooks++;
        }
        fbooks.close();
        cout<<"已载入"<<loadBooks<<"本图书"<<endl;
    }

//----------- 2. 载入 readers.csv -----------
    ifstream freaders("readers.csv");
    if (!freaders.is_open()){
        cout<<"警告：未找到 readers.csv，跳过读者载入。"<<endl;
    }else{
        getline(freaders,line);
        while (getline(freaders,line)){
            if (line.empty()) continue;

            stringstream ss(line);
            string id, name, phone, type;
            getline(ss,id,',');
            getline(ss,name,',');
            getline(ss,phone,',');
            getline(ss,type,',');

            if (type == "教师"){
                readers.push_back(new TeacherReader(id, name, phone));
            }else{
                readers.push_back(new StudentReader(id, name, phone));
            }
            loadReaders++;
        }
        freaders.close();
        cout<<"已载入"<<loadReaders<<"位读者"<<endl;
    }

//----------- 3. 载入 borrow_records.csv -----------
    ifstream frecords("borrow_records.csv");
    if (!frecords.is_open()){
        cout<<"警告：未找到 borrow_records.csv，跳过记录载入。"<<endl;
    }else{
        getline(frecords,line);
        int maxId = 0;
        while (getline(frecords,line)){
            if (line.empty()) continue;

            stringstream ss(line);
            string rid, bid, readerId, borrowStr, returnStr, status;
            getline(ss,rid,',');
            getline(ss,bid,',');
            getline(ss,readerId,',');
            getline(ss,borrowStr,',');
            getline(ss,returnStr,',');
            getline(ss,status,',');

            Date borrowDate = Date::FromString(borrowStr);
            BorrowRecord rec(rid, bid, readerId, borrowDate);

            if (returnStr != "NONE"){
                Date retDate = Date::FromString(returnStr);
                rec.SetReturnDate(retDate);
            }

            records.push_back(rec);

            string numStr = rid.substr(2);
            int num = stoi(numStr);
            if (num > maxId) maxId = num;

            loadRecords++;
        }
        frecords.close();
        nextRecordId = maxId + 1;
        cout<<"已载入"<<loadRecords<<"条借阅记录"<<endl;
    }

    cout<<"数据载入完毕！"<<endl;
}

Book* LibrarySystem::FindBook(const string& bookId){
    for(auto& book: books){
        if(book.GetBookId() == bookId) return &book;
    }
    return nullptr;
}

Reader* LibrarySystem:: FindReader(const string& readerId){
    for(auto& r: readers){
        if(r->GetReaderId() == readerId) return r;
    }
    return nullptr;
}

string LibrarySystem:: GenerateRecordId(){
    int len = (int)to_string(nextRecordId).length();
    int zeros = (3 - len > 0) ? (3 - len) : 0;
    string id = "BR" + string(zeros,'0') + to_string(nextRecordId);
    nextRecordId++;
    return id;
}