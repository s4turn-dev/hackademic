CC=g++
CFLAGS=-std=c++17
SFLAGS=-static -static-libgcc -static-libstdc++
LDFLAGS=-L/crypto -Wl,-Bstatic -lcrypto -lstdc++ -lpthread -Wl,-Bdynamic -lgdi32 -lcrypt32 -lws2_32

SRC=main.cpp $(wildcard encrypt/*.cpp) $(wildcard pers/*.cpp)
OBJ=$(SRC:.cpp=.o)
EXE=hackademic

all: $(EXE)

clean:
	del /S /Q *.o $(EXE).exe 2>nul || exit 0

rebuild: clean all

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(EXE): $(OBJ)
	$(CC) -static $(OBJ) -o $@ $(SFLAGS) $(LDFLAGS)
