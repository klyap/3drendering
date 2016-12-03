###############################################################################
# CS/CNS 171 Fall 2015
###############################################################################

# Use for MacOS/Linux-specific changes
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    LIBDIR = -L/opt/X11/lib -L/usr/local/lib
else
	LIBDIR = -L/usr/X11R6/lib -L/usr/local/lib
endif

# The following line is a relative directory reference that assumes the Eigen
# folder--which your program will depend on--is located one directory above the
# directory that contains this Makefile.
INCLUDE_EIGEN = -I../
INCLUDE_GL = -I/usr/X11R6/include -I/usr/include/GL -I/usr/include

CC = g++
LIBS = -lGLEW -lGL -lGLU -lglut -lm
FLAGS = -g -std=c++11 $(INCLUDE_EIGEN) $(INCLUDE_GL)
LD_FLAGS = $(LIBDIR) $(LIBS)

EXENAMES = opengl_shader
# Put all sources here (do not include the one that contains main() )
SRCS = object.cpp
OBJS = $(SRCS:.cpp=.o)

# Put other targets here if needed
all: opengl_shader

$(OBJS): %.o : %.h

.cpp.o:
	$(CC) $(FLAGS) -c $<

opengl_shader: $(OBJS)
	$(CC) $(FLAGS) $^ $(LD_FLAGS) $@.cpp -o $@

clean:
	rm -f *.o data/*.ppm $(EXENAMES)
	rm -rf *.dSYM

.PHONY: all clean
