# Building SDL_fox

SDL_fox depends on *libsdl2-dev*, *libfontconfig-dev*, *libfreetype-dev*.  
A configurable makefile is given inside the repository. If you would
like to build for a specific platform, you need to adapt the makefile.  

### Install dependencies

#### Build-essential tools
- [GNU make](https://www.gnu.org/software/make/)
- [GNU binutils](https://www.gnu.org/software/binutils/)
- A working C compiler with support for std=c99

#### Development libraries
- [libsdl2](http://libsdl.org/)
- [libfreetype](https://freetype.org/)
- [libfontconfig](https://www.freedesktop.org/wiki/Software/fontconfig/)

To obtain development libraries of the dependencies, you can either
compile the dependencies yourself (if you know what you are doing
and have some time and headache to spare) or you can download prebuilt
libraries, which should make your life easier.
Both methods are detailed in the following two subsections.

##### Building required development libraries from source

*libsdl2*

Download the [SDL2 sourcecode](http://libsdl.org/download-2.0.php) by clicking
on one of the links under the "Source Code" section. Follow the build
instructions given inside of the SDL2 sourcecode repository.  
Additional information on installing SDL2 can be found on the 
[SDL Wiki](https://wiki.libsdl.org/Installation).

*libfreetype*

Download the [freetype2 sourcecode](https://freetype.org/developer.html)
and follow the build instructions given inside of the freetype sourcecode
repository.

*libfontconfig*

Download the [fontconfig sourcecode](https://www.freedesktop.org/software/fontconfig/release/)
and follow the build instructions given inside of the fontconfig
sourcecode repository.

##### Downloading prebuilt development libraries

If you are not targeting some obscure system, chances are someone has
already done the job of building the development libraries from source
for you. As a linux user you may benefit from your package manager.
As a Windows user you will probably have to download them manually.

###### Debian-based linux  systems (including Ubuntu)
```sh
sudo apt update && sudo apt upgrade
sudo apt install -y libsdl2-dev libfreetype-dev libfontconfig-dev
```

Other linux distros may use different package managers such as
Pacman (Arch Linux) or yum (Red Hat/Fedora). All of these do also
provide prebuilt development libraries for SDL_fox dependencies.

###### Windows

Choose any of the options listed for each of the dependecies. At least
one of the links listed as bullet points should work for you.

*SDL2*
- [SDL2 Download page](http://libsdl.org/download-2.0.php), section "Development Libraries"/"Windows"

*freetype*
- [MSYS2](https://packages.msys2.org/base/mingw-w64-freetype), section "Binary packages"
- [VS2017](https://github.com/ubawurinna/freetype-windows-binaries)
- [GnuWin32](http://gnuwin32.sourceforge.net/packages/freetype.htm)
- [GTK+](https://sourceforge.net/projects/gtk-mingw/files/freetype/)

*fontconfig (optional)*
- [MSYS2](https://packages.msys2.org/base/mingw-w64-fontconfig), section "Binary packages"

Or use https://github.com/Microsoft/vcpkg/#quick-start-windows for freetype and fontconfig.


### Build with Make

Assuming you have the aforementioned dependencies at hand, you can
now download the repository and start configuring the Makefile.  
The Makefile supplied with the SDL_fox repository currently supports
Windows and Linux as target platforms. You can specify whether to compile
with support for fontconfig.  
Cross-compilation (Linux to Windows) is possible by setting the target
platform to Windows and specifying x86_64-w64-mingw32-gcc as the compiler.

```bash
git clone [REPOSITORY URL]
cd ./SDL_fox
# configure the build target before running make.
# configuration files are in the ./make directory
make CONFIG=target
# where target is the filename without extension
# of a config file in the ./make directory.
# Running make without specifying a CONFIG
# results in make using ./make/default.mk
```

#### Example configuration file for debian linux

Assuming we have installed the required libraries via aptitude.

```Makefile
# Set to true if you want to compile SDL_fox with fontconfig support
# Otherwise set it to false
USE_FONTCONFIG := true

# Compiler to use during compilation
# e.g.: gcc, x86_64-w64-mingw32-gcc, tcc, ...
CC := gcc

# Target platform
# Possible values:
# linux, windows
PLATFORM := linux

SDL_INCLUDE_PATH := /usr/include/SDL2
SDL_LIBRARY_PATH := /usr/lib

FREETYPE_INCLUDE_PATH := /usr/include/freetype2
FREETYPE_LIBRARY_PATH := /usr/lib/x86_64-linux-gnu

# Ignored if USE_FONTCONFIG set to false
FONTCONFIG_INCLUDE_PATH := /usr/include/fontconfig
FONTCONFIG_LIBRARY_PATH := /usr/lib/x86_64-linux-gnu

# install path
INSTALL_LIBRARY_PATH := /usr/local/lib
INSTALL_HEADERS_PATH := /usr/local/include
```

#### Example configuration file for cross compilation with mingw (Linux->Windows)

Assuming we have created a directory *./deps* inside the SDL_fox
repository, in which we have stored the header files and libraries
of our dependencies, following the layout of:  
- *DEPENDENCY_NAME*
	- include/
	- lib/
		- lib*.lib
		- lib*.dll.a
	- bin
		- *.dll

We omit fontconfig in our build (so no fontconfig support)
and only use SDL2 and freetype.

```Makefile
# Set to true if you want to compile SDL_fox with fontconfig support
# Otherwise set it to false
USE_FONTCONFIG := false

# Compiler to use during compilation
# e.g.: gcc, x86_64-w64-mingw32-gcc, tcc, ...
CC := x86_64-w64-mingw32-gcc

# Target platform
# Possible values:
# linux, windows
PLATFORM := windows

#SDL_INCLUDE_PATH := /usr/include/SDL2
#SDL_LIBRARY_PATH := /usr/lib
SDL_INCLUDE_PATH := ./deps/SDL2/include
SDL_LIBRARY_PATH := ./deps/SDL2/lib

#FREETYPE_INCLUDE_PATH := /usr/include/freetype2
#FREETYPE_LIBRARY_PATH := /usr/lib/x86_64-linux-gnu
FREETYPE_INCLUDE_PATH := ./deps/freetype/include/freetype2
FREETYPE_LIBRARY_PATH := ./deps/freetype/lib

# Ignored if USE_FONTCONFIG set to false
FONTCONFIG_INCLUDE_PATH := /usr/include/fontconfig
FONTCONFIG_LIBRARY_PATH := /usr/lib/x86_64-linux-gnu

# install path
INSTALL_LIBRARY_PATH := /usr/local/lib
INSTALL_HEADERS_PATH := /usr/local/include

```

### Example build for Debian Linux

1. Install the dependencies
```bash
sudo apt update && sudo apt upgrade
sudo apt install -y make libsdl2-dev libfontconfig-dev libfreetype-dev
```

2. Execute the makefile
```bash
cd ./SDL_fox
make
# Build the examples (optional)
make examples
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

### Deploying SDL_fox

Alongside with your application binary you need to supply the *SDL_fox* shared
library, *SDL2* shared library and *freetype* shared library (along
with *zlib*). If you built *SDL_fox* with *fontconfig* support you
also need to supply the *fontconfig* shared library.  
You are best off taking the *freetype* and *zlib* libraries
provided by *SDL_ttf* as those have been built with the right
configuration. Otherwise you might run into issues because
a default *freetype* build requires more dependencies like e.g. *harfbuzz*.  
If you compile *freetype* yourself try to go for a minimal dependency build, or ensure the necessary dependecies are present when shipping your application.