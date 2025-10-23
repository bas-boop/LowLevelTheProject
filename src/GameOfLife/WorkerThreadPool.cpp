#include "WorkerThreadPool.h"

void WorkerThreadPool::threadFunc()
{
    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this]
            {
                return stop || !tasks.empty();
            });
            
            if (stop && tasks.empty())
                return;
            
            task = std::move(tasks.front());
            tasks.pop();
        }
        
        ++activeTasks;
        task();
        --activeTasks;
    }
}

WorkerThreadPool::WorkerThreadPool(size_t numThreads)
{
    for (size_t i = 0; i < numThreads; ++i)
        workers.emplace_back(&WorkerThreadPool::threadFunc, this);
}

bool WorkerThreadPool::isIdle() const
{
    return activeTasks == 0 && tasks.empty();
}

int WorkerThreadPool::getQueueSize() const
{
    std::unique_lock<std::mutex> lock(const_cast<std::mutex&>(queueMutex));
    return tasks.size();
}

WorkerThreadPool::~WorkerThreadPool()
{
    stop = true;
    condition.notify_all();
    
    for (std::thread& worker : workers)
        if (worker.joinable()) worker.join();
}
