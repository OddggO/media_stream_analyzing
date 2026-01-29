#include "TrtModelWorker.h"
#include "Log.h"

TrtModelWorker::TrtModelWorker(const std::string& threadName, const std::string& modelName, const std::string& modelFilePath, 
    int inImgWidth, int inImgHeight, TrtModel::PostProcessCb cb, 
    const std::vector<std::string>& classNames, const std::vector<int>& riskClsIdx, std::shared_ptr<MediaWorker> mediaWorker, 
    int maxResultSize): WorkerThread(threadName), 
            mQueue(maxResultSize), mMediaWorker(mediaWorker)       
{
    mTrtModel = new TrtModel(modelName, modelFilePath, inImgWidth, inImgHeight, cb, classNames, riskClsIdx); 
    LOGI("TrtModelWorker construct, threadName=%s, modelName=%s, modelFilePath=%s", 
        mThreadName.data(), mTrtModel->modelName().data(), mTrtModel->modelFilePath().data());
    mTrtModel = new TrtModel(modelName, modelFilePath, inImgWidth, inImgHeight, cb, classNames, riskClsIdx);
}

TrtModelWorker::~TrtModelWorker()
{
    LOGI("TrtModelWorker destroy, threadName=%s, modelName=%s, modelFilePath=%s", 
        mThreadName.data(), mTrtModel->modelName().data(), mTrtModel->modelFilePath().data());
    delete mTrtModel;
}

void TrtModelWorker::push(const nlohmann::json& result)
{
    mQueue.push(result);
}

nlohmann::json TrtModelWorker::pop()
{
    return mQueue.pop();
}

std::shared_ptr<MediaWorker> TrtModelWorker::mediaWorker() const
{
    return mMediaWorker;
}

void TrtModelWorker::run()
{
    LOGI("thread %s start", mThreadName.data());
    // 初始化模型
    mTrtModel->modelInit(); // 上下文的创建要和执行在一个线程...
    while (!mIsStop)
    {
        cv::Mat img = mMediaWorker->pop();
        nlohmann::json result;
        mTrtModel->inference(img, result);
        mMediaWorker->addStreamInfo(result);
        // LOGI("result: %s", result.dump().data());
        push(result);
    }
    LOGI("thread %s end", mThreadName.data());
}
