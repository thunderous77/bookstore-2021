//
// Created by 27595 on 2021/12/22.
//

#ifndef INC_1075_EXPRESS_H
#define INC_1075_EXPRESS_H

#include <iomanip>
#include "TokenScanner.h"
#include "Books.h"
#include "Accounts.h"
#include "Diary.h"
#include "Error.h"
#include <unordered_map>
#include <set>

class Express {
private:
    Books books;
    Diary diary;
    User user;

    void divide(const string &command, string &type, string &content) {
        int pos = 0;
        while (command[pos] != '=' && pos < command.length()) { pos++; }
        if (pos == 0 || pos >= command.length() - 1) throw Error();
        type = command.substr(1, pos - 1);
        if (type == "ISBN") {
            content = command.substr(pos + 1, command.length() - pos - 1);
            if (!isbn_judge(content)) throw Error();
        } else if (type == "name") {
            if (command.length() - pos - 1 <= 2 || command[pos + 1] != '\"' || command[command.length() - 1] != '\"')
                throw Error();
            content = command.substr(pos + 2, command.length() - pos - 3);
            if (!name_judge(content)) throw Error();
        } else if (type == "author") {
            if (command.length() - pos - 1 <= 2 || command[pos + 1] != '\"' || command[command.length() - 1] != '\"')
                throw Error();
            content = command.substr(pos + 2, command.length() - pos - 3);
            if (!author_judge(content)) throw Error();
        } else if (type == "keyword") {
            if (command.length() - pos - 1 <= 2 || command[pos + 1] != '\"' || command[command.length() - 1] != '\"')
                throw Error();
            content = command.substr(pos + 2, command.length() - pos - 3);
            if (content[0] == '|' && content[content.length() - 1] == '|')
                throw Error();
            if (!keyword_judge(content)) throw Error();
        } else if (type == "price") {
            content = command.substr(pos + 1, command.length() - pos - 1);
            if (!price_judge(content)) throw Error();
        }
    }

public:
    void initialise() {
        books.initialise("books");
        diary.initialise("diary");
        user.initialise("user");
    }

    bool express(TokenScanner tok) {
        //quit and exit are judged in main.cpp
        if (tok.empty()) return false;
        string head = tok.firstToken(), next = "";
        if (tok.hasMoreToken()) next = tok.nextToken();
        if (head == "quit" || head == "exit") {
            if (tok.Num() != 1) throw Error();
            return true;
        } else if (head == "su") {
            //su [User-ID] ([Password])?
            if (tok.Num() == 2) {
                user.su(next);
            } else if (tok.Num() == 3) {
                user.su(next, tok.nextToken());
            } else throw Error();
        } else if (head == "logout") {
            //logout
            if (tok.Num() == 1) user.logout();
            else throw Error();
        } else if (head == "register") {
            //register [User-ID] [Password] [User-Name]
            if (tok.Num() != 4) throw Error();
            string password = tok.nextToken(), username = tok.nextToken();
            user.Register(next, password, username);
        } else if (head == "passwd") {
            //passwd [User-ID] ([Old-Password])? [New-Password]
            string nxt;
            if (tok.hasMoreToken()) nxt = tok.nextToken();
            if (tok.Num() == 3) {
                user.changepassword(next, nxt);
            } else if (tok.Num() == 4) {
                user.changepassword(next, nxt, tok.nextToken());
            } else throw Error();
        } else if (head == "useradd") {
            //useradd [User-ID] [Password] [Priority] [User-Name]
            if (tok.Num() != 5) throw Error();
            string password = tok.nextToken(), priority = tok.nextToken(), username = tok.nextToken();
            user.useradd(next, password, username, priority);
        } else if (head == "delete") {
            //delete [User-ID]
            if (tok.Num() != 2) throw Error();
            user.Delete(next);
        } else if (head == "show") {
            if (user.Privilege() < 1) throw Error();
            if (next != "") {
                if (next == "finance") {
                    //show finance ([Time])?
                    if (user.Privilege() < 7) throw Error();
                    if (tok.Num() == 2) {
                        diary.show();
                    } else {
                        if (tok.Num() != 3) throw Error();
                        else diary.show(tok.nextToken());
                    }
                } else {
                    //show (-ISBN=[ISBN] | -name="[Book-Name]" | -author="[Author]" | -keyword="[Keyword]")?
                    if (tok.Num() != 2) throw Error();
                    string type = "", content = "";
                    divide(next, type, content);
                    if (type == "ISBN") {
                        books.show_isbn(content);
                    } else if (type == "name") {
                        books.show_name(content);
                    } else if (type == "author") {
                        books.show_author(content);
                    } else if (type == "keyword") {
                        books.show_keyword(content);
                    } else throw Error();
                }
            } else {
                //show
                books.show();
            }
        } else if (head == "buy") {
            //buy [ISBN] [Quantity]
            if (user.Privilege() < 1) throw Error();
            string isbn = next, quantity;
            if (tok.Num() != 3) throw Error();
            quantity = tok.nextToken();
            if (!isbn_judge(isbn) || !count_judge(quantity)) throw Error();
            int index = books.isbn_search(isbn);
            if (index == -1) throw Error();
            double money = books.buy(index, std::stoi(quantity));
            cout << std::fixed << std::setprecision(2) << money << "\n";
            trade t(money);
            diary.add(t);
        } else if (head == "select") {//记录文件位置
            //select [ISBN]
            if (user.Privilege() < 3) throw Error();
            if (tok.Num() != 2) throw Error();
            if (!isbn_judge(next)) throw Error();
            user.selectbook(books.select(next));
        } else if (head == "modify") {
            //modify (-ISBN=[ISBN] | -name="[Book-Name]" | -author="[Author]" | -keyword="[Keyword]" | -price=[Price])+
            if (user.Privilege() < 3) throw Error();
            int index = user.Selectbook();//选中书再储存文件的地址
            if (tok.Num() == 1 || index == -1) throw Error();
            unordered_map<string, string> command;
            set<string> unique;//是否重复
            string type, content, nxt;
            //先检查
            divide(next, type, content);
            unique.insert(type);
            command[type] = content;
            while (tok.hasMoreToken()) {
                nxt = tok.nextToken();
                divide(nxt, type, content);
                if (unique.find(type) != unique.end()) throw Error();
                else {
                    unique.insert(type);
                    command[type] = content;
                }
            }
            //再modify
            for (auto iter: unique) {
                if (iter == "ISBN") {
                    books.modify_isbn(index, command[iter]);
                } else if (iter == "name") {
                    books.modify_name(index, command[iter]);
                } else if (iter == "author") {
                    books.modify_author(index, command[iter]);
                } else if (iter == "keyword") {
                    books.modify_keyword(index, command[iter]);
                } else if (iter == "price") {
                    books.modify_price(index, command[iter]);
                }
            }
        } else if (head == "import") {
            //import [Quantity] [Total-Cost]
            if (user.Privilege() < 3) throw Error();
            if (tok.Num() != 3) throw Error();
            string price = tok.nextToken();
            if (!count_judge(next) || !price_judge(price)) throw Error();
            int index = user.Selectbook();//选中书再储存文件的地址
            if (index == -1) throw Error();
            books.import(index, std::stoi(next));
            trade t(0-std::stod(price));
            diary.add(t);
        } else if (head == "report") {
            if (tok.Num() != 2)throw Error();
            if (next == "myself") {
                //report myself

            } else if (next == "employee") {
                //report employee

            } else if (next == "finance") {
                //report finance

            }
        } else if (head == "log") {
            //log

        } else throw Error();
        return false;
    }
};


#endif //INC_1075_EXPRESS_H
