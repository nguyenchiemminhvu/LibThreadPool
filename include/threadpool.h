#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <iostream>
#include <queue>
#include <chrono>
#include <functional>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>

struct Task
{
    std::chrono::steady_clock::time_point timestamp;
    int priority;
    std::function<void()> func;

    bool operator<(const Task& other) const
    {
        if (timestamp == other.timestamp)
        {
            return priority < other.priority;
        }

        return timestamp > other.timestamp;
    }
};

class ThreadPool
{
public:
    explicit ThreadPool(std::size_t num_threads);
    ~ThreadPool();

    template <class F, class... Args>
    void enqueue(int priority, F&& f, Args&&... args)
    {
        auto timestamp = std::chrono::steady_clock::now();
        Task t = Task{
            .timestamp = timestamp,
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