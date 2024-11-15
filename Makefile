SUBLVL      := ../..
LVL         := $(SUBLVL)/..

INCLUDE_DIR := $(LVL)/include
LIB_DIR     := $(LVL)/lib

CC          := gcc
CXX         := g++

CFLAGS      := -O3 -Wall -W -std=c++11 -DCK_GENERIC -D__USE_GNU -fPIC

INCLUDES    := -I$(INCLUDE_DIR) \
	       -I/usr/include/postgresql

LDFLAGS     := -lpq

SOURCES     := main.cpp \
               IPGSQLDatabase.cpp \
	       PGmisc.cpp
OBJECTS     := $(SOURCES:.cpp=.o)
DEPEND      := $(SOURCES:.cpp=.d)

TARGET      := main

BUILD_DIR := build

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

%.d: %.cpp
	$(CXX) -M $(CFLAGS) $(INCLUDES) $< >> $@.tmp
	mv $@.tmp $@

%.o: %.cpp
	@echo "Current directory: $(shell pwd)"
	$(CXX) -c $(CFLAGS) $(INCLUDES) $< -o $@
	$(CXX) $(CFLAGS) $(INCLUDES) -M -MT $@ $< -o $(@:.o=.d)

clean:
	rm -f $(TARGET) $(OBJECTS) $(DEPEND) $(DEPEND:.d=.d.tmp)
	rm -rf $(BUILD_DIR)  # Если создается подкаталог

