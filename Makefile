# ���������� � �����
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread

# ��� ������������ �����
TARGET = stop_and_wait_test

# �������� �����
SOURCES = main.cpp \
          protocol.cpp \
          logger.cpp \
          test_runner.cpp \
          testsUnit.cpp \
          testIntegration.cpp \
          test_suites.cpp \
          menu.cpp

# ��������� �����
OBJECTS = $(SOURCES:.cpp=.o)

# ������������ �����
HEADERS = common.h \
          protocol.h \
          logger.h \
          test_runner.h \
          testsUnit.h \
          testIntegration.h \
          test_suites.h \
          menu.h

# �������� ����
all: $(TARGET)

# ������ ������������ �����
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# ���������� ��������� ������
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ������� ��������������� ������
clean:
	rm -f $(OBJECTS) $(TARGET)

# ���������� �������
rebuild: clean all

# ������ ���������
run: $(TARGET)
	./$(TARGET)

# ���������� ������
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# �������� ������
release: CXXFLAGS += -O2 -DNDEBUG
release: $(TARGET)

# ��������� ����
.PHONY: all clean rebuild run debug release