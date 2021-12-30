//
// Created by 27595 on 2021/12/22.
//

#ifndef INC_1075_TOKENSCANNER_H
#define INC_1075_TOKENSCANNER_H

#include <string>

using namespace std;

class TokenScanner {
private:
    struct word {
        string token;
        word *next;
    } *head;
    word *cur;
    int num = 0;//字符串块数
public:
    TokenScanner() {
        head = new word;
        head->next = nullptr;
        cur = head;
    }

    ~TokenScanner() {
        while (head->next != nullptr) {
            word *p = head->next;
            head->next = p->next;
            delete p;
        }
    }

    bool empty() {
        return num == 0;
    }

    int Num() {
        return num;
    }

    void divide(const string &sentence) {
        //todo:divide the sentence into tokens,in the form of linklist
        num = 0;
        int pos = 0, pre = 0;
        word *tmp = head;
        if (sentence.length() == 0) {
            return;
        }
        while (pos < sentence.length()) {
            if (sentence[pos] == ' ') {
                num++;
                word *p;
                p = new word;
                p->token = sentence.substr(pre, pos - pre);
                tmp->next = p;
                p->next = nullptr;
                tmp = p;
                pre = pos + 1;
            }
            pos++;
        }
        if (pre < pos) {
            num++;
            word *p;
            p = new word;
            p->token = sentence.substr(pre, pos - pre);
            tmp->next = p;
            p->next = nullptr;
        }
    }

    string firstToken() {//the first token
        cur = head->next;
        return cur->token;
    }

    //usage:if(obj.hasMoreToken())str=obj.nextToken();
    bool hasMoreToken() {
        return (cur->next != nullptr);
    }

    string nextToken() {
        cur = cur->next;
        return cur->token;
    }


};


#endif //INC_1075_TOKENSCANNER_H
