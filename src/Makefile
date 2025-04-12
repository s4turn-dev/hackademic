ifeq ($(OS), Windows_NT)
	CXX=g++
else
	CXX=x86_64-w64-mingw32-g++
endif

CFLAGS=-std=c++17
SFLAGS=-static -static-libgcc -static-libstdc++
LDFLAGS=-L/crypto -Wl,-Bstatic -lcrypto -lstdc++ -lpthread -Wl,-Bdynamic -lgdi32 -lcrypt32 -lws2_32

SRC=main.cpp $(wildcard encrypt/*.cpp) $(wildcard pers/*.cpp)
OBJ=$(SRC:.cpp=.o)
EXE=hackademic

all: $(EXE)

clean:
ifeq ($(OS), Windows_NT)
	del /S /Q *.o */*.o $(EXE).exe 2>nul || exit 0
else
	rm -rf *.o */*.o $(EXE).exe
endif

rebuild: clean all

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(EXE): $(OBJ)
	$(CXX) -static $(OBJ) -o $@ $(SFLAGS) $(LDFLAGS)
