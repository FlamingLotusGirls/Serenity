CC = gcc
CFLAGS =  -O3 -march=native -std=gnu11 -I. 
LDFLAGS = -lpulse -lsndfile -ljansson -lmicrohttpd -lcurl
DEPS = saplay.h 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


%: %.o 
	$(CC) -o $@ $^ $(LDFLAGS)

saplay: saplay.o httpd.o http.o stream.o filedb.o
all: saplay
clean: 
	rm saplay
	rm *.o
