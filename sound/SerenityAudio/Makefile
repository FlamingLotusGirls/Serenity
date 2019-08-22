CC = gcc
CFLAGS =  -O3 -march=native -std=gnu11 -I. 
LDFLAGS = -lpulse -lsndfile -ljansson -lmicrohttpd -lcurl

# supposedly uses the GCC built-in sanitizer
#CFLAGS =  -g -O3 -fsanitize=undefined -march=native -std=gnu11 -I. 
#LDFLAGS = -fsanitize=undefined -lpulse -lsndfile -ljansson -lmicrohttpd -lcurl

DEPS = saplay.h 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


%: %.o 
	$(CC) -o $@ $^ $(LDFLAGS)

saplay: saplay.o httpd.o http.o stream.o filedb.o scape.o sink.o
all: saplay
clean: 
	rm saplay
	rm *.o
