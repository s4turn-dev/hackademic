CC=g++
CFLAGS=-Wall -Werror -Wextra
LDFLAGS=

SRC=cryptor.cc #$(wildcard *.cc)
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
	./$@ ./

