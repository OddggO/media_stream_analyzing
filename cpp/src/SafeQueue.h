#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

// 线程安全的队列
template<typename T>
class SafeQueue{
public:
    SafeQueue(int maxSize): mMaxSize(maxSize) {}
    ~SafeQueue() = default;

    // inline void push(const T& t) {
    //     std::lock_guard<std::mutex> lck(mMtx);
    //     while (mQueue.size() >= mMaxSize) {
    //         mNotFullCond.wait(lck);
    //     }
    //     mQueue.push(std::move(t));
    //     mNotEmptyCond.notify_one();
    // }
    // inline T pop() {
    //     std::lock_guard<std::mutex> lck(mMtx);
    //     while (mQueue.size() != 0) {
    //         mNotEmptyCond.wait(lck);
    //     }
    //     T t = std::move(mQueue.front());
    //     mQueue.pop();
    //     mNotFullCond.notify_one();
    //     return t;
    // }

    void push(T t)
    {
        std::unique_lock<std::mutex> lock(mMtx);
        mNotFullCond.wait(lock, [this] {
            return mQueue.size() < mMaxSize;
        });
        mQueue.push(std::move(t));
        mNotEmptyCond.notify_one();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(mMtx);
        mNotEmptyCond.wait(lock, [this] {
            return !mQueue.empty();
        });
        T val = std::move(mQueue.front());
        mQueue.pop();
        mNotFullCond.notify_one();
        return val;
    }

private:
    std::queue<T> mQueue;
    int mMaxSize;
    std::mutex mMtx;
    std::condition_variable mNotFullCond;
    std::condition_variable mNotEmptyCond;
};
