#include <iostream>
#include <vector>
#include <string>
using namespace std;

class Date {
private:
    int day;
    int month;
    int year;

public:
    Date();
    Date(int y, int m, int d);
    static bool IsValid(int y, int m, int d);
    bool IsValid() const;
    int BorrowedDays();
};

class Book {
public:
    string bookid;
    string title;
    string author;
    string category;
    int availableCount;
    int totalcount;
    int borrowTimes = 0;
    Book(string id, string t, string a, string c, int ac, int tc ,int bt ) : bookid(id), title(t), author(a), category(c), availableCount(ac), totalcount(tc), borrowTimes(bt) {}
    
    void SetBookData(string id, string t, string a, string c, int ac, int tc ,int bt );
    void InputBookData();
    void PrintBookData();
};

class Reader {
public:
    string readerid;
    string name;
    string phone;
    Reader(): readerid(""), name(""), phone("") {}
    virtual ~Reader() = default;

    void SetReaderData(string id, string n, string tel);
    void InputReaderData();
    void PrintReaderData();

    virtual bool IsTeacher() const { return false; }
    virtual bool IsStudent() const { return false; }

};

class StudentReader: public Reader {

};

class TeacherReader: public Reader {

};

class BorrowRecord{
private:
    string recordId;
    string bookid;
    string readerid;
    Date borrowDate;
    Date returnDate;
    string status;
    bool hasReturnDate;

public:
    BorrowRecord(string rid, string bid, string rderid, Date bDate);

    bool SetReturnDate(Date d);

    string GetBookId() const;
    string GetReaderId() const;
    string GetStatus() const;
    bool IsReturned() const;
    int GetBorrrowDays() const;
    bool IsOverdue(int maxDays) const;

    string ToCSV() const;
    void Print() const;

};

class LibrarySystem {
private:

public:
    void ShowMenu();
    void AddBook();
    void BookSearch();
    void DisplayAllBooks();
    void AddReader();
    void Borrow();
    void Return();
    void SortBookByPopular();
    void SaveToFile();
    void LoadFromFile();
};