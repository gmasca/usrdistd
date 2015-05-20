#############################################################################
#
# MAKE FILE - usrdistd
#
#############################################################################

CPP = g++ -std=c++11
CC = gcc


CF_OpenCV = $(shell pkg-config --cflags opencv)
LIB_OpenCV = $(shell pkg-config --libs opencv)
LIB_Math = -lm
LIB_PThread = -lpthread

userdistd: main.cpp ffrec.o httpmodule.o
	$(CPP) $(CF_OpenCV) -c main.cpp
	$(CPP) *.o $(LIB_OpenCV) $(LIB_Math) $(LIB_PThread) -o usrdistd

ffrec.o: ffrec.cpp ffrec.hpp
	$(CPP) $(CF_OpenCV) -c ffrec.cpp

httpmodule.o: httpmodule.cpp httpmodule.hpp libs/wrap.c libs/utils.c libs/utils.h libs/socklib.h libs/udpf.c libs/tcpf.c
	$(CC)  -c libs/*.c 
	$(CPP) -c httpmodule.cpp

clean:
	rm -f *.o

install:
	@>$$HOME/.usrdistenv
	