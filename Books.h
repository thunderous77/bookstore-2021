//
// Created by 27595 on 2021/12/22.
//

#ifndef INC_1075_BOOKS_H
#define INC_1075_BOOKS_H

#include <ostream>
#include "FileAccess.h"
#include "Accounts.h"
#include "Error.h"

const int isbn_len = 20;
const int keyword_len = 60;
const int author_len = 60;
const int name_len = 60;

bool isbn_judge(const string &s) {
    if (s.empty() || s.length() > isbn_len)
        return false;
    for (int i = 0; i < s.length(); ++i) {
        if (!isprint(s[i]))
            return false;
    }
    return true;
}

bool name_judge(const string &s) {
    if (s.empty() || s.length() > name_len)
        return false;
    for (int i = 0; i < s.length(); ++i) {
        if (!isprint(s[i]) || s[i] == '\"')
            return false;
    }
    return true;
}

bool author_judge(const string &s) {
    if (s.empty() || s.length() > author_len)
        return false;
    for (int i = 0; i < s.length(); ++i) {
        if (!isprint(s[i]) || s[i] == '\"')
            return false;
    }
    return true;
}

bool keyword_judge(const string &s) {//show use it
    if (s.empty() || s.length() > keyword_len)
        return false;
    for (int i = 0; i < s.length(); ++i) {
        if (!isprint(s[i]) || s[i] == '\"')
            return false;
    }
    return true;
}

bool count_judge(const string &s) {
    if (s.empty() || s.length() > 10)
        return false;
    for (int i = 0; i < s.length(); ++i) {
        if (s[i] > '9' || s[i] < '0') {
            return false;
        }
    }
    if (std::stoll(s) >= 2147483647) return false;
    return true;
}

bool price_judge(const string &s) {
    int tmp = 0;
    if (s.empty() || s.length() > 13 || s[0] == '.' || s[s.length() - 1] == '.')
        return false;
    for (int i = 0; i < s.length(); ++i) {
        if (s[i] > '9' || s[i] < '0') {
            if (s[i] != '.')
                return false;
            else tmp++;
        }
    }
    return tmp <= 1;
}

class book_detail {
private:
    char isbn[isbn_len + 1] = "", keyword[keyword_len + 1] = "",
            author[author_len + 1] = "", name[name_len + 1] = "";
    long long int count = 0;//图书数量
    double price;
public:
    book_detail() = default;

    book_detail(const string &isbn_, const string &keyword_ = "", const string &author_ = "", const string &name_ = "",
                const long long int count_ = 0, const double price_ = 0) : book_detail() {
        strcpy(isbn, isbn_.c_str());
        strcpy(keyword, keyword_.c_str());
        strcpy(name, name_.c_str());
        strcpy(author, author_.c_str());
    }

    const string Isbn() { return isbn; }

    const string Name() { return name; }

    const string Author() { return author; }

    const string Keyword() { return keyword; }

    const double Price() { return price; }

    const long long int Count() { return count; }

    void addcount(const int &quantity) {
        if (count + quantity < 0) throw Error();
        count += quantity;
    }

    void modifyisbn(const string &isbn_) {
        memset(isbn, '\0', sizeof(isbn));
        strcpy(isbn, isbn_.c_str());
    }

    void modifyname(const string &name_) {
        memset(name, '\0', sizeof(name));
        strcpy(name, name_.c_str());
    }

    void modifyauthor(const string &author_) {
        memset(author, '\0', sizeof(author));
        strcpy(author, author_.c_str());
    }

    void modifykeyword(const string &keyword_) {
        memset(keyword, '\0', sizeof(keyword));
        strcpy(keyword, keyword_.c_str());
    }

    void modifyprice(const double &price_) {
        price = price_;
    }

    friend ostream &operator<<(ostream &os, const book_detail &detail) {
        os << std::fixed << std::setprecision(2) << detail.isbn << '\t' << detail.name << '\t' << detail.author << '\t'
           << detail.keyword << '\t' << detail.price << '\t' << detail.count;
        return os;
    }
};

class Books {
private:
    Memory<book_detail> book;
    blocklist isbn_list;
    blocklist_double name_list, author_list, keyword_list;

