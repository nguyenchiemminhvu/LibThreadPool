#include "threadpool.h"
#include <unistd.h>
#include <iostream>
#include <thread>
#include <future>
#include <chrono>

using namespace std::chrono;

int main()
{
    InitializeThreadPool(4);

    for (int i = 0; i < 10; i++)
    {
        GetThreadPool()->enqueue(0, [i](int val) {
            std::cout << i << " start" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            std::cout << i << " end" << std::endl;
        }, i);
    }

    std::vector<int> future_vector;
    for (int i = 0; i < 10; i++)
    {
        std::promise<int> pr;
        std::future<int> fu = pr.get_future();
        GetThreadPool()->enqueue(0, [i](std::promise<int>& pr) {
            std::this_thread::sleep_for(std::chrono::milliseconds(222));
            pr.set_value(i);
        }, std::ref(pr));

        future_vector.push_back(fu.get());
    }

    std::cout << "Wait a second for future collect data" << std::endl;
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