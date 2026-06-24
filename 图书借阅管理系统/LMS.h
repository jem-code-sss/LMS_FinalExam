#ifndef LMS_H
#define LMS_H

#include <string>
#include <vector>
using namespace std;

class Date{
private:
    int year;
    int month;
    int day;

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
    Book(string id, string t, string a, string c, int total,int avail,int times = 0);
    
    bool CanBorrow() const;
    void BorrowOne();
    void ReturnOne();

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
    BorrowRecord(string rid, string bid, string rId, const Date& bDate);

    bool SetReturnDate(const Date& d);

    int GetBorrowDays() const;
    bool IsOverdue(int maxDays, const Date& today) const;

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

#endif