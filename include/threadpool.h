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
    explicit ThreadPool(std::size_t num_threads);
    ~ThreadPool();

    template <class F, class... Args>
    void enqueue(int priority, F&& f, Args&&... args)
    {
        Task t = Task{
            .timestamp = std::chrono::steady_clock::now(),
            .priority = priority,
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        };

        {
            std::unique_lock<std::mutex> lock(m_mut_tasks);
            m_tasks.push(t);
        }

        m_condition.notify_one();
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