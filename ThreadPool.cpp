#include "ThreadPool.h"

ThreadPool::ThreadPool() :
    m_thread_count(thread::hardware_concurrency() != 0 ? thread::hardware_concurrency() : 4), 
    m_thread_queues(m_thread_count), m_index(0)
{
    start();
}

ThreadPool::~ThreadPool()
{
    stop();
}

void ThreadPool::start() 
{
    for (int i = 0; i < m_thread_count; i++) 
        m_threads.emplace_back(&ThreadPool::threadFunc, this, i);
}

void ThreadPool::stop() 
{
    for (int i = 0; i < m_thread_count; i++) 
    {
        task_type empty_task;
        m_thread_queues[i].push(empty_task);
    }
    for (auto& t : m_threads)
        if (t.joinable())
            t.join();
}

res_type ThreadPool::push_task(FuncType f, int*& arr, int low, int high)
{
    int queue_to_push = m_index++ % m_thread_count;
    task_type task ([=, &arr] {f(arr, low, high); });
    res_type res = task.get_future();
    m_thread_queues[queue_to_push].push(task);
    return res;
}

void ThreadPool::threadFunc(int qindex) 
{
    while (true) 
    {
        task_type task_to_do;
        bool res;
        int i = 0;
        for (; i < m_thread_count; i++) 
            if (res = m_thread_queues[(qindex + i) % m_thread_count].fast_pop(task_to_do))
                break;
        if (!res) 
            m_thread_queues[qindex].pop(task_to_do);
        else if (!task_to_do.valid()) 
            m_thread_queues[(qindex + i) % m_thread_count].push(task_to_do);
        if (!task_to_do.valid()) 
            return;
        task_to_do();
    }
}

void ThreadPool::pending_task()
{
    task_type task_to_do;
    bool res;
    int i = 0;
    for (; i < m_thread_count; ++i)	
    {
        res = m_thread_queues[i % m_thread_count].fast_pop(task_to_do);
        if (res) break;
    }

    if (!task_to_do.valid())	
        std::this_thread::yield();
    else
        task_to_do();
}