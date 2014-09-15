#rocenting@gmail.com
CC = gcc
CXX = g++
CFLAGS = -O2 -Wall
CXXFLAGS = -g -O2 -frtti -fexceptions -Wall
INCPATH = #-I/usr/include/mysql
LD = gcc
LFLAGS = #-static
LIBS = -lstdc++ -lz -luuid -lpthread

DESTDIR_TARGET = byl_log_agent
OBJECTS = main.o log.o split.o conf.o daemonize.o udp.o collect.o rotate.o

all: $(DESTDIR_TARGET)

$(DESTDIR_TARGET): $(OBJECTS)
	$(LD) $(LFLAGS) -o $(DESTDIR_TARGET) $(OBJECTS) $(LIBS)

####### Implicit rules ######

.SUFFIXES: .cpp .cc .cxx .c
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

clean:
	rm -rf $(DESTDIR_TARGET) *.o
