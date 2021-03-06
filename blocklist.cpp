#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstring>

using namespace std;

const int maxsize = 1500;

template<class T>
class information {
private:
    char index[65];
    T value;
public:
    information() {}

    information(std::string ind, T value) : value(value) {
        strcpy(index, ind.c_str());
    }

    char *ind() {
        return index;
    }

    T val() {
        return value;
    }

    bool operator<(const information &rhs) const {
        if (strcmp(index, rhs.index) != 0) return (strcmp(index, rhs.index) < 0);
        return value < rhs.value;
    }

    bool operator>=(const information &rhs) const {
        return (!(*this < rhs));
    }

    bool operator==(const information &rhs) const {
        return (strcmp(index, rhs.index) == 0) && (value == rhs.value);
    }

    bool operator!=(const information &rhs) const {
        return (!(*this == rhs));
    }

    information operator=(const information &rhs) {
        strcpy(index, rhs.index);
        value = rhs.value;
        return *this;
    }
};

template<class T>
class information_double {
private:
    char index1[60], index2[60];
    T value;
public:
    information_double() {}

    information_double(std::string ind1, std::string ind2, T value) :
            value(value) {
        strcpy(index1, ind1.c_str());
        strcpy(index2, ind2.c_str());
    }

    char *ind1() {
        return index1;
    }

    char *ind2() {
        return index2;
    }

    T val() {
        return value;
    }

    bool operator<(const information_double &rhs) const {
        if (strcmp(index1, rhs.index1) != 0) return (strcmp(index1, rhs.index1) < 0);
        if (strcmp(index2, rhs.index2) != 0) return (strcmp(index2, rhs.index2) < 0);
        return value < rhs.value;
    }

    bool operator>=(const information_double &rhs) const {
        return (!(*this < rhs));
    }

    bool operator==(const information_double &rhs) const {
        return (strcmp(index1, rhs.index1) == 0) && (strcmp(index2, rhs.index2) == 0) && (value == rhs.value);
    }

    bool operator!=(const information_double &rhs) const {
        return (!(*this == rhs));
    }

    information_double operator=(const information_double &rhs) {
        strcpy(index1, rhs.index1);
        strcpy(index2, rhs.index2);
        value = rhs.value;
        return *this;
    }
};

template<class T>
class node {
public:
    int next = -1, sum = 0;
    information<T> store[maxsize + 1];

    node() = default;
};

template<class T>
class node_double {
public:
    int next = -1, sum = 0;
    information_double<T> store[maxsize + 1];

    node_double() = default;
};

template<class T>
class Memory {
private:
    std::string file_name;
    std::fstream file;
    std::vector<int> dlt;
public:
    int sum = 0;

    Memory() = default;

    bool empty() {
        return sum == 0;
    }

    Memory(const std::string &file_name) : file_name(file_name) {}


    void initialise(std::string name = "") {
        if (name != "") file_name = name;
        file.open(file_name);
        if (!file) {
            file.open(file_name, std::ios::out);
            file.seekp(0, std::ios::beg);
            file.write(reinterpret_cast<char *>(&sum), sizeof(int));
        } else {
            file.seekg(0, std::ios::beg);
            file.read(reinterpret_cast<char *>(&sum), sizeof(int));
        }
        file.close();
    }

    void pushdlt(int pos) {
        sum--;
        file.open(file_name);
        file.seekp(0, std::ios::beg);
        file.write(reinterpret_cast<char *>(&sum), sizeof(int));
        file.close();
        dlt.push_back(pos);
    }

    void write(int pos, T &t) {
        file.open(file_name);
        file.seekp(pos + sizeof(int));
        file.write(reinterpret_cast<char *>(&t), sizeof(T));
        file.close();
        return;
    }

    int write(T &t) {//?????????
        sum++;
        file.open(file_name);
        file.seekp(0, std::ios::beg);
        file.write(reinterpret_cast<char *>(&sum), sizeof(int));
        if (!dlt.empty()) {
            file.seekp(dlt.back() + sizeof(int));
            int ans = dlt.back();
            file.write(reinterpret_cast<char *>(&t), sizeof(T));
            file.close();
            dlt.pop_back();
            return ans;
        }
        file.seekp(0, std::ios::end);
        file.write(reinterpret_cast<char *>(&t), sizeof(T));
        file.close();
        return (sum - 1) * sizeof(T);
    }

    void read(int pos, T &t) {
        file.open(file_name);
        file.seekg(pos + sizeof(int));
        file.read(reinterpret_cast<char *>(&t), sizeof(T));
        file.close();
    }
};

class blocklist {
private:
    Memory<node<int>> file;
public:
    void initialise(std::string name = "") {
        file.initialise(name);
    }

