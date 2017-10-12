# for gcc5.1+:
CC=g++
# for clang3.4+:
#CC=clang++
# for emscripten / webgl
#CC=em++

ifeq ($(CC),g++)
	# g++ (ver 5.1+ tested)
	CFLAGS=-c -std=c++14 -g # -O0 #-Wall # sadly we are having a lot of unused functions for tests so this is undesirable...
	LDFLAGS=-g #-lglut -lGLESv2 -lm -g # -O0
else 
ifeq ($(CC),em++)
	# TODO: This is just copy paste from old projects, please change accordingly
	# em++ (ver 1.3+ tested)
	# use 1y instead of 14 here, we need the LLVM libc so that streams are copyable (or a more recent gcc toolchain than the 4.9 at my work!)
	CFLAGS=-c -stdlib=libc++ -std=c++1y -O2
	LDFLAGS=--shell-file shell_minimal.html --preload-file models -lglut -stdlib=libc++ -O2 
else
	# clang++ (ver 3.4+ tested)
	# clang uses 1y instead of 14 here, we need the LLVM libc so that streams are copyable (or a more recent gcc toolchain than the 4.9 at my work!)
	CFLAGS=-c -stdlib=libc++ -std=c++1y -g # -fsanitize=address -fno-omit-frame-pointer
	LDFLAGS=-g -stdlib=libc++ #-lglut -lGLESv2 -lm -stdlib=libc++ -g # -fsanitize=address -fsanitize-memory-track-origins
endif
endif

SOURCES=main.cpp test_creationclass.cpp dxflib/dl_dxf.cpp dxflib/dl_writer_ascii.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=dxf_obj

all: $(SOURCES) $(EXECUTABLE)

# The LDFLAGS should be after the files
# because of some lazyness in newer toolchains!
$(EXECUTABLE): $(OBJECTS) 
# In case of emscripten build, we make a html5/webgl output
ifeq ($(CC),em++)
	$(CC) $(OBJECTS) -o $@.html $(LDFLAGS)
else
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
endif

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o dxflib/*.o $(EXECUTABLE)
