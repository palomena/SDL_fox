# Set to true if you want to compile SDL_fox with fontconfig support
# Otherwise set it to false
USE_FONTCONFIG := false

# Compiler to use during compilation
# e.g.: gcc, x86_64-w64-mingw32-gcc, tcc, cc -m32, ...
CC := cc

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