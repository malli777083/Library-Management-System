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

    // Case-insensitive substring match, used for search
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

// ---------------------------------------------------------
// Utilities
// ---------------------------------------------------------
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
    char buffer[200]; // large enough for any single record type used here
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

// ---------------------------------------------------------
// 1. ADD BOOK
// ---------------------------------------------------------
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

// ---------------------------------------------------------
// 2. ADD MEMBER
// ---------------------------------------------------------
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

// ---------------------------------------------------------
// 3. ISSUE BOOK
// ---------------------------------------------------------
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

// ---------------------------------------------------------
// 4. RETURN BOOK
// ---------------------------------------------------------
void returnBookMenu() {
    int recordId;
    cout << "\n----- Return Book -----\n";
    cout << "Enter Issue Record ID: ";
    while (!(cin >> recordId)) { cout << "Invalid input. Enter numeric Record ID: "; clearInputBuffer(); }
    clearInputBuffer();

    fstream file(ISSUES_FILE, ios::binary | ios::in | ios::out);
    if (!file) {
        cout << "\nNo issue records found.\n";
        pauseScreen();
        return;
    }

    IssueRecord record;
    bool found = false;
    streampos pos;

    while (file.read(reinterpret_cast<char*>(&record), sizeof(IssueRecord))) {
        if (record.getRecordId() == recordId) {
            found = true;
            pos = file.tellg();
            pos -= static_cast<streamoff>(sizeof(IssueRecord));
            break;
        }
    }

    if (!found) {
        cout << "\nIssue record not found.\n";
        file.close();
        pauseScreen();
        return;
    }

    if (!record.isIssued()) {
        cout << "\nThis book has already been returned.\n";
        file.close();
        pauseScreen();
        return;
    }

    record.markReturned();
    file.seekp(pos);
    file.write(reinterpret_cast<char*>(&record), sizeof(IssueRecord));
    file.close();

    // Increase the available copy count for the returned book
    Book book;
    streampos bookPos;
    if (findBook(record.getBookId(), book, bookPos)) {
        book.returnCopy();
        rewriteBook(book, bookPos);
        cout << "\n\"" << book.getTitle() << "\" has been returned successfully.\n";
    } else {
        cout << "\nBook record returned, but original book entry was not found.\n";
    }
    pauseScreen();
}

// ---------------------------------------------------------
// 5. SEARCH BOOK (by Title or Author)
// ---------------------------------------------------------
void searchBookMenu() {
    cin.clear();
    cout << "\n----- Search Book -----\n";
    cout << "Enter Title or Author keyword: ";
    string keyword;
    getline(cin, keyword);

    ifstream fin(BOOKS_FILE, ios::binary);
    if (!fin) {
        cout << "\nNo books found.\n";
        pauseScreen();
        return;
    }

    Book book;
    bool found = false;

    cout << "\n----------------------------------------------------------------------\n";
    cout << left << setw(6) << "ID" << setw(30) << "Title" << setw(20) << "Author"
         << setw(8) << "Total" << setw(10) << "Available" << "\n";
    cout << "----------------------------------------------------------------------\n";

    while (fin.read(reinterpret_cast<char*>(&book), sizeof(Book))) {
        if (book.matches(keyword)) {
            found = true;
            book.display();
        }
    }
    fin.close();

    if (!found) {
        cout << "No books matched \"" << keyword << "\".\n";
    }
    cout << "----------------------------------------------------------------------\n";
    pauseScreen();
}

// ---------------------------------------------------------
// 6. DISPLAY ALL BOOKS
// ---------------------------------------------------------
void displayAllBooks() {
    ifstream fin(BOOKS_FILE, ios::binary);
    if (!fin) {
        cout << "\nNo books found.\n";
        pauseScreen();
        return;
    }

    Book book;
    bool found = false;

    cout << "\n----------------------------------------------------------------------\n";
    cout << left << setw(6) << "ID" << setw(30) << "Title" << setw(20) << "Author"
         << setw(8) << "Total" << setw(10) << "Available" << "\n";
    cout << "----------------------------------------------------------------------\n";

    while (fin.read(reinterpret_cast<char*>(&book), sizeof(Book))) {
        found = true;
        book.display();
    }
    fin.close();

    if (!found) cout << "No book records available.\n";
    cout << "----------------------------------------------------------------------\n";
    pauseScreen();
}

// ---------------------------------------------------------
// 7. DISPLAY ALL MEMBERS
// ---------------------------------------------------------
void displayAllMembers() {
    ifstream fin(MEMBERS_FILE, ios::binary);
    if (!fin) {
        cout << "\nNo members found.\n";
        pauseScreen();
        return;
    }

    Member member;
    bool found = false;

    cout << "\n----------------------------------------\n";
    cout << left << setw(6) << "ID" << setw(25) << "Name" << setw(15) << "Phone" << "\n";
    cout << "----------------------------------------\n";

    while (fin.read(reinterpret_cast<char*>(&member), sizeof(Member))) {
        found = true;
        member.display();
    }
    fin.close();

    if (!found) cout << "No member records available.\n";
    cout << "----------------------------------------\n";
    pauseScreen();
}

// ---------------------------------------------------------
// 8. DISPLAY ALL ISSUE RECORDS
// ---------------------------------------------------------
void displayAllIssues() {
    ifstream fin(ISSUES_FILE, ios::binary);
    if (!fin) {
        cout << "\nNo issue records found.\n";
        pauseScreen();
        return;
    }

    IssueRecord record;
    bool found = false;

    cout << "\n----------------------------------------\n";
    cout << left << setw(10) << "RecordID" << setw(10) << "BookID"
         << setw(10) << "MemberID" << setw(10) << "Status" << "\n";
    cout << "----------------------------------------\n";

    while (fin.read(reinterpret_cast<char*>(&record), sizeof(IssueRecord))) {
        found = true;
        record.display();
    }
    fin.close();

    if (!found) cout << "No issue records available.\n";
    cout << "----------------------------------------\n";
    pauseScreen();
}

// ---------------------------------------------------------
// MAIN MENU
// ---------------------------------------------------------
void showMenu() {
    cout << "\n==============================================\n";
    cout << "         LIBRARY MANAGEMENT SYSTEM\n";
    cout << "==============================================\n";
    cout << "1. Add Book\n";
    cout << "2. Add Member\n";
    cout << "3. Issue Book\n";
    cout << "4. Return Book\n";
    cout << "5. Search Book (Title / Author)\n";
    cout << "6. Display All Books\n";
    cout << "7. Display All Members\n";
    cout << "8. Display All Issue Records\n";
    cout << "9. Exit\n";
    cout << "==============================================\n";
    cout << "Enter your choice: ";
}

int main() {
    int choice;

    do {
        showMenu();
        while (!(cin >> choice)) {
            cout << "Invalid input. Enter a number between 1-9: ";
            clearInputBuffer();
        }
        clearInputBuffer();

        switch (choice) {
            case 1: addBookMenu(); break;
            case 2: addMemberMenu(); break;
            case 3: issueBookMenu(); break;
            case 4: returnBookMenu(); break;
            case 5: searchBookMenu(); break;
            case 6: displayAllBooks(); break;
            case 7: displayAllMembers(); break;
            case 8: displayAllIssues(); break;
            case 9: cout << "\nThank you for using the Library Management System!\n"; break;
            default: cout << "\nInvalid choice. Please try again.\n"; pauseScreen();
        }

    } while (choice != 9);

    return 0;
}
