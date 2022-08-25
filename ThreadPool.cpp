#include "ThreadPool.h"
/*
ThreadPool::ThreadPool() :
    m_thread_count(thread::hardware_concurrency() != 0 ? thread::hardware_concurrency() : 4), m_thread_queues(m_thread_count) {}

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
        t.join();
}*/

res_type ThreadPool::push_task(FuncType f, int*& arr, int low, int high)
{
    lock_guard<mutex> l(m_locker);
    TaskWithPromise twp;
    twp.task = [=, &arr] {f(arr, low, high); };
    res_type res = twp.prom.get_future();

    m_task_queue.push(move(twp));
    m_event_holder.notify_one();

    return res;
    /*int queue_to_push = m_index++ % m_thread_count;
    task_type task = [=, &arr] {f(arr, low, high); };
    m_thread_queues[queue_to_push].push(task);*/
}

void ThreadPool::threadFunc() {
    while (true) {
        TaskWithPromise task_to_do;
        {
            unique_lock<mutex> l(m_locker);
            if (m_task_queue.empty() && !m_work)
                return;
            if (m_task_queue.empty()) {
                m_event_holder.wait(l, [&]() {return !m_task_queue.empty() || !m_work; });
            }
            if (!m_task_queue.empty()) {
                task_to_do = move(m_task_queue.front());
                m_task_queue.pop();
            }
        }
        if (task_to_do.task) {
            task_to_do.task();
            task_to_do.prom.set_value();
        }
    }
}