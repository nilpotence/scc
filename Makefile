CC=gcc
CFLAGS=-g

all: bin bin/scc

bin/scc: bin/main.o bin/tokens.o
	$(CC) -o $@ $^

bin/%.o: src/%.c src/%.h
	$(CC) -c -o $@ $< $(CFLAGS) 
bin:
	mkdir -p $@ 

clean:
	rm -rf bin
