#include <cstdio>
#include <iostream>
#include "rainbow.hpp"

using namespace std;
using namespace rt;

int main(int argc, char **argv) {
    Chain chain;
    char buf[256];
    int h;

    if (argc < 2) {
        cout << "usage: ./util <hash|chain|test>" << endl;
        exit(1);
    }

    if (!strcmp(argv[1], "hash")) {
        cout << "seed?: " << flush;
        while (fgets(buf, 256, stdin)) {
            int len = strlen(buf);
            buf[len-1] = 0;     // remove newline
            cout << buf << " => " << chain.hash(buf, len-1) << endl;
            cout << "key?: " << flush;
        }
    } else if (!strcmp(argv[1], "chain")) {
        if (argc < 3) {
            cout << "usage: ./util chain seed" << endl;
            exit(1);
        }
        strcpy(buf, argv[2]);
        int slen = strlen(buf);
        chain.init(true);
        for (int i = 0; i < CHAIN_LENGTH; ++i) {
            h = chain.hash(buf, slen);
            chain.reduction(buf, slen, h, i);
            cout << " => " << h << " => " << buf << endl;
        }
    } else if (!strcmp(argv[1], "test")) {
        chain.init();

    } else {
        cout << "unknown command" << endl;
    }
}
