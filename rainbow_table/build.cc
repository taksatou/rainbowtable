#include <iostream>
#include <signal.h>

#include "rainbow.hpp"

using namespace std;
using namespace rt;


char *gen_seed(char *buf, int len) {
    for (int i = 0; i < len; ++i) {
        buf[i] = ASCII_BEGIN + rand() % ASCII_COUNT;
    }
    buf[len] = 0;
    return buf;
}

typedef void (*sighandler_t)(int);

bool done = false;

void trap(int x) {
    cout << "exit..." << endl;
    done = true;
}

sighandler_t signal(int signum, sighandler_t handler);

int main(int argc, char **argv) {
    Chain chain;
    char buf[256];

    int tab_num = 1;
    if (argc > 1) {
        tab_num = atoi(argv[1]);
        if (tab_num <= 0)
            tab_num = 1;
    }
    signal(SIGINT, trap);
    chain.init();
    for (int i = tab_num; i <= MAX_LENGTH && !done; ++i) {
        int chains = CHAIN_LENGTH;
        cout << "building table... " << i << endl;
        for (int j = 1; j <= chains && !done; ++j) {
            if (j % 1000 == 0)
                cout << j << " in " << chains << endl;
            gen_seed(buf, i);
            chain.build(buf, i);
        }
    }
}
