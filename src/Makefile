CC=g++
CFLAGS=-Wall -Werror -Wextra -std=c++17 
SFLAGS=-static -static-libgcc -static-libstdc++
LDFLAGS=-lcrypto -lgdi32

# g++ main.cpp -static -static-libgcc -static-libstdc++ -lgdi32 -o my_program.exe


SRC=$(wildcard encrypt/*.cpp) $(wildcard pers/*.cpp)
OBJ=$(SRC:.cpp=.o)
EXE=hackademic

all: $(EXE)

clean:
	for /R %f in (*.o) do del "%f"
	del $(EXE).exe

rebuild: clean all

%.o: %.cc
	$(CC) -c $(CFLAGS) $< -o $@

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(SFLAGS)

