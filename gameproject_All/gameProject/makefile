game:*.cpp
	g++ -g -std=c++11 -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive $^ -o $@ -lprotobuf -lzinx -lpthread -lhiredis -static
