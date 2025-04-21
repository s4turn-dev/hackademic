CXX=g++
CXXFLAGS=-Wall -Werror -Wextra -g
LDFLAGS=-lcrypto

WINCXX=x86_64-w64-mingw32-g++
WINCXXFLAGS=-g -I/opt/static-openssl-win64/include
WINLDFLAGS=-L/opt/static-openssl-win64/lib64 \
		   -Wl,-Bstatic -lcrypto -lstdc++ -lpthread \
		   -Wl,-Bdynamic -lgdi32 -lcrypt32 -lws2_32
SFLAGS=-static -static-libgcc -static-libstdc++

SRC=cryptor.cc main.cc
UNIX_OBJ=$(SRC:.cc=.unix.o)
WIN_OBJ=$(SRC:.cc=.win.o)

EXE=hackademic

.PHONY: all clean rebuild windows

all: $(EXE)

clean:
	clear
	rm -rf *.o hackademic hackademic.exe
	find ./ | grep "\.hackademic" | xargs rm -f

rebuild: clean all

%.unix.o: %.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(EXE): $(UNIX_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	./$@

%.win.o: %.cc
	$(WINCXX) -c $(WINCXXFLAGS) $< -o $@

windows: $(WIN_OBJ)
	$(WINCXX) $^ -o $(EXE).exe $(SFLAGS) $(WINLDFLAGS)
	x86_64-w64-mingw32-objdump -p $(EXE).exe | grep dll
	mv $(EXE).exe shared

