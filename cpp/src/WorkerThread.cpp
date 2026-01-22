#include "WorkerThread.h"
#include "Log.h"

WorkerThread::WorkerThread(const std::string& threadName): mThreadName(threadName), mIsStop(true), mIsDetach(false)
{
    LOGI("WorkerThread() construct, threadName=%s", mThreadName.data());
}

WorkerThread::~WorkerThread()
{
    if (!mIsStop || mThread.joinable()) {
        stop();
    }
    LOGI("WorkerThread() destroy, threadName=%s", mThreadName.data());
}

void WorkerThread::start()
{
    LOGI("WorkerThread %s start", mThreadName.data());
    mIsStop = false;
    mThread = std::thread(&WorkerThread::run, this);
}

void WorkerThread::detach()
{
    LOGI("WorkerThread %s detach", mThreadName.data());
    if (!mIsStop && mThread.joinable()) {
        mThread.detach();
        mIsDetach = true;
    }
}

void WorkerThread::join()
{
    LOGI("WorkerThread %s join", mThreadName.data());
    if (!mIsStop && mThread.joinable()) {
        mThread.join();
    }
    mIsStop = true;
}

void WorkerThread::stop()
{
    LOGI("WorkerThread %s stop", mThreadName.data());
    mIsStop = true;
    if (mThread.joinable()) {
        mThread.join();
    }
}

bool WorkerThread::isStop()
{
    return mIsStop;
}

std::string WorkerThread::threadName() const
{
    return mThreadName;
}
