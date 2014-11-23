#Top Level Makefile for GeoGL
# $@ is file name of target rule
# $< name of first prerequisite

#For C++11 you need -std=c++11 or -std=gnu++11 as it's experimental

#Required Library Directories
JSONCPP_HOME=/home/richard/projects/jsoncpp-src-0.5.0
#installed glfw in /usr/local/lib, so don't need this anymore
#GLFW_HOME=externals/glfw-3.0.4
#GLFW_HOME=/home/richard/projects/glfw-3.0.4
LEAPSDK_HOME=/home/richard/projects/LeapDeveloperKit/LeapSDK
POLY2TRI_HOME=poly2tri
LIBJPEG_HOME=externals/jpeg-9a
FREETYPE_INCLUDE=/usr/include/freetype2

CC=gcc
CXX=g++
#all errors and warnings
#CFLAGS=-c -Wall
#fatal errors only
CFLAGS=-c -w
CPPFLAGS=-g -std=c++11 -DLINUX
#note json cpp coming from projects folder i.e. not installed
LDFLAGS=-Wall -lstdc++ -lm -lGL -lGLU -lglut -lGLEW -lglfw -ljson_linux-gcc-4.8.2_libmt\
-lLeap -lpoly2tri -lfreetype -ljpeg
LDLIBS=-L$(JSONCPP_HOME)/libs/linux-gcc-4.8.2 -L$(LEAPSDK_HOME)/lib/x64 -L$(POLY2TRI_HOME) -L$(LIBJPEG_HOME)
INCLUDEDIRS=-IGeoGL -I$(JSONCPP_HOME)/include -I$(LEAPSDK_HOME)/include -I$(POLY2TRI_HOME) -I$(FREETYPE_INCLUDE) -I$(LIBJPEG_HOME)
SOURCES=$(wildcard GeoGL/*.cpp) $(wildcard GeoGL/gengine/*.cpp) $(wildcard GeoGL/gengine/image/*.cpp) \
$(wildcard GeoGL/gengine/events/*.cpp) $(wildcard GeoGL/cache/*.cpp) $(wildcard GeoGL/async/*.cpp) $(wildcard GeoGL/clipper/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=bin/GeoGLProgram

MAKE=make


all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDLIBS) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(INCLUDEDIRS) $< -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
