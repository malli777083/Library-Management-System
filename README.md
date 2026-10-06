/*
    ===========================================================
     LIBRARY MANAGEMENT SYSTEM (Console-based, C++)
    ===========================================================
    Concepts demonstrated:
      - Object-Oriented Programming: separate Book, Member and
        IssueRecord classes, each encapsulating their own data
      - File Handling: three binary files give persistent,
        independent storage for books, members and issue
        records, so all data survives across program runs

    Features:
      1. Add Book
      2. Add Member
      3. Issue Book
      4. Return Book
      5. Search Book (by Title or Author)
      6. Display All Books
      7. Display All Members
      8. Display All Issue Records
      9. Exit
    ===========================================================
*/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <limits>

using namespace std;

// ===========================================================
// Book class
// ===========================================================
class Book {
private:
    int bookId;
    char title[60];
    char author[40];
    int totalCopies;
    int availableCopies;

public:
    void addBook(int id) {
        bookId = id;
        cout << "Enter Title: ";
        cin.getline(title, 60);
        cout << "Enter Author: ";
        cin.getline(author, 40);

        cout << "Enter Number of Copies: ";
        while (!(cin >> totalCopies) || totalCopies <= 0) {
            cout << "Invalid input. Enter a positive number of copies: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        availableCopies = totalCopies;
    }

    int getId() const { return bookId; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    int getTotalCopies() const { return totalCopies; }
    int getAvailableCopies() const { return availableCopies; }

    bool isAvailable() const { return availableCopies > 0; }
    void issueCopy() { if (availableCopies > 0) availableCopies--; }
    void returnCopy() { if (availableCopies < totalCopies) availableCopies++; }

    bool matches(const string& keyword) const {
        string t = title, a = author, k = keyword;
        for (auto& c : t) c = tolower(c);
        for (auto& c : a) c = tolower(c);
        for (auto& c : k) c = tolower(c);
        return t.find(k) != string::npos || a.find(k) != string::npos;
    }

    void display() const {
        cout << left << setw(6) << bookId
             << setw(30) << title
             << setw(20) << author
             << setw(8) << totalCopies
             << setw(10) << availableCopies << "\n";
    }
};

// ===========================================================
// Member class
// ===========================================================
class Member {
private:
    int memberId;
    char name[40];
    char phone[15];

public:
    void addMember(int id) {
        memberId = id;
        cout << "Enter Name: ";
        cin.getline(name, 40);
        cout << "Enter Phone Number: ";
        cin.getline(phone, 15);
    }

    int getId() const { return memberId; }
    string getName() const { return name; }
    string getPhone() const { return phone; }

    void display() const {
        cout << left << setw(6) << memberId
             << setw(25) << name
             << setw(15) << phone << "\n";
    }
};

// ===========================================================
// IssueRecord class — links a Book to a Member
// ===========================================================
class IssueRecord {
private:
    int recordId;
    int bookId;
    int memberId;
    char status[10]; // "Issued" or "Returned"

public:
    void create(int rId, int bId, int mId) {
        recordId = rId;
        bookId = bId;
        memberId = mId;
        strcpy(status, "Issued");
    }

    int getRecordId() const { return recordId; }
    int getBookId() const { return bookId; }
    int getMemberId() const { return memberId; }
    string getStatus() const { return status; }
    bool isIssued() const { return strcmp(status, "Issued") == 0; }

    void markReturned() { strcpy(status, "Returned"); }

    void display() const {
        cout << left << setw(10) << recordId
             << setw(10) << bookId
             << setw(10) << memberId
             << setw(10) << status << "\n";
    }
};

const char* BOOKS_FILE = "books.dat";
const char* MEMBERS_FILE = "members.dat";
const char* ISSUES_FILE = "issues.dat";

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

int getNextId(const char* filename, size_t recordSize) {
    ifstream fin(filename, ios::binary);
    int count = 0;
    char buffer[200];
    while (fin.read(buffer, recordSize)) count++;
    fin.close();
    return 1 + count;
}

bool findBook(int bookId, Book& book, streampos& pos) {
    ifstream fin(BOOKS_FILE, ios::binary);
    if (!fin) return false;
    while (fin.read(reinterpret_cast<char*>(&book), sizeof(Book))) {
        if (book.getId() == bookId) {
            pos = fin.tellg();
            pos -= static_cast<streamoff>(sizeof(Book));
            fin.close();
            return true;
        }
    }
    fin.close();
    return false;
}

bool findMember(int memberId, Member& member) {
    ifstream fin(MEMBERS_FILE, ios::binary);
    if (!fin) return false;
    while (fin.read(reinterpret_cast<char*>(&member), sizeof(Member))) {
        if (member.getId() == memberId) {
            fin.close();
            return true;
        }
    }
    fin.close();
    return false;
}

void rewriteBook(const Book& book, streampos pos) {
    fstream file(BOOKS_FILE, ios::binary | ios::in | ios::out);
    file.seekp(pos);
    file.write(reinterpret_cast<const char*>(&book), sizeof(Book));
    file.close();
}

void addBookMenu() {
    Book book;
    int id = getNextId(BOOKS_FILE, sizeof(Book));

    cout << "\n----- Add New Book -----\n";
    cout << "Book ID will be: " << id << "\n";
    book.addBook(id);

    ofstream fout(BOOKS_FILE, ios::binary | ios::app);
    fout.write(reinterpret_cast<char*>(&book), sizeof(Book));
    fout.close();

    cout << "\nBook added successfully!\n";
    pauseScreen();
}

void addMemberMenu() {
    Member member;
    int id = getNextId(MEMBERS_FILE, sizeof(Member));

    cout << "\n----- Add New Member -----\n";
    cout << "Member ID will be: " << id << "\n";
    member.addMember(id);

    ofstream fout(MEMBERS_FILE, ios::binary | ios::app);
    fout.write(reinterpret_cast<char*>(&member), sizeof(Member));
    fout.close();

    cout << "\nMember added successfully!\n";
    pauseScreen();
}

void issueBookMenu() {
    int bookId, memberId;

    cout << "\n----- Issue Book -----\n";
    cout << "Enter Book ID: ";
    while (!(cin >> bookId)) { cout << "Invalid input. Enter numeric Book ID: "; clearInputBuffer(); }
    clearInputBuffer();

    cout << "Enter Member ID: ";
    while (!(cin >> memberId)) { cout << "Invalid input. Enter numeric Member ID: "; clearInputBuffer(); }
    clearInputBuffer();

    Book book;
    streampos pos;
    if (!findBook(bookId, book, pos)) {
        cout << "\nBook not found.\n";
        pauseScreen();
        return;
    }

    Member member;
    if (!findMember(memberId, member)) {
        cout << "\nMember not found.\n";
        pauseScreen();
        return;
    }

    if (!book.isAvailable()) {
        cout << "\nSorry, no copies of \"" << book.getTitle() << "\" are currently available.\n";
        pauseScreen();
        return;
    }

    book.issueCopy();
    rewriteBook(book, pos);

    IssueRecord record;
    int recordId = getNextId(ISSUES_FILE, sizeof(IssueRecord));
    record.create(recordId, bookId, memberId);

    ofstream fout(ISSUES_FILE, ios::binary | ios::app);
    fout.write(reinterpret_cast<char*>(&record), sizeof(IssueRecord));
    fout.close();

    cout << "\nBook \"" << book.getTitle() << "\" issued to " << member.getName()
         << " (Issue Record ID: " << recordId << ")\n";
    pauseScreen();
}

void returnBookMenu() {
