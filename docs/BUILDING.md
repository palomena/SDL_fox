# Building SDL_fox

SDL_fox depends on *libsdl2-dev*, *libfontconfig-dev*, *libfreetype-dev*.  
A makefile for debian linux is given inside the repository. If you would
like to build for another platform, you need to adapt the makefile.  

## The easy way (Debian Linux)

1. Install the dependencies
```bash
sudo apt update && sudo apt upgrade
sudo apt install -y libsdl2-dev libfontconfig-dev libfreetype-dev
```

2. Execute the makefile
```bash
cd ./SDL_fox
make
```

Make creates a *./build* directory. Inside of this directory the following
files are put:
- *libsdlfox.a*: sdlfox static library
- *libsdlfox.so*: sdlfox shared library
- *sdlfox.o*: sdlfox object file
- *example*: executable example application

Make expects the shared libraries of the dependencies to be present on
the system. Additionally the following files are expected to exist:
- /usr/lib/x86_64-linux-gnu/libfreetype.a
- /usr/lib/x86_64-linux-gnu/libfontconfig.a

These are the static libraries of freetype and fontconfig.

3. Install
```bash
sudo make install
```
Installing will copy the static and shared library from the *./build*
directory to */usr/local/lib/* and the header files to */usr/local/include/*.  
You can include SDL_fox in your application with `#include <SDL_fox.h>`.  
Link your application with `cc *.c -lSDL2 -lsdlfox`.

4. Remove (optional)
```bash
sudo make uninstall # to uninstall from the system
make clean # to remove local ./build directory
```

## The hard way (Compile everything from source)
This should work on every platform. However it is considerably more
time consuming and might create a few headaches if something does not go
as intended.  

1. Grab and build the dependencies
- [Install SDL2 on your system](http://libsdl.org/download-2.0.php)
- [Download and build fontconfig](https://gitlab.freedesktop.org/fontconfig/fontconfig)
 -[Download and build freetype](https://gitlab.freedesktop.org/freetype/freetype)

Instead of building the stuff yourself you can also grab someone elses binaries.
For freetype i have found this repository:
[Github Link](https://github.com/ubawurinna/freetype-windows-binaries).  
For fontconfig i haven't found anything. Good look. :D

2. Build SDL_fox

Adapt the following makefile which is identical to the SDL_fox Makefile.

```bash
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
```