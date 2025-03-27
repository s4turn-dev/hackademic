CC=g++
CFLAGS=-Wall -Werror -Wextra -g
LDFLAGS=-lcrypto

SRC=cryptor.cc main.cc #$(wildcard *.cc)
OBJ=$(SRC:.cc=.o)
EXE=hackademic

all: $(EXE)

clean:
	clear
	rm -rf *.o
	find ./ | grep "\.hackademic" | xargs rm -f

rebuild: clean all

%.o: %.cc
	$(CC) -c $(CFLAGS) $< -o $@

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	./$@

