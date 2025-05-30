CXX=g++
CXXFLAGS=-Wall -Werror -Wextra -g
LDFLAGS=-lcrypto -lcpr

WINCXX=x86_64-w64-mingw32-g++
WINCXXFLAGS=-g \
			-DCURL_STATICLIB \
			-DCURL_DISABLE_LDAP \
			-DCURL_DISABLE_LDAPS \
			-DUSE_WIN32_LDAP=OFF \
			-I/opt/static-openssl-win64/include \
			-I/opt/static-curl-win64/include \
			-I/opt/static-libcpr-win64/include
WINLDFLAGS=-L/opt/static-openssl-win64/lib64 \
		   -L/opt/static-curl-win64/lib \
		   -L/opt/static-libcpr-win64/lib \
		   -Wl,-Bstatic \
		     -lcpr -lcurl -lssl -lcrypto -lstdc++ -lpthread \
		   -Wl,-Bdynamic \
		     -lws2_32 -lcrypt32 -lgdi32 -lwinmm -lbcrypt
SFLAGS=-static -static-libgcc -static-libstdc++

SRC=cryptor.cpp main.cpp
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
	#./$@

%.win.o: %.cc
	$(WINCXX) -c $(WINCXXFLAGS) $< -o $@

windows: $(WIN_OBJ)
	$(WINCXX) $^ -o $(EXE).exe $(SFLAGS) $(WINLDFLAGS)
	x86_64-w64-mingw32-objdump -p $(EXE).exe | grep dll
	mv $(EXE).exe ../shared

