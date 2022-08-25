#pragma once
#include <vector>
#include <functional>
#include <thread>
#include <future>
#include <queue>
#include <mutex>
#include <condition_variable>
using namespace std;

typedef function<void()> task_type;
typedef void (*FuncType) (int*&, int, int);
typedef future<void> res_type;

class ThreadPool 
{
public:
    /*ThreadPool();
    void start();
    void stop();*/
    res_type push_task(FuncType f, int*& arr, int low, int high);
    void threadFunc();
private:
    struct TaskWithPromise {
        task_type task;
        promise<void> prom;
    };
    vector<thread> m_threads;
    queue<TaskWithPromise> m_task_queue;
    mutex m_locker;
    condition_variable m_event_holder;
    volatile bool m_work;
};
