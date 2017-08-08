//
// Created by hal on 2017/8/8.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include <stdio.h>
#include <cstdio>
#include <exception>
#include <pthread.h>

#include "MutexLock.h"

template <typename T>
class ThreadPool
{
public:
    ThreadPool(int thread_number=8, int max_requests=10000);
    ~ThreadPool();

    //向请求队列中添加任务
    bool append(T* request);

private:
    //工作线程运行的函数,不断从工作队列中取出任务并执行
    static void* worker(void* arg);
    void run();

private:
    int m_thread_number;        //线程池中的线程数
    int m_max_requests;         //请求队列中允许的最大请求数
    pthread_t* m_threads;       //描述线程池的线程组,其大小为m_thread_number
    std::list<T*> m_workqueue;  //请求队列

    MutexLock m_mutex;          //保护请求队列的互斥锁

    sem m_queuestat;            //是否有任务需要处理
    bool m_stop;                //是否结束线程
};


#endif //DEMO_THREADPOOL_H
