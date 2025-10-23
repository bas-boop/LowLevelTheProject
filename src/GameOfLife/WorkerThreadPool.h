#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <functional>

class WorkerThreadPool
{
public:
    explicit WorkerThreadPool(size_t numThreads);
    ~WorkerThreadPool();

    template<class F>
    void enqueue(F&& f);
    
    bool isIdle() const;
    int getQueueSize() const;

private:
    void threadFunc();
    
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop { false };
    std::atomic<int> activeTasks { 0 };
};

template <class F>
void WorkerThreadPool::enqueue(F&& f)
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.emplace(std::forward<F>(f));
    }
    
    condition.notify_one();
}
