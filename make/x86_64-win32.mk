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

SDL_INCLUDE_PATH := ./deps/win64/SDL2/include
SDL_LIBRARY_PATH := ./deps/win64/SDL2/lib

FREETYPE_INCLUDE_PATH := ./deps/win64/freetype/include/freetype2
FREETYPE_LIBRARY_PATH := ./deps/win64/freetype/lib

# Ignored if USE_FONTCONFIG set to false
FONTCONFIG_INCLUDE_PATH := blank
FONTCONFIG_LIBRARY_PATH := blank

# install path
INSTALL_LIBRARY_PATH := /usr/local/lib
INSTALL_HEADERS_PATH := /usr/local/include