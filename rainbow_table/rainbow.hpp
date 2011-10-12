#include <tcadb.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <set>
#include <libhashkit/hashkit.h>
#include </home/takayuki/work/include/utils.hpp>
#include </home/takayuki/work/include/dumper.hpp>

// #define ASCII_BEGIN 32
// #define ASCII_END 126
#define ASCII_BEGIN 48
#define ASCII_END 122
#define ASCII_COUNT (ASCII_END - ASCII_BEGIN + 1)
#define CHAIN_LENGTH 100000
#define MAX_LENGTH 7

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
        Hashkit h;
        RainbowTable tbl_;
        std::vector<int> primes_;
        char *magic_seeds_[CHAIN_LENGTH + 10];

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

        //  bench v2 from here
        //  322574  322574 1284526
        inline char* reduction5(char *buf, int len, int h, int idx) {
            for (int i = 0; i < len; ++i) {
                h = bit_invert32(h) * primes_[idx] + (h % primes_[idx+1]);
                buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
            }
            buf[len] = 0;
            return buf;
        }

        // 305335  305335 1215570
        inline char* reduction6(char *buf, int len, int h, int idx) {
            int x = primes_[idx];
            for (int i = 0; i < len; ++i) {
                h = bit_invert32(h) * x + (h % primes_[(unsigned int)(x + h) % (CHAIN_LENGTH-1)]);
                x = primes_[(unsigned int)h % (CHAIN_LENGTH-1)];
                buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
            }
            buf[len] = 0;
            return buf;
        }


        //  ./build > out  32.84s user 0.43s system 98% cpu 33.795 total
        //  131880  131880  521833
        inline char* reduction7(char *buf, int len, int h, int idx) {
            srand(idx);
            for (int i = 0; i < len; ++i) {
                h = bit_invert32(h) * primes_[rand() % (CHAIN_LENGTH - 1)];
                buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
            }
            buf[len] = 0;
            return buf;
        }

        // ./build > out  17.33s user 0.32s system 97% cpu 18.083 total
        //  309061  309061 1230474
        inline char* reduction8(char *buf, int len, int h, int idx) {
            for (int i = 0; i < len; ++i) {
                h = bit_invert32(h) * primes_[idx] + (h % primes_[idx+1]);
                buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
            }
            buf[len] = 0;

            h = libhashkit_digest(buf, len, HASHKIT_HASH_JENKINS);

            for (int i = 0; i < len; ++i) {
                h = bit_invert32(h) * primes_[idx+2] + (h % primes_[idx+3]);
                buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
            }
            buf[len] = 0;
            return buf;
        }

        // ./build > out  17.18s user 0.41s system 91% cpu 19.196 total
        //  294662  294662 1172878
        inline char* reduction9(char *buf, int len, int h, int idx) {
            for (int i = 0; i < len; ++i) {
                h = bit_invert32(h) * primes_[idx] + (h % primes_[idx+1]);
                buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
            }
            buf[len] = 0;

            h = libhashkit_digest(buf, len, HASHKIT_HASH_MURMUR);

            for (int i = 0; i < len; ++i) {
                h = bit_invert32(h) * primes_[idx+2] + (h % primes_[idx+3]);
                buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
            }
            buf[len] = 0;
            return buf;
        }

        //  ./build > out  7.75s user 0.47s system 85% cpu 9.627 total
        //  244425  244425  971930
        inline char* reduction10(char *buf, int len, int h, int idx) {
            switch (idx % 4) {
            case 0:
                for (int i = 0; i < len; ++i) {
                    h = bit_invert32(h) * primes_[idx] + (h % primes_[idx+1]);
                    buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
                }
                break;
            case 1:
                for (int i = 0; i < len; ++i) {
                    h = bit_invert32(h) * primes_[idx];
                    buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
                }
                break;
            case 2:
                for (int i = 0; i < len; ++i) {
                    h *= primes_[idx+i];
                    buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
                }
                break;
            default:
                for (int i = 0; i < len; ++i) {
                    h = h * primes_[idx] + h % primes_[idx+1];
                    buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
                }
                break;
            }

            buf[len] = 0;
            return buf;
        }

        // ./build > out  5.01s user 0.54s system 79% cpu 7.008 total
        //   88667   88667  348984
        inline char* reduction11(char *buf, int len, int h, int idx) {
            int a, b, c, d;
            for (int i = 0; i < len; ++i) {
                h *= primes_[idx];
                // a = h & 0x0000FFFF;
                // b = h & 0xFFFF0000;
                c = h & 0x00FF00FF;
                d = h & 0xFF00FF00;
                h = (c << 8) | (b >> 8);
                buf[i] = ASCII_BEGIN + abs(h % ASCII_COUNT);
            }
            buf[len] = 0;
            return buf;
        }

        inline char* reduction(char *buf, int len, int h, int idx) {
            return reduction5(buf, len, h, idx);
        }


        inline int hash(char *s, int len) {
            int ret = 0;

            for (int i = 0; i < len; i++)
                ret = 26 * ret + (s[i]-26);

            return ret;
        }

        void delete_magic_seed(int n) {
            for (int i = 0; i < n; ++i) {
                free(magic_seeds_[i]);
            }
        }

        void init_magic_seed(int n, hashkit_hash_algorithm_t a) {
            // int seed_size = 256;
            // buff
            // for (int i = 0; i < n; ++i) {
            //     magic_seeds_[i] = malloc(seed_size);
            // }
        }

        void init(bool readonly=false) {
            if (readonly) {
                tbl_.open(readonly);
            } else {
                tbl_.open();
                tbl_.purge();
            }
            primes_ = get_primes2(CHAIN_LENGTH+10);
            //h.set_function(HASHKIT_HASH_JENKINS);
            //            init_magic_seed();
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
