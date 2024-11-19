SRC_DIR      := src
INCLUDE_DIR  := includes
MISC_DIR     := misc
LOGGER_DIR   := logger

CC           := g++
CFLAGS       := -O3 -Wall -W -std=c++11 -DCK_GENERIC -D__USE_GNU -fPIC
INCLUDES     := -I$(INCLUDE_DIR) \
                -I$(MISC_DIR) \
                -I$(LOGGER_DIR) \
                -I/usr/include/postgresql
LDFLAGS      := -lpq

# Файлы
SOURCES      := $(SRC_DIR)/IPGSQLDatabase.cpp \
                $(SRC_DIR)/IPGTable.cpp \
                $(SRC_DIR)/logger.cpp \
                $(MISC_DIR)/misc.cpp \
		main.cpp

HEADERS      := $(INCLUDE_DIR)/IDatabase.hpp \
                $(INCLUDE_DIR)/IDatabase.tpp \
                $(INCLUDE_DIR)/IPGSQLDatabase.hpp \
                $(INCLUDE_DIR)/ITable.hpp \
                $(INCLUDE_DIR)/ITable.tpp \
                $(MISC_DIR)/misc.hpp \
                $(LOGGER_DIR)/logger.hpp
OBJECTS      := $(patsubst %.cpp, build/%.o, $(SOURCES))

TARGET       := main

BUILD_DIR    := build

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)/$(SRC_DIR) $(BUILD_DIR)/$(MISC_DIR) $(BUILD_DIR)/$(LOGGER_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(OBJECTS) $(LDFLAGS)

build/%.o: %.cpp $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

build/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

build/%.o: $(MISC_DIR)/%.cpp $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

build/%.o: $(LOGGER_DIR)/%.cpp $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean

