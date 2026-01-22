#pragma once
#include <thread>
#include <string>

class WorkerThread{
public:
    WorkerThread(const std::string& threadName);
    virtual ~WorkerThread();
    void start();
    void join();
    void detach();
    void stop();
    bool isStop();

    std::string threadName() const;
protected:
    virtual void run() = 0; // 线程入口函数
    
protected:
    std::string mThreadName;
    std::thread mThread;
    bool mIsStop;
    bool mIsDetach;
};
