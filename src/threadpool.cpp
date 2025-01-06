#include "threadpool.h"

ThreadPool::ThreadPool(std::size_t num_threads)
    : m_running(true)
{
    for (std::size_t i = 0U; i < num_threads; i++)
    {
        m_workers.emplace_back(&ThreadPool::worker_thread_func, this);
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(m_mut_tasks);
        m_running = false;
    }

    m_condition.notify_all();
    for (std::thread& worker : m_workers)
    {
        worker.join();
    }
}

void ThreadPool::worker_thread_func()
{
    while (true)
    {
        Task t;
        {
            std::unique_lock<std::mutex> lock(m_mut_tasks);
            m_condition.wait(lock, [this]() { return !m_running || !m_tasks.empty(); });

            if (!m_running)
            {
                break;
            }

            t = m_tasks.top();
            m_tasks.pop();
        }

        if (t.func)
        {
            t.func();
        }
    }
}