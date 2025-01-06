#include "threadpool.h"
#include <unistd.h>
#include <iostream>
#include <thread>
#include <future>
#include <chrono>

using namespace std::chrono;

int main()
{
    ThreadPool pool(4);

    for (int i = 0; i < 10; i++)
    {
        pool.enqueue(0, [i](int val) {
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
        pool.enqueue(0, [i](std::promise<int>& pr) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

    while (true)
    {
        pause();
    }

    return 0;
}