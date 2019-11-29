CC = g++ -std=c++14 -DSEQAN_HAS_ZLIB -DNDEBUG -DSEQAN_DISABLE_VERSION_CHECK -DSEQAN_ENABLE_TESTING=0 -DSEQAN_ENABLE_DEBUG=0 -static -static-libgcc -static-libstdc++
LIB = -Wl,--whole-archive -lpthread -lz -lm -llzma -lbz2 -lrt -Wl,--no-whole-archive
INC = -I./seqan/include

all : diffBAM

diffBAM : diffBAM.cpp
	$(CC) -O3 $^ $(INC) $(LIB) -o $@

clean:
	rm diffBAM
