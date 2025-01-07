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
    static ThreadPool* get_instance(std::size_t num_threads = std::thread::hardware_concurrency())
    {
        static std::once_flag init_instance_flag;
        std::call_once(init_instance_flag, [num_threads]() {
            instance = new ThreadPool(num_threads);
        });

        return instance;
    }

    ~ThreadPool()
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
    explicit ThreadPool(std::size_t num_threads = std::thread::hardware_concurrency())
        : m_running(true)
    {
        for (std::size_t i = 0U; i < num_threads; i++)
        {
            m_workers.emplace_back(&ThreadPool::worker_thread_func, this);
        }
    }

    void worker_thread_func()
    {
        while (m_running)
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

private:
    std::atomic<bool> m_running;
    std::vector<std::thread> m_workers;
    std::priority_queue<Task> m_tasks;
    std::mutex m_mut_tasks;
    std::condition_variable m_condition;

    static ThreadPool* instance;
};

ThreadPool* ThreadPool::instance = nullptr;

#define InitializeThreadPool(n) ThreadPool::get_instance((n))
#define GetThreadPool() ThreadPool::get_instance()

#endif // THREAD_POOL_H