    void insert(std::string index, int num) {
        information inf(index, num);
        node<int> t, iter;
        file.read(0, iter);
        if (file.empty()) {//????????????
            t.next = -1;
            t.store[0] = inf;
            t.sum = 1;
            file.write(t);
            return;
        }
        int pos = 0, tmp = 0;
        for (; pos != -1; tmp = pos, pos = iter.next, file.read(pos, iter)) {
            if (iter.store[0] >= inf) break;
        }
        file.read(tmp, iter);
        if (iter.sum == 0) {
            iter.sum++;
            iter.store[0] = inf;
            file.write(tmp, iter);
        } else {
            iter.sum++;
            int it = std::lower_bound(iter.store, iter.store + iter.sum - 1, inf) - iter.store;
            for (int j = iter.sum - 1; j > it; --j) {
                iter.store[j] = iter.store[j - 1];
            }
            iter.store[it] = inf;
            if (iter.sum > maxsize) split(iter, tmp);
            else file.write(tmp, iter);
        }
        return;
    }

    void split(node<int> p, int pos) {//??????????????????
        node<int> t;
        t.sum = (maxsize + 1) / 2;
        for (int i = 0; i < t.sum; ++i) {
            t.store[t.sum - 1 - i] = p.store[p.sum - 1 - i];
        }
        p.sum -= t.sum;
        t.next = p.next;
        p.next = file.write(t);
        file.write(pos, p);
    }

    int search(std::string index) {
        node<int> iter;
        int pos = 0, tmp = 0;
        for (; pos != -1; tmp = pos, pos = iter.next, file.read(pos, iter)) {
            if (strcmp(index.c_str(), iter.store[0].ind()) <= 0) break;
        }
        file.read(tmp, iter);
        for (int i = 0; i < iter.sum; ++i) {
            if (strcmp(iter.store[i].ind(), index.c_str()) == 0) return iter.store[i].val();
        }
        return -1;
    }

    void find(std::string index, vector<int> &ans) {
        node<int> iter;
        ans.clear();
        int pos = 0, tmp = 0;
        for (; pos != -1; tmp = pos, pos = iter.next, file.read(pos, iter)) {
            if (strcmp(index.c_str(), iter.store[0].ind()) <= 0) break;
        }
        file.read(tmp, iter);
        information inf(index, 0);
        auto it = std::lower_bound(iter.store, iter.store + iter.sum, inf);
        for (; it < iter.store + iter.sum; ++it) {
            if (strcmp(it->ind(), index.c_str()) == 0) {
                ans.push_back(it->val());
            } else break;
        }
        bool flag = true;
        while (flag) {
            if (iter.next != -1) file.read(iter.next, iter);
            else break;
            for (int i = 0; i < iter.sum; ++i) {
                if (strcmp(index.c_str(), iter.store[i].ind()) == 0) {
                    ans.push_back(iter.store[i].val());
                } else {
                    if (strcmp(index.c_str(), iter.store[i].ind()) < 0) {
                        flag = false;
                        break;
                    }
                }
            }
        }
        return;
    }

    void merge(node<int> p, node<int> nxt, int pos) {
        for (int i = 0; i < nxt.sum; ++i) {
            p.store[i + p.sum] = nxt.store[i];
        }
        file.pushdlt(p.next);
        p.sum += nxt.sum;
        p.next = nxt.next;
        file.write(pos, p);
    }

    void dlt(std::string index, int num) {
        information inf(index, num);
        node<int> iter, nxt;
        file.read(0, iter);
        int pos = 0, tmp = 0;
        for (; pos != -1; tmp = pos, pos = iter.next, file.read(pos, iter)) {
            if (inf < iter.store[0]) break;
        }
        file.read(tmp, iter);
        int l = 0, r = iter.sum - 1, mid;
        while (l <= r) {
            mid = (l + r) / 2;
            if (iter.store[mid] == inf) break;
            if (iter.store[mid] < inf)
                l = mid + 1;
            else r = mid - 1;
        }
        if (l <= r) {
            iter.sum--;
            for (int j = mid; j < iter.sum; ++j) {
                iter.store[j] = iter.store[j + 1];
            }
            if (iter.next != -1) {
                file.read(iter.next, nxt);
                if (iter.sum + nxt.sum < maxsize / 2) {
                    merge(iter, nxt, tmp);
                } else file.write(tmp, iter);
            } else file.write(tmp, iter);
        }
        return;
    }

    void show(vector<int> &ans) {
        node<int> iter;
        ans.clear();
        int pos = 0;
        while (pos != -1) {
            file.read(pos, iter);
            for (int i = 0; i < iter.sum; ++i)
                ans.push_back(iter.store[i].val());
            pos = iter.next;
        }
        return;
    }
};

class blocklist_double {
private:
    Memory<node_double<int>> file;
public:
    void initialise(std::string name = "") {
        file.initialise(name);
    }

