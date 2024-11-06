# Имя выходного исполняемого файла
TARGET = main

# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++11 -Wall -I/usr/include/postgresql
LDFLAGS = -lpq  # флаг для подключения библиотеки libpq (для работы с PostgreSQL)

# Файлы исходного кода и объектные файлы
SRC = main.cpp ipgqsldatabase.cpp
OBJ = $(SRC:.cpp=.o)

# Правило по умолчанию: сборка программы
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

# Правило для сборки объектных файлов
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Очистка объектных файлов и исполняемого файла

