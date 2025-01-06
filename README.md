# ThreadPool

The ThreadPool library is a simple and efficient way to manage multiple tasks in a program. It allows you to run several tasks at the same time using a pool of worker threads. This can help improve the performance of your program, especially when dealing with many tasks that can be executed concurrently.

**Requirements**
- A C++17 compatible compiler (e.g., GCC 7.1+, Clang 5.0+, MSVC 2017+)
- CMake 3.10 or higher

**To build and install the ThreadPool library, follow these steps:**

Navigate to the LibThreadPool directory:

```
cd LibThreadPool
```

Create a build directory and navigate into it:

```
mkdir build
cd build
```

Configure the build using CMake:

```
cmake ..
```

Compile the library:

```
make
```

Install the library (requires superuser privileges):

```
sudo make install
```

**To test the ThreadPool library, follow these steps:**

Navigate to the LibThreadPool/test directory:

```
cd LibThreadPool/test
```

Create a build directory and navigate into it:

```
mkdir build
cd build
```

Configure the test build using CMake:

```
cmake ..
```

Compile the test:

```
make
```

Execute the test:

```
./test
```
