all:: solve build util
.PHONY: clean

MYLIB = rainbow.hpp
OPT = -O3 -g
LIBS = -ltokyocabinet -lhashkit

solve: solve.cc $(MYLIB)
	c++ $(OPT) $(LIBS) -Wl,-rpath=/usr/lib solve.cc -o solve

build: build.cc $(MYLIB)
	c++ $(OPT) $(LIBS) -Wl,-rpath=/usr/lib build.cc -o build

util: util.cc $(MYLIB)
	c++ $(OPT) $(LIBS) -Wl,-rpath=/usr/lib util.cc -o util

clean:
	rm -f util solve build *.tch
