# Отключаем встроенные правила и суффиксы
.SUFFIXES:
MAKEFLAGS += --no-builtin-rules

ifeq ($(OS), Windows_NT)
	CXX = g++
	LIB = /crypto
	INCLUDE =
	RM = del /S /Q
	MV = move
	EXE = hackademic.exe
else
	CXX = x86_64-w64-mingw32-g++
	LIB = /opt/static-openssl-win64/lib64
	INCLUDE = /opt/static-openssl-win64/include
	RM = rm -rf
	MV = mv
	EXE = hackademic
endif

# Флаги
CXXFLAGS = -std=c++17 $(if $(INCLUDE),-I$(INCLUDE))
SFLAGS   = -static -static-libgcc -static-libstdc++
LDFLAGS  = -L$(LIB) -Wl,-Bstatic -lcrypto -lstdc++ -lpthread -Wl,-Bdynamic -lgdi32 -lcrypt32 -lws2_32

# Файлы
SRC = main.cpp $(wildcard encryption/*.cpp) $(wildcard pers/*.cpp)
OBJ = $(SRC:.cpp=.o)

all: $(EXE)

clean:
	$(RM) *.o */*.o $(EXE) >nul 2>&1 || exit 0

rebuild: clean all

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) $(SFLAGS) $(OBJ) -o $@ $(LDFLAGS)
	$(MV) $(EXE) shared/
