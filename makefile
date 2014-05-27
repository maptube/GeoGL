#Top Level Makefile for GeoGL
# $@ is file name of target rule
# $< name of first prerequisite

#For C++11 you need -std=c++11 or -std=gnu++11 as it's experimental

CC=gcc
CXX=g++
#all errors and warnings
#CFLAGS=-c -Wall
#fatal errors only
CFLAGS=-c -w
CPPFLAGS=-g -D LINUX -std=c++11
LDFLAGS=-lstdc++ -lm -lGL -lGLU -lglut -lGLEW
LDLIBS=
INCLUDEDIRS=-IGeoGL -Iexternals/glfw-3.0.4/include
SOURCES=GeoGL/*.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=GeoGLProgram

MAKE=make


all: GeoGL

GeoGL: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(INCLUDEDIRS) $< -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
