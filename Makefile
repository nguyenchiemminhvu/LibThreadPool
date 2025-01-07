CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
TARGET = test_threadpool
SRCS = test_threadpool.cpp
HEADERS = threadpool.h

all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) -lpthread

clean:
	rm -f $(TARGET)