CXX=g++
LINK=$(CXX)
TOPDIR=..
INCLUDEPATH=$(TOPDIR)/include
LIBPATH=$(TOPDIR)/lib
OUTPATH=$(TOPDIR)/bin
LDFLAGS= -static-libgcc -static-libstdc++ -Wl,-rpath,$(LIBPATH) 
CXXFLAGS= -std=c++0x -fPIC -g -fno-omit-frame-pointer \
					-DNDEBUG=1 -Wconversion -O3 -Wall -W -fvisibility=hidden

LIBS= -L$(LIBPATH) -lagora-cloud-recording-client

SRC=$(wildcard *.cpp)
OBJ=$(addsuffix .o, $(basename $(SRC)))
TARGET=$(OUTPATH)/cloud_recording_demo.bin

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJ)
	$(LINK) $(LDFLAGS) $^ -o $@ $(LIBS)

%.o:%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< -I$(INCLUDEPATH)

clean:
	rm -f $(TARGET)
	rm -f $(OBJ)
