#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstring>

const int maxsize = 1500;

class information {
private:
    char index[65];
    int value;
public:
    information() {}

    information(std::string ind, int value) : value(value) {
        strcpy(index, ind.c_str());
    }

    char *ind() {
        return index;
    }

    int val() {
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

class node {
public:
    int next = -1, sum = 0;
    information store[maxsize + 1];

    node() = default;
};

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
            file.close();
        } else {
            file.seekg(0, std::ios::beg);
            file.read(reinterpret_cast<char *>(&sum), sizeof(int));
            file.close();
        }
    }

    void pushdlt(int pos) {
        sum--;
        file.open(file_name, std::ios::out);
        file.seekp(0, std::ios::beg);
        file.write(reinterpret_cast<char *>(&sum), sizeof(int));
        file.close();
        dlt.push_back(pos);
    }

    void write(int pos, node &t) {
        file.open(file_name);
        file.seekp(pos + sizeof(int));
        file.write(reinterpret_cast<char *>(&t), sizeof(node));
        file.close();
        return;
    }

    int write(node &t) {//在最后
        sum++;
        file.open(file_name);
        file.seekp(0, std::ios::beg);
        file.write(reinterpret_cast<char *>(&sum), sizeof(int));
        if (!dlt.empty()) {
            file.seekp(dlt.back() + sizeof(int));
            int ans = dlt.back();
            file.write(reinterpret_cast<char *>(&t), sizeof(node));
            file.close();
            dlt.pop_back();
            return ans;
        }
        file.seekp(0, std::ios::end);
        file.write(reinterpret_cast<char *>(&t), sizeof(node));
        file.close();
        return (sum - 1) * sizeof(node);
    }

    void read(int pos, node &t) {
        file.open(file_name, std::ios::in);
        file.seekg(pos + sizeof(int));
        file.read(reinterpret_cast<char *>(&t), sizeof(node));
        file.close();
    }
};

class list {
private:
    Memory file;
public:
    list() = default;

    void initialise(std::string name = "") {
        file.initialise(name);
    }

    void insert(std::string index, int num) {
        information inf(index, num);
        node t, iter;
        file.read(0, iter);
        if (file.empty()) {//文件为空
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
        iter.sum++;
        int it = std::lower_bound(iter.store, iter.store + iter.sum - 1, inf) - iter.store;
        for (int j = iter.sum - 1; j > it; --j) {
            iter.store[j] = iter.store[j - 1];
        }
        iter.store[it] = inf;
        if (iter.sum > maxsize) split(iter, tmp);
        else file.write(tmp, iter);
        return;
    }

    void split(node p, int pos) {//还要重写文件
        node t;
        t.sum = (maxsize + 1) / 2;
        for (int i = 0; i < t.sum; ++i) {
            t.store[t.sum - 1 - i] = p.store[p.sum - 1 - i];
        }
        p.sum -= t.sum;
        t.next = p.next;
        p.next = file.write(t);
        file.write(pos, p);
    }

    void merge(node p, node nxt, int pos) {
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
        node iter, nxt;
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

    void find(std::string index) {
        node iter;
        int pos = 0, tmp = 0;
        bool output = false;
        for (; pos != -1; tmp = pos, pos = iter.next, file.read(pos, iter)) {
            if (strcmp(index.c_str(), iter.store[0].ind()) <= 0) break;
        }
        file.read(tmp, iter);
        information inf(index, 0);
        auto it = std::lower_bound(iter.store, iter.store + iter.sum, inf);
        for (; it < iter.store + iter.sum; ++it) {
            if (strcmp(it->ind(), index.c_str()) == 0) {
                output = true;
                std::cout << it->val() << " ";
            } else break;
        }
        bool flag = true;
        while (flag) {
            if (iter.next != -1) file.read(iter.next, iter);
            else break;
            for (int i = 0; i < iter.sum; ++i) {
                if (strcmp(index.c_str(), iter.store[i].ind()) == 0) {
                    output = true;
                    std::cout << iter.store[i].val() << " ";
                } else {
                    if (strcmp(index.c_str(), iter.store[i].ind()) < 0) {
                        flag = false;
                        break;
                    }
                }
            }
        }
        if (!output) std::cout << "null";
        std::cout << "\n";
        return;
    }
};


int main() {
    //freopen("1.in", "r", stdin);
    //freopen("1.out", "w", stdout);
    int n, value;
    std::string index;
    std::string command;
    list l;
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
                l.find(index);
            }
        }
    }
    //fclose(stdin);
    //fclose(stdout);
    return 0;
}