#include <tcadb.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <set>
#include </home/takayuki/work/include/utils.hpp>
#include </home/takayuki/work/include/dumper.hpp>

// #define ASCII_BEGIN 32
// #define ASCII_END 126
#define ASCII_BEGIN 48
#define ASCII_END 122
#define ASCII_COUNT (ASCII_END - ASCII_BEGIN + 1)
#define CHAIN_LENGTH 10000000
#define MAX_LENGTH 8

namespace rt {

    // class Func {};

    // class Reductor : public Func {
    //     std::vector<int> primes_;

    // public:

    //     Reductor() {
    //         primes_ = primes(CHAIN_LENGTH);
    //     }

    //     char* operator()(char *buf, int len, int h, int idx) {
    //         for (int i = 0; i < len; ++i) {
    //             h *= primes_[idx];
    //             buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
    //         }
    //         buf[len] = 0;
    //         return buf;
    //     }
    // };

    // class Hash : public Func {
    // public:

    //     int operator()(char *s, int len) {
    //         int ret = 0;

    //         for (int i = 0; i < len; i++)
    //             ret = 26 * ret + (s[i]-26);

    //         return ret;
    //     }
    // };

    class RainbowTable {
    private:
        TCHDB *db_;
        char buf_[256];
        char buf2_[256];

    public:
        RainbowTable() {
            db_ = tchdbnew();
        }

        ~RainbowTable() {
            tchdbclose(db_);
            tchdbdel(db_);
        }

        void open(bool readonly=false) {
            if (readonly) {
                if (!tchdbopen(db_, "rainbow.tch", HDBOREADER)) {
                    fprintf(stderr, "failed to open\n");
                    abort();
                }
            } else {
                if (!tchdbopen(db_, "rainbow.tch", HDBOWRITER | HDBOCREAT | HDBONOLCK)) {
                    fprintf(stderr, "failed to open\n");
                    abort();
                }
            }
        }

        void purge() {
            tchdbvanish(db_);
        }

        char *get(int h) {
            int l = sprintf(buf_, "%d", h);
            int ret = tchdbget3(db_, buf_, l, buf2_, 256);

            if (ret > 0) {
                //                buf2_[ret] = 0;
                return buf2_;
            } else
                return NULL;
        }

        bool set(int h, char *txt, int siz) {
            int l = sprintf(buf_, "%d", h);
            txt[siz] = '';
            txt[siz+1] = 0;
            return tchdbputcat(db_, buf_, l, txt, siz+1);
        }
    };

    class Chain {
    private:
        RainbowTable tbl_;
        std::vector<int> primes_;

    public:

        // 204235  204235  811240
        inline char* reduction0(char *buf, int len, int h, int idx) {
            for (int i = 0; i < len; ++i) {
                h = h * primes_[idx] + h % primes_[idx+1];
                buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
            }
            buf[len] = 0;
            return buf;
        }

        //  105471  105471  416114
        inline char* reduction1(char *buf, int len, int h, int idx) {
            for (int i = 0; i < len; ++i) {
                h = h * primes_[idx];
                buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
            }
            buf[len] = 0;
            return buf;
        }

        //    6790    6790   25910
        inline char* reduction2(char *buf, int len, int h, int idx) {
            for (int i = 0; i < len; ++i) {
                h += idx;
                buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
            }
            buf[len] = 0;
            return buf;
        }

        // 65210   65210  255074
        inline char* reduction3(char *buf, int len, int h, int idx) {
            for (int i = 0; i < len; ++i) {
                h += h ^ primes_[idx];
                buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
            }
            buf[len] = 0;
            return buf;
        }

        //  262312  262312 1043491
        inline char* reduction4(char *buf, int len, int h, int idx) {
            for (int i = 0; i < len; ++i) {
                h = h * primes_[idx] + (h % primes_[idx+1]) * (h % primes_[idx+2]) + h % primes_[idx+3];
                buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
            }
            buf[len] = 0;
            return buf;
        }


        inline char* reduction(char *buf, int len, int h, int idx) {
            for (int i = 0; i < len; ++i) {
                h = bit_invert32(h) * primes_[idx] + (h % primes_[idx+1]);
                buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
            }
            buf[len] = 0;
            return buf;
        }

        inline int hash(char *s, int len) {
            int ret = 0;

            for (int i = 0; i < len; i++)
                ret = 26 * ret + (s[i]-26);

            return ret;
        }

        void init(bool readonly=false) {
            if (readonly) {
                tbl_.open(readonly);
            } else {
                tbl_.open();
                tbl_.purge();
            }
            primes_ = primes(CHAIN_LENGTH+10);
            //            std::cout << primes_ << std::endl;
        }

        void build(char *seed, int slen) {
            char buf[slen];
            int h;

            strcpy(buf, seed);
            for (int i = 0; i < CHAIN_LENGTH; ++i) {
                h = hash(buf, slen);
                //                printf("%s => %d(%d)\n", buf, h, i);
                reduction(buf, slen, h, i);
            }
            h = hash(buf, slen);
            //            printf("%s => %d\n", buf, h);
            tbl_.set(h, seed, slen);
        }

        std::set<std::string> find(int h, int slen) {
            char buf[slen];
            char *ret = NULL;
            int org = h;
            std::set<std::string> founds;

            //            ret = tbl_.get(h);
            int offset;
            for (offset = 0; offset <= CHAIN_LENGTH && ret == NULL; ++offset) {
                h = org;
                for (int i = offset; i < CHAIN_LENGTH; ++i) {
                    reduction(buf, slen, h, i);
                    //                    printf("%d => %s (%d)\n", h, buf, i);
                    h = hash(buf, slen);
                }
                if (offset % 1000 == 0)
                    printf("searching... %d/%d\n", offset, CHAIN_LENGTH);
                ret = tbl_.get(h);
            }


            if (ret) {
                std::vector<std::string> lis = split(std::string(ret), '');
                //               printf("found hash: %d\n", h);
                //                printf(">>found, <<%s, %s>>, %d, %d\n", ret, buf, h, i);
                // std::cout << lis << std::endl;
                int h2 = h;
                for (std::vector<std::string>::iterator it = lis.begin(); it != lis.end(); ++it) {
                    if (it->size() != slen) continue;

                    strcpy(buf, it->c_str());
                    //                    printf("found hash: %d, %s\n", h2, buf);

                    //                    printf(">>seed, <<%s>>\n", buf);
                    for (int i = 0; i < CHAIN_LENGTH; ++i) {
                        h = hash(buf, slen);
                        if (h == org) {
                            founds.insert(buf);
                        }
                        reduction(buf, slen, h, i);
                    }
                    h = hash(buf, slen);
                    if (h == org) {
                        founds.insert(buf);
                    }

                }
            }

            return founds;
        }
    };
}
