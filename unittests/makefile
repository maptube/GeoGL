#makefile for the unit tests
#g++ -c -w -g -std=c++11 -DLINUX -I../GeoGL -o pointquadtree_test.cpp

FREETYPE_INCLUDE=/usr/include/freetype2

CXXFLAGS=-g -std=c++11 -DLINUX -I../GeoGL -I$(FREETYPE_INCLUDE)

all: pointquadtree_test.exe fp-precision_test.exe

pointquadtree_test.exe: pointquadtree_test.h pointquadtree_test.cpp pointquadtree_test.o ../GeoGL/algorithms/index/pointquadtree.o ../GeoGL/gis/envelope2d.o
	gcc $(CXXFLAGS) -lstdc++ -lm pointquadtree_test.cpp ../GeoGL/algorithms/index/pointquadtree.o ../GeoGL/gis/envelope2d.o -o pointquadtree_test.exe

fp-precision_test.exe: fp-precision_test.cpp fp-precision_test.o
	gcc $(CXXFLAGS) -lstdc++ -lm fp-precision_test.cpp -o fp-precision_test.exe

clean:
	rm pointquadtree_test.exe
	rm pointquadtree_test.o
	rm fp-precision_test.exe
	rm fp-precision_test.o