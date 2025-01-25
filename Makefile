
TARGET=main
SOURCE=src
INCLUDE=include

LIBS=sdl2
LIB_FLAGS=`pkg-config --libs $(LIBS)`

CXXFLAGS:=$(CXXFLAGS) -std=c++11 -Wall -Wextra -Wpedantic -I$(INCLUDE)

CXXSRC=$(shell find $(SOURCE) -type f -name '*.cpp')
CXXOBJ=$(CXXSRC:.cpp=.o)
CXXDEP=$(CXXSRC:.cpp=.d)

.PHONY: clean test update

all: $(TARGET)

-include $(CXXDEP)

$(TARGET): $(CXXOBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(CXXOBJ) $(LIB_FLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c -o $@ $<

clean:
	$(RM) $(TARGET)
	$(RM) $(SOURCE)/*.o $(SOURCE)/*.d
	$(RM) $(SOURCE)/**/*.o $(SOURCE)/**/*.d
