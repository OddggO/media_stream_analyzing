#include "MessageWorker.h"
#include "Log.h"
#define CLIENT_READ_TIMEOUT 5
#define POST_FAILED_SLEEP_TIME 2

MessageWorker::MessageWorker(const std::string& threadName, const std::string& ip, const int& port, const int& threadNum, 
            const std::vector<std::shared_ptr<TrtModelWorker>>& trtModelWorkers): WorkerThread(threadName), 
            mIp(ip), mPort(port), mThreadNum(threadNum), mTrtModelWorkers(trtModelWorkers)
{
    LOGI("MessageWorker construct, threadName=%s, ip=%s, port=%d, threadNum=%d", 
    mThreadName.data(), mIp.data(), mPort, mThreadNum);
    // mClient = new httplib::Client(mIp, mPort);
    // mClient->set_read_timeout(CLIENT_READ_TIMEOUT, 0);
}

MessageWorker::~MessageWorker()
{
    LOGI("MessageWorker destroy, threadName=%s, ip=%s, port=%d, threadNum=%d", 
    mThreadName.data(), mIp.data(), mPort, mThreadNum);
}

void MessageWorker::run()
{
    LOGI("thread %s start", mThreadName.data());
    while (!mIsStop) 
    {
        for (const auto& worker : mTrtModelWorkers)
        {
            // if (worker->isStop()) {
            //     continue;
            // }
            // LOGI("begin to post worker's[%s] message", worker->threadName().data());
            nlohmann::json result = worker->pop(); // TODO: 要设置超时时间, 否则会一直卡住. 可以使用默认参数
            // mClient->Post(worker->mediaWorker()->destPath().data(), result.dump(), "application/json");
            httplib::Client client(worker->mediaWorker()->destHost(), worker->mediaWorker()->destPort());
            httplib::Result res = client.Post(worker->mediaWorker()->destPath().data(), result.dump(), "application/json");
            if (!res) {
                LOGI("请求失败, 无法连接到服务器. url: %s", worker->mediaWorker()->destUrl().data());
                // return;
                // if (worker->isStop()) {
                //     continue;
                // }
                sleep(POST_FAILED_SLEEP_TIME);
            }

            if (res->status == 200) {
                // LOGI("发送成功，发送内容: %s", result.dump().data());
                nlohmann::json respJson = nlohmann::json::parse(res->body);
                // LOGI("响应内容: %s", respJson.dump());
                // LOGI("响应内容: %s", res->body.data());
            } else {
                LOGI("请求失败, 错误码: %d", res->status);
            }
        }
        // sleep(1);
    }
    LOGI("thread %s end", mThreadName.data());
}
