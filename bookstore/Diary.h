//
// Created by 27595 on 2021/12/22.
//

#ifndef INC_1075_DIARY_H
#define INC_1075_DIARY_H

#include <ostream>
#include "FileAccess.h"
#include "Error.h"

bool time_judge(const string &s) {
    if (s.length() > 10) return false;
    if (s.empty() || std::stoll(s) > 2147483647) return false;
    for (auto iter: s) {
        if (iter > '9' || iter < '0')
            return false;
    }
    return true;
}

class trade {
private:
    double in = 0, out = 0;
public:
    trade() = default;

    trade(const double &money) {
        if (money > 0)
            in = money;
        else out = 0 - money;
    }

    void operator+=(const trade &t) {
        in += t.in;
        out += t.out;
    }

    friend ostream &operator<<(ostream &os, const trade &trade) {
        os << std::fixed << std::setprecision(2) << "+ " << trade.in << " - " << trade.out;
        return os;
    }
};

class Diary {
    Memory<trade> td;
public:
    void initialise(const string &s) {
        td.initialise(s + "_trade_data");
    }

    void add(trade &t) {
        td.write(t);
    }

    void show(const string &time) {
        if (!time_judge(time)) throw Error();
        int tim = std::stoi(time);
        if (tim > td.sum) throw Error();
        if (tim == 0) cout << "\n";
        else {
            trade ans, tmp;
            for (int iter = 1; iter <= tim; ++iter) {
                td.read((td.sum - iter) * sizeof(trade), tmp);
                ans += tmp;
            }
            cout << ans << '\n';
        }
    }

    void show() {
        trade ans, tmp;
        for (int iter = td.sum - 1; iter >= 0; --iter) {
            td.read(iter * sizeof(trade), tmp);
            ans += tmp;
        }
        cout << ans << '\n';
    }
};


#endif //INC_1075_DIARY_H
