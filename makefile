#Top Level Makefile for GeoGL
# $@ is file name of target rule
# $< name of first prerequisite

#For C++11 you need -std=c++11 or -std=gnu++11 as it's experimental

#Required Library Directories
JSONCPP_HOME=/home/richard/projects/jsoncpp-src-0.5.0
GLFW_HOME=externals/glfw-3.0.4
LEAPSDK_HOME=/home/richard/projects/LeapDeveloperKit/LeapSDK
POLY2TRI_HOME=poly2tri

CC=gcc
CXX=g++
#all errors and warnings
#CFLAGS=-c -Wall
#fatal errors only
CFLAGS=-c -w
CPPFLAGS=-g -std=c++11 -D LINUX
LDFLAGS=-lstdc++ -lm -lGL -lGLU -lglut -lGLEW
LDLIBS=
INCLUDEDIRS=-IGeoGL -I$(GLFW_HOME)/include -I$(JSONCPP_HOME)/include -I$(LEAPSDK_HOME)/include -I$(POLY2TRI_HOME)
SOURCES=$(wildcard GeoGL/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=GeoGLProgram

MAKE=make


all: GeoGLProgram

GeoGLProgram: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(INCLUDEDIRS) $< -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