    void print(const vector<int> &index) {
        for (auto iter: index) {
            book_detail out;
            book.read(iter, out);
            cout << out << '\n';
        }
    }

public:
    void initialise(const string &file_name) {
        book.initialise(file_name + "_store");
        isbn_list.initialise(file_name + "_isbn_list");
        name_list.initialise(file_name + "_name_list");
        author_list.initialise(file_name + "_author_list");
        keyword_list.initialise(file_name + "_keyword_list");
    }

    void show_isbn(const string &content) {
        vector<int> index;
        isbn_list.find(content, index);
        print(index);
    }

    void show_name(const string &content) {
        vector<int> index;
        name_list.find(content, index);
        if (!index.empty())
            print(index);
        else cout << "\n";
    }

    void show_author(const string &content) {
        vector<int> index;
        author_list.find(content, index);
        print(index);
    }

    void show_keyword(const string &content) {
        vector<int> index;
        keyword_list.find(content, index);
        print(index);
    }

    void show() {
        if (book.sum == 0)
            cout << "\n";
        else {
            vector<int> index;
            isbn_list.show(index);
            print(index);
        }
    }

    double buy(const int &index, const int &quantity) {
        book_detail tmp;
        book.read(index, tmp);
        tmp.addcount(-quantity);
        book.write(index, tmp);
        return tmp.Price() * quantity;
    }

    int isbn_search(const string &isbn) {
        return isbn_list.search(isbn);
    }

    int select(const string &isbn) {
        int index = isbn_list.search(isbn);
        if (index == -1) {
            book_detail tmp(isbn);
            int pos = book.write(tmp);
            isbn_list.insert(isbn, pos);
            name_list.insert("", isbn, pos);
            author_list.insert("", isbn, pos);
            keyword_list.insert("", isbn, pos);
            return pos;
        } else return index;
    }

    void modify_isbn(const int &index, const string &content) {
        if (isbn_list.search(content)!=-1) throw Error();
        book_detail tmp;
        book.read(index, tmp);
        isbn_list.dlt(tmp.Isbn(), index);
        tmp.modifyisbn(content);
        isbn_list.insert(content, index);
        book.write(index, tmp);
    }

    void modify_name(const int &index, const string &content) {
        book_detail tmp;
        book.read(index, tmp);
        name_list.dlt(tmp.Name(), tmp.Isbn(), index);
        tmp.modifyname(content);
        name_list.insert(content, tmp.Isbn(), index);
        book.write(index, tmp);
    }

    void modify_author(const int &index, const string &content) {
        book_detail tmp;
        book.read(index, tmp);
        author_list.dlt(tmp.Author(), tmp.Isbn(), index);
        tmp.modifyauthor(content);
        author_list.insert(content, tmp.Isbn(), index);
        book.write(index, tmp);
    }

    void modify_keyword(const int &index, const string &content) {
        book_detail tmp;
        book.read(index, tmp);
        int pos = 0;
        string key;
        for (int iter = 0; iter < tmp.Keyword().length(); ++iter) {
            if (tmp.Keyword()[iter] == '|') {
                key = tmp.Keyword().substr(pos, iter - pos);
                keyword_list.dlt(key, tmp.Isbn(), index);
                pos = iter + 1;
            }
        }
        key = tmp.Keyword().substr(pos);
        keyword_list.dlt(key, tmp.Isbn(), index);
        tmp.modifykeyword(content);
        pos = 0;
        for (int iter = 0; iter < content.length(); ++iter) {
            if (tmp.Keyword()[iter] == '|') {
                key = content.substr(pos, iter - pos);
                keyword_list.insert(key, tmp.Isbn(), index);
                pos = iter + 1;
            }
        }
        key = content.substr(pos);
        keyword_list.insert(key, tmp.Isbn(), index);
        book.write(index, tmp);
    }

    void modify_price(const int &index, const string &content) {
        book_detail tmp;
        book.read(index, tmp);
        tmp.modifyprice(std::stod(content));
        book.write(index, tmp);
    }

    void import(const int &index, const int &quantity) {
        book_detail tmp;
        book.read(index, tmp);
        tmp.addcount(quantity);
        book.write(index, tmp);
    }
};


#endif //INC_1075_BOOKS_H
