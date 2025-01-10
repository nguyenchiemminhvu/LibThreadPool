CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
TARGET = test_threadpool
TARGET_PTHREADPOOL = test_pthreadpool
HEADERS = threadpool.h

all: $(TARGET) $(TARGET_PTHREADPOOL)

$(TARGET): test_threadpool.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) test_threadpool.cpp -lpthread

$(TARGET_PTHREADPOOL): test_pthreadpool.c $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET_PTHREADPOOL) test_pthreadpool.c -lpthread

clean:
	rm -f $(TARGET) $(TARGET_PTHREADPOOL)