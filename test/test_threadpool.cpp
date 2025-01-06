#include "threadpool.h"
#include <unistd.h>
#include <iostream>
#include <thread>
#include <future>

using namespace std::chrono_literals;

int main()
{
    ThreadPool pool(4);

    for (int i = 0; i < 10; i++)
    {
        pool.enqueue(0, [i](int val) {
            std::cout << i << " start" << std::endl;
            std::this_thread::sleep_for(2000ms);
            std::cout << i << " end" << std::endl;
        }, i);
    }

    while (true)
    {
        pause();
    }

    return 0;
}