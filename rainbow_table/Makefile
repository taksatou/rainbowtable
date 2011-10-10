all:: solve build util
.PHONY: clean

MYLIB = rainbow.hpp
OPT = -O0 -g

solve: solve.cc $(MYLIB)
	c++ $(OPT) -ltokyocabinet -Wl,-rpath=/usr/lib solve.cc -o solve

build: build.cc $(MYLIB)
	c++ $(OPT) -ltokyocabinet -Wl,-rpath=/usr/lib build.cc -o build

util: util.cc $(MYLIB)
	c++ $(OPT) -ltokyocabinet -Wl,-rpath=/usr/lib util.cc -o util

clean:
	rm -f util solve build *.tch
