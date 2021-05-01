a.out: parser.hpp main.cpp
	g++ -flto -O3 -lfmt -std=c++20 -Wall -Werror -Wpedantic main.cpp
