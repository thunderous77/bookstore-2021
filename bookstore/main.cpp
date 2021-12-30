#include <iostream>
#include <string>
#include "Express.h"
#include "Error.h"

using namespace std;
Express bookstore;

int main() {
    freopen("1.in", "r", stdin);
    freopen("1.out", "w", stdout);
    string in;
    TokenScanner in_tok;
    bookstore.initialise();
    while (cin) {
        getline(cin, in);
        try {
            if (in.length() > 1024)
                throw Error();
            in_tok.divide(in);
            if (bookstore.express(in_tok)) break;
        } catch (Error) {
            cout << "Invalid\n";
        }
    }
    fclose(stdin);
    fclose(stdout);
    return 0;
}
