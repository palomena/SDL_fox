###############################################################################
# Makefile for SDL_fox
###############################################################################

###############################################################################
# USER CONFIGURATION //
###############################################################################

CONFIG := default

###############################################################################
# // END OF USER CONFIGURATION
###############################################################################

include ./make/${CONFIG}.mk

ifeq (${PLATFORM}, windows)
SOEXT := dll
AREXT := lib
else
SOEXT := so
AREXT := a
endif

# SDL_fox sourcecode build options
# These options are passed to the compiler and mainly concern conditional
# compilation/dependencies.
# Possible values:
# -DFOX_USE_FONTCONFIG : Build SDL_fox with fontconfig support
ifeq (${USE_FONTCONFIG}, true)
BUILDOPT += -DFOX_USE_FONTCONFIG
endif

ifeq (${PLATFORM}, windows)
BUILDOPT += -DSDL_MAIN_HANDLED
endif

# Compiler include path
INCLUDE_PATH  := -I${SDL_INCLUDE_PATH} -I${FREETYPE_INCLUDE_PATH}

# Compiler libraries path
LIBRARY_PATH := -L${SDL_LIBRARY_PATH} -L${FREETYPE_LIBRARY_PATH}

# Linker
LINK := -lSDL2 -lfreetype

# Static libraries
STATIC_LIBRARIES := ${FREETYPE_LIBRARY_PATH}/libfreetype.${AREXT}

ifeq (${USE_FONTCONFIG}, true)
INCLUDE_PATH += -I${FONTCONFIG_INCLUDE_PATH}
LIBRARY_PATH += -L${FONTCONFIG_LIBRARY_PATH}
LINK += -lfontconfig
STATIC_LIBRARIES += ${FONTCONFIG_LIBRARY_PATH}/libfontconfig.${AREXT}
endif

CFLAGS := -pedantic -Wall ${INCLUDE_PATH} ${LIBRARY_PATH} ${BUILDOPT}

###############################################################################
# all
######
# Builds the SDL_fox library.
# (1) Creates the local ./build directory
# (2) Compiles ./src/SDL_fox.c into an object file ./build/sdlfox.o
# (3) Creates the static library using dependency static libraries
# (4) Creates the shared library
###############################################################################


all:
	echo ${CC}
	# (1) Create build directory
	mkdir -p ./build

	# (2) Create object file
	${CC} -c -fpic -o./build/sdlfox.o ./src/SDL_fox.c ${CFLAGS}

	# (3) Create static library
	ar crs ./build/libsdlfox.${AREXT} ./build/*.o ${STATIC_LIBRARIES}

	# (4) Create shared library
	${CC} -shared -Wl,-soname,libsdlfox.${SOEXT} ./build/*.o \
			-o./build/libsdlfox.${SOEXT} ${CFLAGS} ${LINK}

###############################################################################
# examples
###########
# (5) Compiles the examples ./src/examples/ into an executable ./build/examples
###############################################################################
examples:
	# (5) Compile examples
	mkdir -p ./build/examples
	${CC} ./src/examples/scrolling.c -I./src -L./build/ -lsdlfox \
			-o./build/examples/scrolling ${CFLAGS} ${LINK}
	${CC} ./src/examples/simple.c -I./src -L./build/ -lsdlfox \
			-o./build/examples/simple ${CFLAGS} ${LINK}
	cp -v ./src/bin/*.ttf ./build/examples/

###############################################################################
# install
##########
# The install job copies the locally built library and its header files
# to the system path for libraries and header files.
# Running this job may require elevated privileges.
###############################################################################

install:
	cp -v ./build/libsdlfox.* ${INSTALL_LIBRARY_PATH}
	cp -v ./src/SDL_fox.h ${INSTALL_HEADERS_PATH}

###############################################################################
# uninstall
############
# The uninstall job reverts the install job by removing all files deployed
# by the install job from the system. May require elevated privileges.
###############################################################################

uninstall:
	rm -v ${INSTALL_LIBRARY_PATH}/libsdlfox.*
	rm -v ${INSTALL_HEADERS_PATH}/SDL_fox.*

###############################################################################
# clean
########
# The clean job deletes the local ./build directory and thus
# all files that may have been built with this Makefile prior.
###############################################################################

clean:
	rm -rv ./build

###############################################################################
# release
##########
# Copies build to bin
###############################################################################

release:
	mkdir -p ./bin
	mkdir -p ./bin/${CONFIG}
	cp -v ./build/*.${SOEXT} ./build/*.${AREXT} ./bin/${CONFIG}