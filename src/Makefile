ifeq ($(OS), Windows_NT)
	CXX=g++
	LIB=/crypto
	INCLUDE=
else
	CXX=x86_64-w64-mingw32-g++
	LIB=/opt/static-openssl-win64/lib64
	INCLUDE=/opt/static-openssl-win64/include
endif

CXXFLAGS=-std=c++17 -I$(INCLUDE)
SFLAGS=-static -static-libgcc -static-libstdc++
LDFLAGS=-L/$(LIB) -Wl,-Bstatic -lcrypto -lstdc++ -lpthread -Wl,-Bdynamic -lgdi32 -lcrypt32 -lws2_32

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
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXE): $(OBJ)
	$(CXX) -static $(OBJ) -o $@ $(SFLAGS) $(LDFLAGS)
