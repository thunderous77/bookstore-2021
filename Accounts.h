//
// Created by 27595 on 2021/12/22.
//

#ifndef INC_1075_ACCOUNTS_H
#define INC_1075_ACCOUNTS_H

#include "FileAccess.h"

#include "Error.h"
#include "FileAccess.h"
#include <unordered_map>

bool username_judge(const string &s) {
    if (s.length() > 30) return false;
    for (auto iter: s) {
        if (!isprint(iter)) return false;
    }
    return true;
}

bool id_judge(const string &s) {
    if (s.length() > 30) return false;
    for (auto iter: s) {
        if (!isalnum(iter) && iter != '_') return false;
    }
    return true;
}

bool password_judge(const string &s) {
    return id_judge(s);
}

bool privilege_judge(const string &s) {
    if (s.length() != 1) return false;
    if (s[0] != '0' && s[0] != '1' && s[0] != '3' && s[0] != '7') return false;
    return true;
}

class Accounts {
private:
    char id[31] = "", password[31] = "", name[31] = "";
    int privilege = 0;
public:
    Accounts() = default;

    Accounts(const string &id_, const string &password_, const string &name_, const int &privilege_)
            : Accounts() {
        privilege = privilege_;
        strcpy(id, id_.c_str());
        strcpy(password, password_.c_str());
        strcpy(name, name_.c_str());
    }

    Accounts(const Accounts &a) {
        privilege = a.privilege;
        strcpy(id, a.id);
        strcpy(password, a.password);
        strcpy(name, name);
    }

    int Privilege() {
        return privilege;
    }

    const string Name() {
        return name;
    }

    const string Password() {
        return password;
    }

    const string Id() {
        return id;
    }

    void changepassword(const string &new_password) {
        if (!password_judge(new_password)) throw Error();
        memset(password, '\0', sizeof(password));
        strcpy(password, new_password.c_str());
    }
};

class loginuser {
private:
    Accounts account;
    int select_book_index = -1;
public:
    loginuser() = default;

    loginuser(const Accounts &a) : account(a), select_book_index(-1) {}

    int Privilege() {
        return account.Privilege();
    }

    int Selectbook() {
        return select_book_index;
    }

    void selectbook(int index) {
        select_book_index = index;
    }

    const string Id() {
        return account.Id();
    }
};

class User {
private:
    blocklist id_list;
    Memory<Accounts> user;
    vector<loginuser> all_login_user;
    unordered_map<string, int> id_login;
public:
    int Privilege() {
        if (all_login_user.empty()) return 0;
        else return all_login_user.rbegin()->Privilege();
    }

    int Selectbook() {
        if (all_login_user.empty()) throw Error();
        return all_login_user.rbegin()->Selectbook();
    }

    void selectbook(int index) {
        all_login_user.rbegin()->selectbook(index);
    }

    const string Id() {
        return all_login_user.rbegin()->Id();
    }

    void initialise(const string &file_name) {
        user.initialise(file_name + "_store");
        id_list.initialise(file_name + "_id_list");
        int index = id_list.search("root");
        if (index == -1) {
            Accounts super("root", "sjtu", "", 7);
            id_list.insert("root", user.write(super));
        }
    }

    void su(const string &id, const string &password) {
        Accounts tmp;
        int index = id_list.search((id));
        if (index == -1) throw Error();
        user.read(index, tmp);
        if (password != tmp.Password()) {
            throw Error();
        }
        loginuser l(tmp);
        all_login_user.push_back(l);
        id_login[id]++;
    }

    void su(const string &id) {
        Accounts tmp;
        int index = id_list.search((id));
        if (index == -1) throw Error();
        user.read(index, tmp);
        if (Privilege() <= tmp.Privilege())
            throw Error();
        loginuser l(tmp);
        all_login_user.push_back(l);
        id_login[id]++;
    }

    void logout() {
        if (Privilege() < 1)
            throw Error();
        if (all_login_user.empty())
            throw Error();
        else {
            id_login[Id()]--;
            all_login_user.pop_back();
        }
    }

    void Register(const string &id, const string &password, const string &username) {
        if (!id_judge(id) || !password_judge(password) || !username_judge(username))
            throw Error();
        if (id_list.search(id) != -1) throw Error();
        Accounts tmp(id, password, username, 1);
        id_list.insert(id, user.write(tmp));
    }

    void changepassword(const string &id, const string &old_password, const string &new_password) {
        Accounts tmp;
        if (!password_judge(new_password)) throw Error();
        int index = id_list.search((id));
        if (index == -1) throw Error();
        user.read(index, tmp);
        if (old_password != tmp.Password()) {
            throw Error();
        }
        tmp.changepassword(new_password);
        user.write(index, tmp);
    }

    void changepassword(const string &id, const string &new_password) {
        Accounts tmp;
        if (!password_judge(new_password)) throw Error();
        int index = id_list.search((id));
        if (index == -1) throw Error();
        user.read(index, tmp);
        if (Privilege() < 7) throw Error();
        tmp.changepassword(new_password);
        user.write(index, tmp);
    }

    void useradd(const string &id, const string &password, const string &name, const string &privilege) {
        if (!id_judge(id) || !password_judge(password) || !username_judge(name) || !privilege_judge(privilege))
            throw Error();
        int plg = std::stoi(privilege);
        if (plg >= Privilege()) throw Error();
        if (id_list.search(id) != -1) throw Error();
        Accounts tmp(id, password, name, plg);
        id_list.insert(id, user.write(tmp));
    }

    void Delete(const string &id) {
        int index = id_list.search((id));
        if (index == -1 || id_login[id]>0) throw Error();
        user.pushdlt(index);
        id_list.dlt(id, index);
    }
};


#endif //INC_1075_ACCOUNTS_H
