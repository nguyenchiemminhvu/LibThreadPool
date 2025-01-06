#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <functional>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <atomic>

class ThreadPool
{
public:
    struct Task
    {
        std::chrono::steady_clock::time_point timestamp;
        int priority;
        std::function<void()> func;

        bool operator<(const Task& other) const
        {
            if (priority < other.priority)
                return true;
            if (priority > other.priority)
                return false;
            return timestamp > other.timestamp;
        }
    };

public:
    explicit ThreadPool(std::size_t num_threads = std::thread::hardware_concurrency());
    ~ThreadPool();

    template <class F, class... Args>
    std::future<typename std::result_of<F(Args...)>::type> enqueue(int priority, F&& f, Args&&... args)
    {
        using return_type = typename std::result_of<F(Args...)>::type;

        auto packaged_func = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = packaged_func->get_future();

        Task t = Task{
            .timestamp = std::chrono::steady_clock::now(),
            .priority = priority,
            .func = [packaged_func]() { (*packaged_func)(); }
        };

        {
            std::unique_lock<std::mutex> lock(m_mut_tasks);
            m_tasks.push(t);
        }

        m_condition.notify_one();

        return res;
    }

private:
    void worker_thread_func();

private:
    std::atomic<bool> m_running;
    std::vector<std::thread> m_workers;
    std::priority_queue<Task> m_tasks;
    std::mutex m_mut_tasks;
    std::condition_variable m_condition;
};

#endif // THREAD_POOL_H