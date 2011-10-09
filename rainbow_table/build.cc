#include <iostream>

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

int main(int argc, char **argv) {
    Chain chain;
    char buf[256];

    if (argc > 1) {
        cout << "seed" << endl;
        srand(getpid());
    }
    chain.init();
    for (int i = 1; i <= MAX_LENGTH; ++i) {
        int chains = pow(i, 4);
        cout << "building table... " << i << endl;
        for (int j = 1; j <= chains; ++j) {
            if (j % 1000 == 0)
                cout << j << " in " << chains << endl;
            gen_seed(buf, i);
            chain.build(buf, i);
        }
    }
}
