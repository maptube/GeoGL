#Top Level Makefile for GeoGL
# $@ is file name of target rule
# $< name of first prerequisite

#For C++11 you need -std=c++11 or -std=gnu++11 as it's experimental

#Required Library Directories
#JSONCPP_HOME=/home/richard/projects/jsoncpp-src-0.5.0
#sudo dnf install jsoncpp jsoncpp-devel
#installed glfw in /usr/local/lib, so don't need this anymore
#GLFW_HOME=externals/glfw-3.0.4
#GLFW_HOME=/home/richard/projects/glfw-3.0.4
LEAPSDK_HOME=/home/richard/projects/LeapDeveloperKit/LeapSDK
#remember LD_LIBRARY_PATH for poly2tri, otherwise LINUX won't find the .so
POLY2TRI_HOME=poly2tri
#LIBJPEG_HOME=externals/jpeg-9a
#this is a hack to use the library version!!!!!!!!!
LIBJPEG_HOME=/usr/include
FREETYPE_INCLUDE=/usr/include/freetype2
#GTK_INCLUDE=/usr/include/gtk-3.0
#GLIB_INCLUDE=/usr/include/glib-2.0
#GLIB_INCLUDE2=/usr/lib64/glib/include
#GLIB_INCLUDE2=/usr/lib64/glib-2.0/include
#PANGO_INCLUDE=/usr/include/pango-1.0
#CAIRO_INCLUDE=/usr/include/cairo
#PIX-BUF_INCLUDE=/usr/include/gdk-pixbuf-2.0
#ATK_INCLUDE=/usr/include/atk-1.0
#GTKMM_INCLUDE=/usr/include/gtkmm-3.0
WBENNY_LIBZIP_HOME=/home/richard/projects/github/GeoGL/externals/wbenny-ziplib

CC=gcc
CXX=g++
#all errors and warnings
#CFLAGS=-c -Wall
#fatal errors only
CFLAGS=-c -w
CPPFLAGS=-g -std=c++11 -DLINUX
#note json cpp coming from projects folder i.e. not installed
#-ljson_linux-gcc-4.8.2_libmt -lLeap
LDFLAGS=-Wall -lstdc++ -lm -lGL -lGLU -lglut -lGLEW -lglfw -ljsoncpp \
-lpoly2tri -lfreetype -ljpeg -lcurl `pkg-config gtkmm-3.0 --libs` -lpthread -lzip
LDLIBS=-L$(JSONCPP_HOME)/libs/linux-gcc-4.8.2 -L$(LEAPSDK_HOME)/lib/x64 -L$(POLY2TRI_HOME) -L$(LIBJPEG_HOME) -L$(WBENNY_LIBZIP_HOME)/Bin
#-I$(JSONCPP_HOME)/include
#-I$(LEAPSDK_HOME)/include
#-I$(LIBJPEG_HOME)
INCLUDEDIRS=-IGeoGL `pkg-config jsoncpp --cflags` -I$(POLY2TRI_HOME) -I$(FREETYPE_INCLUDE) -I$(LIBJPEG_HOME) \
`pkg-config gtkmm-3.0 --cflags` -I$(WBENNY_LIBZIP_HOME)/Source
SOURCES=$(wildcard GeoGL/*.cpp) $(wildcard GeoGL/gengine/*.cpp) $(wildcard GeoGL/gengine/image/*.cpp) \
$(wildcard GeoGL/gengine/events/*.cpp) $(wildcard GeoGL/cache/*.cpp) $(wildcard GeoGL/async/*.cpp) \
$(wildcard GeoGL/net/*.cpp) $(wildcard GeoGL/gui/*.cpp) $(wildcard GeoGL/clipper/*.cpp) \
$(wildcard GeoGL/abm/*.cpp) $(wildcard GeoGL/abm/sensor/*.cpp) $(wildcard GeoGL/io/*.cpp) $(wildcard usr/*.cpp) \
$(wildcard GeoGL/algorithms/index/*.cpp) $(wildcard GeoGL/gis/*.cpp) $(wildcard GeoGL/text/*.cpp) $(wildcard GeoGL/data/gtfs/*.cpp)
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
