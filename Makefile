# Modify to alter build parameters
INCLUDE := -I/usr/include/SDL2 -I/usr/include/fontconfig -I/usr/include/freetype2
LIBS := -lfreetype -lfontconfig
STATIC_LIBS := /usr/lib/x86_64-linux-gnu/libfreetype.a /usr/lib/x86_64-linux-gnu/libfontconfig.a
INSTALL_LIB := /usr/local/lib
INSTALL_HEADERS := /usr/local/include
#################################

CFLAGS := -pedantic -Wall -fpic ${INCLUDE} -lSDL2 ${LIBS}

all:
	mkdir -p ./build
	cc ${CFLAGS} -c -o./build/sdlfox.o ./src/SDL_fox.c
	ar crs ./build/libsdlfox.a ./build/*.o ${STATIC_LIBS}
	cc -shared ${CFLAGS} -Wl,-soname,libsdlfox.so \
		-o./build/libsdlfox.so ./build/*.o ${LIBS}
	cc ./src/example.c -I./src -lsdlfox -L./build/ -o./build/example -lSDL2

install:
	cp -v ./build/*.so ./build/*.a ${INSTALL_LIB}
	cp -v ./src/SDL_fox.h ${INSTALL_HEADERS}

uninstall:
	rm -v ${INSTALL_LIB}/libsdlfox.*
	rm -v ${INSTALL_HEADERS}/SDL_fox.*

clean:
	rm -rv ./build
