# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread

# Имя исполняемого файла
TARGET = stop_and_wait_test

# Исходные файлы
SOURCES = main.cpp \
          protocol.cpp \
          logger.cpp \
          test_runner.cpp \
          testsUnit.cpp \
          testIntegration.cpp \
          test_suites.cpp \
          menu.cpp

# Объектные файлы
OBJECTS = $(SOURCES:.cpp=.o)

# Заголовочные файлы
HEADERS = common.h \
          protocol.h \
          logger.h \
          test_runner.h \
          testsUnit.h \
          testIntegration.h \
          test_suites.h \
          menu.h

# Основная цель
all: $(TARGET)

# Сборка исполняемого файла
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Компиляция объектных файлов
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Очистка сгенерированных файлов
clean:
	rm -f $(OBJECTS) $(TARGET)

# Пересборка проекта
rebuild: clean all

# Запуск программы
run: $(TARGET)
	./$(TARGET)

# Отладочная сборка
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Релизная сборка
release: CXXFLAGS += -O2 -DNDEBUG
release: $(TARGET)

# Фальшивые цели
.PHONY: all clean rebuild run debug release