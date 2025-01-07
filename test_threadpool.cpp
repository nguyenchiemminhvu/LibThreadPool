#include "threadpool.h"
#include <unistd.h>
#include <iostream>
#include <thread>
#include <future>
#include <chrono>

using namespace std::chrono;

static bool thread_pool_init_func = []()
{
    InitializeThreadPool(4);
    return true;
}();

int main()
{
    for (int i = 0; i < 8; i++)
    {
        GetThreadPool()->enqueue(0, [i](int val) {
            std::cout << val << " start" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            std::cout << val << " end" << std::endl;
        }, i);
    }

    std::vector<int> future_vector;
    for (int i = 0; i < 10; i++)
    {
        std::promise<int> pr;
        std::future<int> fu = pr.get_future();
        GetThreadPool()->enqueue(0, [i](std::promise<int>& pr) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            pr.set_value(i);
        }, std::ref(pr));

        future_vector.push_back(fu.get());
    }

    std::cout << "Future vector: ";
    for (int val : future_vector)
    {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    auto fu = GetThreadPool()->enqueue(0, []() -> int {
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        return 100;
    });

    std::cout << "Result returned by direct future assignment: " << fu.get() << std::endl;

    while (true)
    {
        pause();
    }

    return 0;
}