    void insert(std::string index1, std::string index2, int num) {
        information_double inf(index1, index2, num);
        node_double<int> t, iter;
        file.read(0, iter);
        if (file.empty()) {//????????????
            t.next = -1;
            t.store[0] = inf;
            t.sum = 1;
            file.write(t);
            return;
        }
        int pos = 0, tmp = 0;
        for (; pos != -1; tmp = pos, pos = iter.next, file.read(pos, iter)) {
            if (iter.store[0] >= inf) break;
        }
        file.read(tmp, iter);
        if (iter.sum == 0) {
            iter.sum++;
            iter.store[0] = inf;
            file.write(tmp, iter);
        } else {
            iter.sum++;
            int it = std::lower_bound(iter.store, iter.store + iter.sum - 1, inf) - iter.store;
            for (int j = iter.sum - 1; j > it; --j) {
                iter.store[j] = iter.store[j - 1];
            }
            iter.store[it] = inf;
            if (iter.sum > maxsize) split(iter, tmp);
            else file.write(tmp, iter);
        }
        return;
    }

    void split(node_double<int> p, int pos) {//??????????????????
        node_double<int> t;
        t.sum = (maxsize + 1) / 2;
        for (int i = 0; i < t.sum; ++i) {
            t.store[t.sum - 1 - i] = p.store[p.sum - 1 - i];
        }
        p.sum -= t.sum;
        t.next = p.next;
        p.next = file.write(t);
        file.write(pos, p);
    }

    void find(std::string index, vector<int> &ans) {
        node_double<int> iter;
        ans.clear();
        int pos = 0, tmp = 0;
        for (; pos != -1; tmp = pos, pos = iter.next, file.read(pos, iter)) {
            if (strcmp(index.c_str(), iter.store[0].ind1()) <= 0) break;
        }
        file.read(tmp, iter);
        information_double inf(index, "", 0);
        auto it = std::lower_bound(iter.store, iter.store + iter.sum, inf);
        for (; it < iter.store + iter.sum; ++it) {
            if (strcmp(it->ind1(), index.c_str()) == 0) {
                ans.push_back(it->val());
            } else break;
        }
        bool flag = true;
        while (flag) {
            if (iter.next != -1) file.read(iter.next, iter);
            else break;
            for (int i = 0; i < iter.sum; ++i) {
                if (strcmp(index.c_str(), iter.store[i].ind1()) == 0) {
                    ans.push_back(iter.store[i].val());
                } else {
                    if (strcmp(index.c_str(), iter.store[i].ind1()) < 0) {
                        flag = false;
                        break;
                    }
                }
            }
        }
        return;
    }

    void merge(node_double<int> p, node_double<int> nxt, int pos) {
        for (int i = 0; i < nxt.sum; ++i) {
            p.store[i + p.sum] = nxt.store[i];
        }
        file.pushdlt(p.next);
        p.sum += nxt.sum;
        p.next = nxt.next;
        file.write(pos, p);
    }

    void dlt(std::string index1, std::string index2, int num) {
        information_double inf(index1, index2, num);
        node_double<int> iter, nxt;
        file.read(0, iter);
        int pos = 0, tmp = 0;
        for (; pos != -1; tmp = pos, pos = iter.next, file.read(pos, iter)) {
            if (inf < iter.store[0]) break;
        }
        file.read(tmp, iter);
        int l = 0, r = iter.sum - 1, mid;
        while (l <= r) {
            mid = (l + r) / 2;
            if (iter.store[mid] == inf) break;
            if (iter.store[mid] < inf)
                l = mid + 1;
            else r = mid - 1;
        }
        if (l <= r) {
            iter.sum--;
            for (int j = mid; j < iter.sum; ++j) {
                iter.store[j] = iter.store[j + 1];
            }
            if (iter.next != -1) {
                file.read(iter.next, nxt);
                if (iter.sum + nxt.sum < maxsize / 2) {
                    merge(iter, nxt, tmp);
                } else file.write(tmp, iter);
            } else file.write(tmp, iter);
        }
        return;
    }
};


int main() {
    //freopen("1.in", "r", stdin);
    //freopen("1.out", "w", stdout);
    int n, value;
    std::string index;
    std::string command;
    blocklist l;
    l.initialise("mystore_0");
    std::cin >> n;
    while (n--) {
        std::cin >> command;
        if (command == "insert") {
            std::cin >> index >> value;
            l.insert(index, value);
        } else {
            if (command == "delete") {
                std::cin >> index >> value;
                l.dlt(index, value);
            } else {
                std::cin >> index;
                std::vector<int> ans;
                l.find(index, ans);
                if (index.empty())
                    cout << "null";
                else {
                    for (auto i: ans)
                        cout << i << " ";
                }
                cout << "\n";
            }
        }
    }
    //fclose(stdin);
    //fclose(stdout);
    return 0;
}