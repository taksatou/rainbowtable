#include <cstdio>
#include <iostream>
#include "rainbow.hpp"

using namespace std;
using namespace rt;

int main() {
    Chain chain;
    char buf[256];
    int h;

    cout << "hash: " << flush;
    while (fgets(buf, 256, stdin)) {
        int x = atoi(buf);
        bool found = false;
        for (int i = 1; i <= MAX_LENGTH; ++i) {
            set<string> ret = chain.find(x, i);
            if (!ret.empty()) {
                cout << "found: " << ret << endl;
                found = true;
            }
        }
        if (!found)
            cout << "not found" << endl;
        cout << "hash?: " << flush;
    }
}
