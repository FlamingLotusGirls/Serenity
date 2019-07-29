CC = gcc
CFLAGS =  -O3 -march=native -std=gnu11 -I. 
LDFLAGS = -lpulse -lsndfile -ljansson -lmicrohttpd
DEPS = saplay.h 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


%: %.o 
	$(CC) -o $@ $^ $(LDFLAGS)

saplay: saplay.o httpd.o stream.o
all: saplay
clean: 
	rm saplay
	rm *.o
