#ifndef MUTEX_LOCK_H
#define MUTEX_LOCK_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

//信号量
class sem
{
public:
    sem()
    {
        if( sem_init( &m_sem, 0, 0 ) != 0 )
        {
            throw std::exception();
        }
    }
    ~sem()
    {
        sem_destroy( &m_sem );
    }

    //等待信号量
    bool wait()
    {
        return sem_wait( &m_sem ) == 0;
    }

    //增加信号量
    bool post()
    {
        return sem_post( &m_sem ) == 0;
    }

private:
    sem_t m_sem;
};

//使用RAII机制封装互斥锁
class MutexLock
{
public:
    MutexLock()
    {
        if( pthread_mutex_init( &m_mutex, NULL ) != 0 )
        {
            throw std::exception();
        }
    }
    ~MutexLock()
    {
        pthread_mutex_destroy( &m_mutex );
    }

    void lock()
    {
        pthread_mutex_lock( &m_mutex );
    }
    void unlock()
    {
       pthread_mutex_unlock( &m_mutex );
    }

    pthread_mutex_t* getPthreadMutex()
    {
        return &m_mutex;
    }

private:
    pthread_mutex_t m_mutex;

    MutexLock(const MutexLock&);
    MutexLock& operator=(const MutexLock&);
};

class MutexLockGuard
{
public:
    explicit MutexLockGuard(MutexLock& mutex):m_mutexlock(mutex)
    {
        m_mutexlock.lock();
    }

    ~MutexLockGuard()
    {
        m_mutexlock.unlock();
    }
private:
    MutexLock& m_mutexlock;

    MutexLockGuard(const MutexLockGuard&);
    MutexLockGuard& operator=(const MutexLockGuard&);
};

//条件变量
class cond
{
public:
    cond()
    {
        if( pthread_mutex_init( &m_mutex, NULL ) != 0 )
        {
            throw std::exception();
        }
        if ( pthread_cond_init( &m_cond, NULL ) != 0 )
        {
            pthread_mutex_destroy( &m_mutex );
            throw std::exception();
        }
    }
    ~cond()
    {
        pthread_mutex_destroy( &m_mutex );
        pthread_cond_destroy( &m_cond );
    }
    bool wait()
    {
        int ret = 0;
        pthread_mutex_lock( &m_mutex );
        ret = pthread_cond_wait( &m_cond, &m_mutex );
        pthread_mutex_unlock( &m_mutex );
        return ret == 0;
    }
    bool signal()
    {
        return pthread_cond_signal( &m_cond ) == 0;
    }

private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
};

#endif
