# Отключаем встроенные правила и суффиксы
.SUFFIXES:
MAKEFLAGS += --no-builtin-rules

CXXFLAGS = -std=c++17 $(if $(INCLUDE),-I$(INCLUDE))
SFLAGS   = -static -static-libgcc -static-libstdc++
LDFLAGS  = -L$(LIB) -Wl,-Bstatic -lcrypto -lstdc++ -lpthread -Wl,-Bdynamic -lgdi32 -lcrypt32 -lws2_32
EXE		 = hackademic

ifeq ($(OS), Windows_NT)
	CXX = g++
	LIB = /crypto
	INCLUDE =
	RM = del /S /Q
	MV = move
	EXE = hackademic.exe
else
	CXX = x86_64-w64-mingw32-g++
	CXXFLAGS = -DCURL_STATICLIB \
			-DCURL_DISABLE_LDAP \
			-DCURL_DISABLE_LDAPS \
			-DUSE_WIN32_LDAP=OFF \
			-I/opt/static-openssl-win64/include \
			-I/opt/static-curl-win64/include \
			-I/opt/static-libcpr-win64/include
	LDFLAGS = -L/opt/static-openssl-win64/lib64 \
		   -L/opt/static-curl-win64/lib \
		   -L/opt/static-libcpr-win64/lib \
		   -Wl,-Bstatic \
		     -lcpr -lcurl -lssl -lcrypto -lstdc++ -lpthread \
		   -Wl,-Bdynamic \
		     -lws2_32 -lcrypt32 -lgdi32 -lwinmm -lbcrypt

	RM = rm -rf
	MV = mv
	EXE = hackademic.exe
endif

# Файлы
SRC = main.cpp encryption/cryptor.cpp $(wildcard pers/*.cpp)
OBJ = $(SRC:.cpp=.o)


.PHONY: all clean rebuild $(EXE)

all: $(EXE)

clean:
	$(RM) *.o */*.o $(EXE) 

rebuild: clean all

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) $(SFLAGS) $(OBJ) -o $@ $(LDFLAGS)
	$(MV) $(EXE) shared/

