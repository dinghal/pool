//
// Created by hal on 2017/8/8.
//

#ifndef PROCESSPOOL_H
#define PROCESSPOOL_H

#include "common.h"

class process
{
public:
    process():m_pid(-1){}

public:
    pid_t m_pid;
    int m_pipefd[2];
};

template <typename T>
class processpool
{
private:
    //将构造函数私有化,只能使用后面的静态函数才能创建processpool实例
    processpool(int listenfd,int process_number =8);

public:

    //保证程序只有一个进程池实例
    static processpool<T>* create(int listenfd,int process_number=8){
        if(!m_instance)
        {
            m_instance=new processpool<T>(listenfd,process_number);
        }
        return m_instance;
    }

    ~processpool()
    {
        delete []m_sub_process;
    }

    void run();

private:
    void setup_sig_pipe();
    void run_parent();
    void run_child();

private:
    //进程池允许的最大的子进程数量
    static const int MAX_PROCESS_NUMBER=16;

    //每个子进程最多可处理的客户连接数
    static const int USER_PER_PROCESS=65536;

    //epoll最多能处理的事件数
    static const int MAX_EVENT_NUMBER=10000;

    //进程池中的进程总数
    int m_process_number;

    //子进程在池中的序号,从0开始
    int m_index;

    //每个子进程都有一个epoll内核事件表,用m_epollfd标识
    int m_epolled;

    //监听socket
    int m_listenfd;

    //子进程通过m_stop来决定是否终止运行
    int m_stop;

    //保存所有子进程的描述信息
    process* m_sub_process;

    //进程池静态实例
    static processpool<T>* m_instance;
};

#endif //PROCESSPOOL_H
