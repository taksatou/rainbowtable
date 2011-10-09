#include <tcadb.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <set>
#include </home/takayuki/work/include/utils.hpp>
#include </home/takayuki/work/include/dumper.hpp>

#define ASCII_BEGIN 32
#define ASCII_END 126
#define ASCII_COUNT (ASCII_END - ASCII_BEGIN + 1)
#define CHAIN_LENGTH 65536
#define MAX_LENGTH 12

namespace rt {

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

        void open() {
            if (!tchdbopen(db_, "rainbow.tch", HDBOWRITER | HDBOCREAT)) {
                // fprintf(stderr, "failed to open\n");
                // abort();
            }
        }

        void purge() {
            tchdbvanish(db_);
        }

        char *get(int h) {
            int l = sprintf(buf_, "%d", h);
            int ret = tchdbget3(db_, buf_, l, buf2_, 256);

            if (ret > 0) {
                buf2_[ret] = 0;
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

        char* reduction(char *buf, int len, int h, int idx) {
            if (primes_.empty()) {
                primes_ = primes(CHAIN_LENGTH);
            }

            for (int i = 0; i < len; ++i) {
                h *= primes_[idx];
                buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
            }
            buf[len] = 0;
            return buf;
        }

        int hash(char *s, int len) {
            int ret = 0;

            for (int i = 0; i < len; i++)
                ret = 26 * ret + (s[i]-26);

            return ret;
        }

        void init() {
            tbl_.purge();
        }

        void build(char *seed, int slen) {
            char buf[slen];
            int h;

            tbl_.open();
            strcpy(buf, seed);
            for (int i = 0; i < CHAIN_LENGTH; ++i) {
                h = hash(buf, slen);
                reduction(buf, slen, h, i);
            }
            tbl_.set(h, seed, slen);
        }

        std::set<std::string> find(int h, int slen) {
            char buf[slen];
            char *ret = NULL;
            int org = h;
            std::set<std::string> founds;

            tbl_.open();
            ret = tbl_.get(h);
            int i;
            for (i = 0; i < CHAIN_LENGTH && ret == NULL; ++i) {
                reduction(buf, slen, h, CHAIN_LENGTH - i -1);
                h = hash(buf, slen);
                ret = tbl_.get(h);
                //                std::cout << buf << ',' << h << std::endl;
            }

            if (ret) {
                //                printf(">>found, <<%s, %s>>, %d, %d\n", ret, buf, h, i);
                //                printf("%s\n", ret);
                std::vector<std::string> lis = split(std::string(ret), '');
                for (std::vector<std::string>::iterator it = lis.begin(); it != lis.end(); ++it) {
                    strcpy(buf, it->c_str());
                    //                    printf(">>seed, <<%s>>\n", buf);
                    for (int i = 0; i < CHAIN_LENGTH; ++i) {
                        h = hash(buf, slen);
                        if (h == org) {
                            founds.insert(buf);
                        }
                        reduction(buf, slen, h, i);
                    }
                }
            }

            return founds;
        }
    };
}
