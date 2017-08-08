//
// Created by hal on 2017/8/8.
//

#include "threadpool.h"

template <typename T>
ThreadPool<T>::ThreadPool(int thread_number, int max_requests)
        :m_thread_number(thread_number),m_max_requests(max_requests)
{
    if(thread_number<=0 ||m_max_requests<=0)
        throw std::exception();

    m_threads=new pthread_t[m_thread_number];

    if(!m_threads)
        throw std::exception();

    //创建m_thread_number个线程,并设置为分离线程
    for(int i=0;i<m_thread_number;++i){
        printf("Create the %dth thread.\n",i);

        if(pthread_create(m_threads+i,NULL,worker,this) !=0)
        {
            delete []m_threads;
            m_threads=NULL;
            throw std::exception();
        }

        if(pthread_detach(m_threads[i]))
        {
            delete []m_threads;
            m_threads=NULL;
            throw std::exception();
        }
    }
}

template <typename T>

ThreadPool::~ThreadPool() {
    delete []m_threads;
    m_stop=true;
}

template <typename T>
bool ThreadPool<T>::append(T *request) {
    //操作队列时一定要加锁,因为请求队列被全部线程共享
    MutexLockGuard mutexlock(m_mutex);
    if(m_workqueue.size() >=m_max_requests)
        return false;
    m_workqueue.push_back(request);
    delete mutexlock;
    m_queuestat.post();
    return true;
}

template <typename T>
void* ThreadPool<T>::worker(void *arg) {
    ThreadPool* pool=(ThreadPool*) arg;
    pool->run();
    return pool;
}

template <typename T>
void ThreadPool<T>::run() {
    while(!m_stop)
    {
        m_queuestat.wait();
        MutexLockGuard mutexlock(m_mutex);
        if(m_workqueue.empty())
        {
            delete mutexlock;
            continue;
        }
        T* request=m_workqueue.front();
        m_workqueue.pop_front();
        delete mutexlock;
        if(!request)
            continue;

        request->process();
    }
}