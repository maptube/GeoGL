#Nested makefile for poly2tri
# $@ is file name of target rule
# $< name of first prerequisite

#For C++11 you need -std=c++11 or -std=gnu++11 as it's experimental

CC=gcc
CXX=g++
#all errors and warnings
#CFLAGS=-c -Wall
#fatal errors only
CFLAGS=-c -w -fPIC
CPPFLAGS=-g -std=c++11
LDFLAGS=-lstdc++ -lm -shared
LDLIBS=
INCLUDEDIRS=-I common -I sweep
SOURCES=common/shapes.cc sweep/advancing_front.cc sweep/cdt.cc sweep/sweep_context.cc sweep/sweep.cc
OBJECTS=$(SOURCES:.cc=.o)
TARGET=libpoly2tri.so

MAKE=make


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cc.o:
	$(CC) $(CFLAGS) $(INCLUDEDIRS) $< -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
