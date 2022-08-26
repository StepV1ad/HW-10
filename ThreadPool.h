#pragma once
#include "BlockedQueue.h"
#include <vector>
#include <functional>
#include <thread>
#include <future>

typedef std::packaged_task<void()> task_type;
typedef void (*FuncType) (int*&, int, int);
typedef future<void> res_type;

class ThreadPool 
{
public:
    ThreadPool();
    ~ThreadPool();
    void start();
    void stop();
    res_type push_task(FuncType f, int*& arr, int low, int high);
    void threadFunc(int qindex);
    void pending_task();
private:
    int m_thread_count;
    vector<thread> m_threads;
    vector<BlockedQueue<task_type>> m_thread_queues;
    int m_index;
};
