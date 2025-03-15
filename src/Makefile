CC=g++
CFLAGS=-Wall -Werror -Wextra
LDFLAGS=

#SRC=$(wildcard *.cc)
#OBJ=$(SRC:.cc=.o)
TEXT_F=$(filter-out dummy.txt, $(wildcard *.txt))

all: weak

clean:
	rm -rf *.o $(TEXT_F) weak

rebuild: clean all

%.o: %.cc
	$(CC) -c $(CFLAGS) $< -o $@

weak: weak_example.o
	$(CC) $(CFLAGS) $^ -o weak $(LDFLAGS)
	./weak

